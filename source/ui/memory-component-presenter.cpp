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

#include <algorithm>
#include <string>

#include "common/assert.hpp"
#include "common/string-conversions.hpp"
#include "core/memory-value.hpp"
#include "ui/gui-project.hpp"

MemoryComponentPresenter::MemoryComponentPresenter(const MemoryAccess &access,
                                                   const MemoryManager &manager,
                                                   QQmlContext *projectContext,
                                                   QObject *parent)
: QAbstractListModel(parent)
, _memoryAccess(access)
, _memoryManager(manager)
, _memorySize(access.getMemorySize().get())
, _memoryCacheOffset(std::min(static_cast<size_t>(10), _memorySize / 2)) {
  projectContext->setContextProperty("memoryModel", this);
}

void MemoryComponentPresenter::onMemoryChanged(size_t address, size_t length) {
  // calculate region for (max) 64bit memory cells
  // region should not exceed real memory size
  size_t start = address - (address % (64 / 8));
  size_t end = std::min(start + (length - (length % (64 / 8)) + (64 / 8)) - 1,
                        _memorySize - 1);

  // if the memory that is hold in cache is changed, invalidate cache
  bool overlapAddress = (address >= _memoryCacheBaseAddress &&
                         address <= _memoryCacheBaseAddress + _memoryCacheSize);

  bool overlapLength =
      ((address + length) >= _memoryCacheBaseAddress &&
       (address + length) <= _memoryCacheBaseAddress + _memoryCacheSize);

  if (overlapAddress || overlapLength) {
    _memoryCacheValid = false;
  }

  // update calculated region
  emit dataChanged(this->index(start), this->index(end));
}

void MemoryComponentPresenter::onMemorySizeChanged(size_t newSize) {
  if (newSize > _memorySize) {
    beginInsertRows(QModelIndex(), _memorySize, newSize - 1);
    _memorySize = newSize;
    endInsertRows();
  } else if (newSize < _memorySize) {
    beginRemoveRows(QModelIndex(), newSize, _memorySize - 1);
    _memorySize = newSize;
    endRemoveRows();
  }
}

void MemoryComponentPresenter::setValue(int address,
                                        const QString &value,
                                        int numberOfBits,
                                        const QString &role) {
  assert::that(!role.isEmpty());

  std::string string("0");
  if (!value.isEmpty()) {
    string = value.toStdString();
  }
  auto dataFormat = _roleToDataFormat(role);
  assert::that(GuiProject::getStringToMemoryConversions().contains(dataFormat));

  auto converter = GuiProject::getStringToMemoryConversions()[dataFormat];
  auto memory = converter(value.toStdString(), numberOfBits);

  // if the conversions returned a value, write the value, otherwise zero.
  if (memory) {
    _memoryAccess.putMemoryValueAt(address, *memory);
  } else {
    _memoryAccess.putMemoryValueAt(address, MemoryValue(numberOfBits));
  }
}


void MemoryComponentPresenter::setContextInformation(int addressStart,
                                                     int length,
                                                     int identifier) {
  // TODO
}


int MemoryComponentPresenter::rowCount(const QModelIndex &) const {
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
  return QString("0x%1").arg(index.row(), 4, 16, QLatin1Char('0'));
}

QVariant
MemoryComponentPresenter::dataMemory(const QModelIndex &index, int role) const {
  // get role as a string because there is more information in it
  QString roleString = MemoryComponentPresenter::roleNames().value(role);

  int numberOfBits = 8;
  if (roleString.endsWith("16")) numberOfBits = 16;
  if (roleString.endsWith("32")) numberOfBits = 32;
  if (roleString.endsWith("64")) numberOfBits = 64;


  int memoryAddress = index.row();
  int memoryLength = numberOfBits / 8;

  // return empty string if cell is not displayed
  if (memoryAddress % memoryLength != 0) return QString("");


  if (memoryAddress + memoryLength <= _memorySize) {
    MemoryValue memoryCell = getMemoryValueCached(memoryAddress, memoryLength);

    // auto dataFormat = role_string.remove(QRegularExpression("\\d+"));
    auto dataFormat = _roleToDataFormat(roleString);
    auto converter = GuiProject::getMemoryToStringConversions()[dataFormat];
    auto memoryValueString = converter(memoryCell);

    return QString::fromStdString(memoryValueString);
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
    const size_t memoryAddress, const size_t memoryLength) const {
  // check for cache
  if (_memoryCacheValid && memoryAddress >= _memoryCacheBaseAddress &&
      memoryAddress + memoryLength <=
          _memoryCacheBaseAddress + _memoryCacheSize) {
    // cache hit -> get memory value from cache

    // fetch data from cache
    return _memoryCache.subSet(
        (memoryAddress - _memoryCacheBaseAddress) * 8,
        (memoryAddress - _memoryCacheBaseAddress + memoryLength) * 8);
  } else {
    // cache miss -> update cache from core

    // calculate new cache region
    _memoryCacheBaseAddress = memoryAddress;
    _memoryCacheSize = memoryLength;

    _updateCache();

    size_t subsetStart = (memoryAddress - _memoryCacheBaseAddress) * 8;
    size_t subsetEnd =
        (memoryAddress - _memoryCacheBaseAddress + memoryLength) * 8;
    return _memoryCache.subSet(subsetStart, subsetEnd);
  }
}


QHash<int, QByteArray> MemoryComponentPresenter::roleNames() const {
  // clang-format off
  static QHash<int, QByteArray> roles = {
    {AddressRole, "address"},
    {ValueRoleBin8, "BinaryData8"},
    {ValueRoleBin16, "BinaryData16"},
    {ValueRoleBin32, "BinaryData32"},
    {ValueRoleBin64, "BinaryData64"},
    {ValueRoleHex8, "HexData8"},
    {ValueRoleHex16, "HexData16"},
    {ValueRoleHex32, "HexData32"},
    {ValueRoleHex64, "HexData64"},
    {ValueRoleDec8, "UnsignedDecimalData8"},
    {ValueRoleDec16, "UnsignedDecimalData16"},
    {ValueRoleDec32, "UnsignedDecimalData32"},
    {ValueRoleDec64, "UnsignedDecimalData64"},
    {ValueRoleDecS8, "SignedDecimalData8"},
    {ValueRoleDecS16, "SignedDecimalData16"},
    {ValueRoleDecS32, "SignedDecimalData32"},
    {ValueRoleDecS64, "SignedDecimalData64"},
    {InfoRole, "info"}
  };
  // clang-format on
  return roles;
}

void MemoryComponentPresenter::_updateCache() const {
  // add some offset around but
  // cache size should not exceed real memory boundaries
  // (done this way to prevent overflows)
  if (_memoryCacheBaseAddress < _memoryCacheOffset) {
    _memoryCacheBaseAddress = 0;
  } else {
    _memoryCacheBaseAddress -= _memoryCacheOffset;
  }
  if (_memoryCacheBaseAddress + _memoryCacheSize >=
      _memorySize - _memoryCacheOffset * 2) {
    _memoryCacheSize = _memorySize - _memoryCacheBaseAddress;
  } else {
    _memoryCacheSize += _memoryCacheOffset * 2;
  }

  // fetch cache from core
  _memoryCache =
      _memoryAccess.getMemoryValueAt(_memoryCacheBaseAddress, _memoryCacheSize)
          .get();
  // set cache status to valid
  _memoryCacheValid = true;
}

QString MemoryComponentPresenter::_roleToDataFormat(QString role) {
  // remove digits at line ending
  return role.remove(QRegularExpression("\\d+$"));
}
