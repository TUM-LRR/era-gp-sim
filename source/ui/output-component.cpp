/* C++ Assembler Interpreter
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
 * along with this program. If not, see http://www.gnu.org/licenses/.
 */

#include "ui/output-component.hpp"
#include "core/memory-manager.hpp"
#include "core/memory-value.hpp"

OutputComponent::OutputComponent(MemoryManager &memoryManager,
                                 MemoryAccess &memoryAccess,
                                 QQmlContext *projectContext,
                                 QObject *parent)
: _memoryAccess(memoryAccess) {
  // Make this component available to all QML output items.
  projectContext->setContextProperty("outputComponent", this);

  // Add all the output items and their default values.
  _outputItemsInformation.push_back(
      QVariant(QMap<QString, QVariant>{{"type", "LightStrip"},
                                       {"baseAddress", QVariant(0)},
                                       {"numberOfStrips", QVariant(8)}}));
  _outputItemsInformation.push_back(
      QVariant(QMap<QString, QVariant>{{"type", "SevenSegment"},
                                       {"baseAddress", QVariant(0)},
                                       {"numberOfDigits", QVariant(2)}}));
  _outputItemsInformation.push_back(
      QVariant(QMap<QString, QVariant>{{"type", "TextConsole"},
                                       {"baseAddress", QVariant(0)},
                                       {"textMode", QVariant(0)}}));
}


QVariant OutputComponent::getOutputItem(int index) const {
  return _outputItemsInformation[index];
}


void OutputComponent::setOutputItemProperty(int ouputItemIndex,
                                            QString property,
                                            QVariant newValue) {
  // Write the new value into the model.
  QMap<QString, QVariant> ouputItemProperties =
      _outputItemsInformation[ouputItemIndex].toMap();
  ouputItemProperties[property] = newValue;
  _outputItemsInformation[ouputItemIndex] = ouputItemProperties;
  // Notify all instances of all output items that their settings might have
  // changed.
  emit outputItemSettingsChanged();
}


void OutputComponent::updateMemory(std::size_t address, std::size_t length) {
  emit memoryChanged(QVariant::fromValue(address), QVariant::fromValue(length));
}

void OutputComponent::putMemoryValue(int address,
                                     QVector<bool> memoryContentBitVector) {
  // Round up size of altered content up to bytes.
  size_t memoryValueSize = memoryContentBitVector.size() +
                           (8 - (memoryContentBitVector.size() % 8)) % 8;
  // Create new MemoryValue.
  MemoryValue memoryContent(memoryValueSize);
  // Insert the bit values of the altered content into the newly create
  // MemoryValue.
  for (size_t index = 0; index < memoryContentBitVector.size(); ++index) {
    memoryContent.put(index, memoryContentBitVector.at(index));
  }
  // Pass new value to Core.
  _memoryAccess.putMemoryValueAt(address, memoryContent);
}

QVector<bool> OutputComponent::getMemoryContent(int address, int length) const {
  // Fetch memory content from Core.
  MemoryValue content = _memoryAccess.getMemoryValueAt(address, length).get();
  // Convert memory content to a Bit-QVector.
  QVector<bool> contentVector;
  for (auto it = content.cbegin(); it != content.cend(); ++it) {
    contentVector.append(*it);
  }
  return contentVector;
}
