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

DataTypeInformation::DataTypeInformation(const std::string& name, size_t size) {
  // To check constraints
  this->name(name);
  this->size(size);
}

DataTypeInformation& DataTypeInformation::name(const std::string& name) {
  assert(!_name.empty());
  _name = name;

  return *this;
}

const std::string& DataTypeInformation::getName() const noexcept {
  return _name;
}

DataTypeInformation& DataTypeInformation::size(size_t size) {
  assert(_size > 0);
  _size = size;

  return *this;
}

DataTypeInformation::size_t DataTypeInformation::size() const noexcept {
  return _size;
}

bool DataTypeInformation::isValid() const noexcept {
  return !_name.empty() && _size > 0;
}
