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

#include <string>

#include "arch/common/architecture-formula.hpp"

Architecture::Formula& Architecture::Formula::add(const std::string& name) {
  assert(!name.empty());
  _container.emplace_back(name);
  return *this;
}

std::string Architecture::Formula::operator[](index_t index) {
  assert(index < _container.size());
  return _container[index];
}


const std::string& Architecture::Formula::operator[](index_t index) const {
  assert(index < _container.size());
  return _container[index];
}

const std::string& Architecture::Formula::architectureName() const noexcept {
  return _architectureName;
}

Architecture::Formula&
Architecture::Formula::getArchitectureName(const std::string& name) {
  assert(!name.empty());
  _architectureName = name;

  return *this;
}
