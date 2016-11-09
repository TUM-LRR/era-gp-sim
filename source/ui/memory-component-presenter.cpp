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

#include "ui/memory-component-presenter.hpp"
#include <iostream>

#include "common/assert.hpp"
#include "common/string-conversions.hpp"
#include "core/memory-value.hpp"

MemoryComponentPresenter::MemoryComponentPresenter(MemoryAccess access,
                                                   MemoryManager manager,
                                                   QQmlContext *projectContext,
                                                   QObject *parent)
: QAbstractListModel(parent)
, _memoryAccess(access)
, _memoryManager(manager)
, _memorySize(access.getMemorySize().get()) {
  projectContext->setContextProperty("memoryModel", this);
}


MemoryComponentPresenter::~MemoryComponentPresenter() {
}

void MemoryComponentPresenter::onMemoryChanged(std::size_t address,
                                               std::size_t length) {
  emit dataChanged(this->index(address), this->index(address + length - 1));
}

void MemoryComponentPresenter::setValue(int address, QString number) {
  _memoryAccess.putMemoryValueAt(
      address,
      *StringConversions::hexStringToMemoryValue(number.toStdString(), 8));
}

void MemoryComponentPresenter::setContextInformation(int addressStart,
                                                     int length,
                                                     int identifier) {
  // TODO
}


int MemoryComponentPresenter::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return _memorySize;
}

QVariant
MemoryComponentPresenter::data(const QModelIndex &index, int role) const {
  // check boundaries
  assert::that(index.isValid());

  switch (role) {
    case AddressRole8:
    case AddressRole16:
    case AddressRole32: {
      // format index as hex value and return it
      return QString("%1")
          .arg(index.row() * numberOfBytes(role), 4, 16, QLatin1Char('0'))
          .toUpper()
          .prepend("0x");
    }
    case ValueRole: {
      MemoryValue memory_cell =
          _memoryAccess.getMemoryValueAt(index.row()).get();
      std::string stringvalue = StringConversions::toHexString(memory_cell);
      return QString::fromStdString(stringvalue);
    }
    case InfoRole: {
      // TODO fetch value from core
      return QString("");
    }
    default: {
      // unknown column role, return empty value
      return QVariant();
    }
  }
}
QHash<int, QByteArray> MemoryComponentPresenter::roleNames() const {
  // connect TableColumns in View with columns in this model
  QHash<int, QByteArray> roles;
  roles[AddressRole8] = "address8";
  roles[AddressRole16] = "address16";
  roles[AddressRole32] = "address32";
  roles[ValueRole] = "value";
  roles[InfoRole] = "info";
  return roles;
}

int MemoryComponentPresenter::numberOfBytes(int role) const {
  switch (role) {
    case AddressRole8: return 1;
    case AddressRole16: return 2;
    case AddressRole32: return 4;
    default: return 0;
  }
}
