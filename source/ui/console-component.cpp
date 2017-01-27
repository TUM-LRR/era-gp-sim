/*
  * C++ Assembler Interpreter
  * Copyright (C) 2016 Chair of Computer Architecture
  * at Technical University of Munich
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program. If not, see <http://www.gnu.org/licenses/>.
  */

#include "ui/console-component.hpp"

#include <string>

#include "core/conversions.hpp"

ConsoleComponent::ConsoleComponent(QQmlContext* context,
                                   MemoryAccess memoryAccess)
: QObject()
, _start(1)
, _length(0)
, _interruptAddress(0)
, _memoryAccess(memoryAccess)
, _deleteBuffer(false)
, _interruptTriggered(false) {
  context->setContextProperty("consoleComponent", this);
}

void ConsoleComponent::appendText(QString text) {
  if (_deleteBuffer) {
    deleteTextInMemory();
  }
  for (auto i : Utility::range<size_t>(0, text.length())) {
    if (_start + _length > _memoryAccess.getMemorySize().get()) {
      // Too long
      return;
    }
    QChar qchar = text.at(i);
    MemoryValue m = conversions::convert(qchar.unicode(), 8);
    _memoryAccess.putMemoryValueAt(_start + _length, m);
    // A nullbyte signals the last byte of the console, so we should not have
    // one inside the text.
    if (qchar == '\0') return;

    // Don't count a nullbyte as length, otherwise text can't be appended
    // afterwards!
    ++_length;
  }
}

QString ConsoleComponent::getText() {
  QString text = "";
  for (_length = 0; (_start + _length) < _memoryAccess.getMemorySize().get();
       ++_length) {
    MemoryValue memoryValue =
        _memoryAccess.getMemoryValueAt(_start + _length).get();
    uint8_t value = conversions::convert<uint8_t>(memoryValue);

    // Stop when a null character is read.
    if (value == 0) {
      break;
    }

    text += value;
  }
  return text;
}

void ConsoleComponent::setStart(size_t start) {
  if (_memoryAccess.getMemorySize().get() >= start) {
    deleteTextInMemory();
    _start = start;
    _length = 0;
  }
}

void ConsoleComponent::setInterruptAddress(size_t interruptAddress) {
  _interruptAddress = interruptAddress;
}

ConsoleComponent::size_t ConsoleComponent::getInterruptAddress() {
  return _interruptAddress;
}

ConsoleComponent::size_t ConsoleComponent::getStart() {
  return _start;
}

ConsoleComponent::size_t ConsoleComponent::getLength() {
  return _length;
}

void ConsoleComponent::setDeleteBuffer(bool deleteBuffer) {
  _deleteBuffer = deleteBuffer;
  emit settingsChanged();
}

bool ConsoleComponent::deleteBuffer() {
  return _deleteBuffer;
}

bool ConsoleComponent::checkInterrupt() {
  // The interrupt is edge triggered to prevent flooding the console.
  MemoryValue memoryValue =
      _memoryAccess.getMemoryValueAt(_interruptAddress).get();
  uint8_t value = conversions::convert<uint8_t>(memoryValue);
  bool interruptValue = value & 1;
  if (_interruptTriggered) {
    // reset the triggered flag (false if the interrupt was reset)
    _interruptTriggered = interruptValue;
    return false;
  }
  return interruptValue;
}

void ConsoleComponent::setInterrupt() {
  MemoryValue memoryValue =
      _memoryAccess.getMemoryValueAt(_interruptAddress).get();
  uint8_t value = conversions::convert<uint8_t>(memoryValue);
  MemoryValue resetValue = conversions::convert(value | 1, 8);
  _memoryAccess.putMemoryValueAt(_interruptAddress, resetValue);
}

void ConsoleComponent::resetInterrupt() {
  MemoryValue memoryValue =
      _memoryAccess.getMemoryValueAt(_interruptAddress).get();
  uint8_t value = conversions::convert<uint8_t>(memoryValue);
  MemoryValue resetValue = conversions::convert(value & 254, 8);
  _memoryAccess.putMemoryValueAt(_interruptAddress, resetValue);
}

void ConsoleComponent::deleteTextInMemory() {
  if (_length > 0) {
    MemoryValue zero(_length * 8);
    _memoryAccess.putMemoryValueAt(_start, zero);
    _length = 0;
  }
}
