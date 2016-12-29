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

#include <QDebug>
#include "common/assert.hpp"
#include "common/string-conversions.hpp"
#include "core/memory-value.hpp"

MemoryComponentPresenter::MemoryComponentPresenter(MemoryAccess access,
                                                   MemoryManager manager,
                                                   QQmlContext *projectContext,
                                                   QObject *parent)
: QAbstractTableModel(parent)
, _memoryAccess(access)
, _memoryManager(manager)
, _memorySize(access.getMemorySize().get()) {
  projectContext->setContextProperty("memoryModel", this);
}


MemoryComponentPresenter::~MemoryComponentPresenter() {
}


void MemoryComponentPresenter::onMemoryChanged(std::size_t address,
                                               std::size_t length) {
  int start = address - (address % (64 / 8));
  int end = start + (length - (length % (64 / 8)) + (64 / 8));
  // put updated memory in cache
  _memoryCacheBaseAddress = start;
  _memoryCacheSize = end - start;

  if (_memoryCacheSize > _memorySize) _memoryCacheSize = _memorySize;

  _memoryCache =
      _memoryAccess.getMemoryValueAt(_memoryCacheBaseAddress, _memoryCacheSize)
          .get();

  qDebug() << "changed " << address << length;
  emit dataChanged(this->index(start, 0), this->index(end, 0));

  /*emit dataChanged(this->index(address - (address % (8 / 8)), 0),
                   this->index(address - (address % (8 / 8)) + length - 1,
                               0));//  8bit
  emit dataChanged(this->index(address - (address % (16 / 8)), 1),
                   this->index(address - (address % (16 / 8)) + length - 1,
                               1));// 16bit
  emit dataChanged(this->index(address - (address % (32 / 8)), 2),
                   this->index(address - (address % (32 / 8)) + length - 1,
                               2));// 32bit*/
}


void MemoryComponentPresenter::setValue(int address,
                                        QString number,
                                        int length_bit,
                                        QString presentation) {
  if (presentation.startsWith("bin"))
    _memoryAccess.putMemoryValueAt(address,
                                   *StringConversions::binStringToMemoryValue(
                                       number.toStdString(), length_bit));
  // not yet implemented by conversions
  /*else if (pres.startsWith("oct"))
    _memoryAccess.putMemoryValueAt(address,
                                   *StringConversions::octStringToMemoryValue(
                                       number.toStdString(), length_bit));*/
  else if (presentation.startsWith("hex"))
    _memoryAccess.putMemoryValueAt(address,
                                   *StringConversions::hexStringToMemoryValue(
                                       number.toStdString(), length_bit));
  else if (presentation.startsWith("decs"))
    _memoryAccess.putMemoryValueAt(
        address,
        *StringConversions::unsignedDecStringToMemoryValue(number.toStdString(),
                                                           length_bit));
  else if (presentation.startsWith("dec"))
    _memoryAccess.putMemoryValueAt(
        address,
        *StringConversions::signedDecStringToMemoryValue(number.toStdString(),
                                                         length_bit));
  return;
}


void MemoryComponentPresenter::setContextInformation(int addressStart,
                                                     int length,
                                                     int identifier) {
  // TODO
}


int MemoryComponentPresenter::rowCount(const QModelIndex &parent) const {
  // Q_UNUSED(parent)
  return _memorySize;
}


int MemoryComponentPresenter::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  // dynamic number of columns in every single view -> default value
  return 3;
}


QVariant
MemoryComponentPresenter::data(const QModelIndex &index, int role) const {
  // check boundaries
  qDebug() << index.row() << index.column();
  assert::that(index.isValid());

  // get role as a string because there is more information in it
  QString role_string = MemoryComponentPresenter::roleNames().value(role);

  int number_of_bits = 8;
  if (role_string.endsWith("16")) number_of_bits = 16;
  if (role_string.endsWith("32")) number_of_bits = 32;

  if (role_string.startsWith("address")) {
    // format index as hex value and return it
    return QString("%1")
        .arg(index.row(), 4, 16, QLatin1Char('0'))
        .toUpper()
        .prepend("0x");
  }

  if (role == InfoRole) {
    // TODO fetch value from core
    return QString("");
  }

  int memory_address = index.row();//* (number_of_bits / 8);
  int memory_length = number_of_bits / 8;

  MemoryValue memory_cell;
  // check for cache
  if (false && memory_address >= _memoryCacheBaseAddress &&
      memory_address + memory_length <=
          _memoryCacheBaseAddress + _memoryCacheSize) {
    // cache hit
    // qDebug() << "hit" << memory_address;
    memory_cell = _memoryCache.subSet(
        memory_address - _memoryCacheBaseAddress,
        memory_address - _memoryCacheBaseAddress + number_of_bits);
  } else {
    // cache miss -> fetch from core
    // qDebug() << "miss" << memory_address;
    memory_cell =
        _memoryAccess.tryGetMemoryValueAt(memory_address, memory_length).get();
  }

  std::string stringValue;
  if (role_string.startsWith("bin")) {
    stringValue = StringConversions::toBinString(memory_cell);
  }
  // not yet implemented by conversions
  // else if (role_string.startsWith("oct"))
  //  stringvalue = StringConversions::toOctString(memory_cell);
  else if (role_string.startsWith("hex")) {
    stringValue = StringConversions::toHexString(memory_cell);
  } else if (role_string.startsWith("decs")) {
    stringValue = StringConversions::toSignedDecString(memory_cell);
  } else if (role_string.startsWith("dec")) {
    stringValue = StringConversions::toUnsignedDecString(memory_cell);
  } else {
    stringValue = "unknown format";
  }

  return QString::fromStdString(stringValue);
}


QHash<int, QByteArray> MemoryComponentPresenter::roleNames() const {
  // connect TableColumns in View with columns in this model
  QHash<int, QByteArray> roles;
  roles[AddressRole8] = "address8";
  roles[AddressRole16] = "address16";
  roles[AddressRole32] = "address32";
  roles[ValueRoleBin8] = "bin8";
  roles[ValueRoleBin16] = "bin16";
  roles[ValueRoleBin32] = "bin32";
  roles[ValueRoleOct8] = "oct8";
  roles[ValueRoleOct16] = "oct16";
  roles[ValueRoleOct32] = "oct32";
  roles[ValueRoleHex8] = "hex8";
  roles[ValueRoleHex16] = "hex16";
  roles[ValueRoleHex32] = "hex32";
  roles[ValueRoleDec8] = "dec8";
  roles[ValueRoleDec16] = "dec16";
  roles[ValueRoleDec32] = "dec32";
  roles[ValueRoleDecS8] = "decs8";
  roles[ValueRoleDecS16] = "decs16";
  roles[ValueRoleDecS32] = "decs32";
  roles[InfoRole] = "info";
  return roles;
}
