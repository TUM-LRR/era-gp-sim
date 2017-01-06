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
  // calculate region for (max) 64bit memory cells
  // region should not exceed real memory size
  std::size_t start = address - (address % (64 / 8));
  std::size_t end = std::max(
      start + (length - (length % (64 / 8)) + (64 / 8)) - 1, _memorySize - 1);

  // if the memory that is hold in cache is changed, invalidate cache
  // new value intersects with the beginning of the cached region
  bool overlap_beginning =
      (address <= _memoryCacheBaseAddress + _memoryCacheSize &&
       address + length >= _memoryCacheBaseAddress + _memoryCacheSize);
  // new value intersects with the ending of the cached region
  bool overlap_ending = (address <= _memoryCacheBaseAddress &&
                         address + length >= _memoryCacheBaseAddress);
  // new value is completly inside cached region
  bool overlap_middle =
      (address >= _memoryCacheBaseAddress &&
       address + length <= _memoryCacheBaseAddress + _memoryCacheSize);

  if (overlap_beginning || overlap_middle || overlap_ending) {
    _memoryCacheValid = false;
  }

  // update calculated region
  emit dataChanged(this->index(start), this->index(end));
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
  Q_UNUSED(parent)
  return _memorySize;
}


QVariant
MemoryComponentPresenter::data(const QModelIndex &index, int role) const {
  // check boundaries
  assert::that(index.isValid());

  switch (role) {
    case AddressRole: return dataAdress(index, role);
    case InfoRole: return dataInfo(index, role);
    default: return dataMemory(index, role);
  }
}


QVariant
MemoryComponentPresenter::dataAdress(const QModelIndex &index, int role) const {
  // format address as hex value and return it
  return QString("%1")
      .arg(index.row(), 4, 16, QLatin1Char('0'))
      .toUpper()
      .prepend("0x");
}


QVariant
MemoryComponentPresenter::dataMemory(const QModelIndex &index, int role) const {
  // get role as a string because there is more information in it
  QString role_string = MemoryComponentPresenter::roleNames().value(role);

  int number_of_bits = 8;
  if (role_string.endsWith("16")) number_of_bits = 16;
  if (role_string.endsWith("32")) number_of_bits = 32;


  int memory_address = index.row();
  int memory_length = number_of_bits / 8;

  // return empty string if cell is not displayed
  if (memory_address % memory_length != 0) return QString("");


  if (memory_address + memory_length <= _memorySize) {
    MemoryValue memory_cell =
        getMemoryValueCached(memory_address, memory_length);

    std::string memoryStringValue;
    if (role_string.startsWith("bin")) {
      memoryStringValue = StringConversions::toBinString(memory_cell);
    }
    // not yet implemented by conversions
    // else if (role_string.startsWith("oct"))
    //  stringvalue = StringConversions::toOctString(memory_cell);
    else if (role_string.startsWith("hex")) {
      memoryStringValue = StringConversions::toHexString(memory_cell);
    } else if (role_string.startsWith("decs")) {
      memoryStringValue = StringConversions::toSignedDecString(memory_cell);
    } else if (role_string.startsWith("dec")) {
      memoryStringValue = StringConversions::toUnsignedDecString(memory_cell);
    } else {
      memoryStringValue = "unknown format";
    }

    return QString::fromStdString(memoryStringValue);
  } else {
    // wrong address
    return QString("");
  }
}


QVariant
MemoryComponentPresenter::dataInfo(const QModelIndex &index, int role) const {
  // TODO fetch value from core
  return QString("");
}


MemoryValue MemoryComponentPresenter::getMemoryValueCached(
    const std::size_t memory_address, const std::size_t memory_length) const {
  // check for cache
  if (_memoryCacheValid && memory_address >= _memoryCacheBaseAddress &&
      memory_address + memory_length <=
          _memoryCacheBaseAddress + _memoryCacheSize) {
    // cache hit -> get memory value from cache

    // fetch data from cache
    return _memoryCache.subSet(
        (memory_address - _memoryCacheBaseAddress) * 8,
        (memory_address - _memoryCacheBaseAddress + memory_length) * 8);
  } else {
    // cache miss -> update cache from core

    // calculate new cache region
    _memoryCacheBaseAddress = memory_address;
    _memoryCacheSize = memory_length;

    // add some offset around but
    // cache size should not exceed real memory boundaries
    // (done this way to prevent overflows)
    const std::size_t offset = 10;
    if (_memoryCacheBaseAddress < offset) {
      _memoryCacheBaseAddress = 0;
    } else {
      _memoryCacheBaseAddress -= offset;
    }
    if (_memoryCacheBaseAddress + _memoryCacheSize >=
        _memorySize - offset * 2) {
      _memoryCacheSize = _memorySize - _memoryCacheBaseAddress;
    } else {
      _memoryCacheSize += offset * 2;
    }

    // fetch cache from core
    _memoryCache =
        _memoryAccess
            .getMemoryValueAt(_memoryCacheBaseAddress, _memoryCacheSize)
            .get();
    // set cache status to valid
    _memoryCacheValid = true;

    std::size_t subset_start = (memory_address - _memoryCacheBaseAddress) * 8;
    std::size_t subset_end =
        (memory_address - _memoryCacheBaseAddress + memory_length) * 8;
    return _memoryCache.subSet(subset_start, subset_end);
  }
}


QHash<int, QByteArray> MemoryComponentPresenter::roleNames() const {
  // connect TableColumns in View with columns in this model
  QHash<int, QByteArray> roles;
  roles[AddressRole] = "address";
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
