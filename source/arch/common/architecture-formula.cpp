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

ArchitectureFormula::ArchitectureFormula(
    const std::string& architectureName = std::string(),
    InitializerList list                = InitializerList())
: super(list), _architectureName(architectureName) {
}


ArchitectureFormula& ArchitectureFormula::add(const std::string& name) {
  assert(!name.empty());
  _container.emplace_back(name);
  return *this;
}

const std::string& ArchitectureFormula::architectureName() const noexcept {
  return _architectureName;
}

ArchitectureFormula& ArchitectureFormula::getArchitectureName(
    const std::string& name) {
  assert(!name.empty());
  _architectureName = name;

  return *this;
}

bool ArchitectureFormula::isValid() const noexcept override {
  return !_archtitectureName.empty() && !isEmpty();
}
