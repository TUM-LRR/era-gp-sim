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

#include <cassert>
#include <string>

#include "arch/common/datatype-information.hpp"

DataTypeInformation::DataTypeInformation() noexcept : _size(0) {
}

DataTypeInformation::DataTypeInformation(InformationInterface::Format& data) {
  _deserialize(data);
}

DataTypeInformation::DataTypeInformation(const std::string& name, size_t size) {
  // To check constraints
  this->name(name);
  this->size(size);
}

bool DataTypeInformation::operator==(const DataTypeInformation& other) const
    noexcept {
  if (this->_name != other._name) return false;
  if (this->_size != other._size) return false;

  return true;
}

bool DataTypeInformation::operator!=(const DataTypeInformation& other) const
    noexcept {
  return !(*this == other);
}

DataTypeInformation&
DataTypeInformation::deserialize(InformationInterface::Format& data) {
  _deserialize(data);
  return *this;
}

DataTypeInformation& DataTypeInformation::name(const std::string& name) {
  assert(!name.empty());
  _name = name;

  return *this;
}

const std::string& DataTypeInformation::getName() const noexcept {
  assert(hasName());
  return _name;
}

bool DataTypeInformation::hasName() const noexcept {
  return !_name.empty();
}

DataTypeInformation& DataTypeInformation::size(size_t size) {
  assert(size > 0);
  _size = size;

  return *this;
}

DataTypeInformation::size_t DataTypeInformation::getSize() const noexcept {
  assert(hasSize());
  return _size;
}

bool DataTypeInformation::hasSize() const noexcept {
  return _size > 0;
}


bool DataTypeInformation::isValid() const noexcept {
  return !_name.empty() && _size > 0;
}

void DataTypeInformation::_deserialize(InformationInterface::Format& data) {
  assert(data.count("name"));
  assert(data.count("size"));

  name(data["name"]);
  size(data["size"]);
}
