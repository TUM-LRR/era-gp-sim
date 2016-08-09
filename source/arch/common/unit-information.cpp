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

#include "arch/common/unit-information.hpp"

UnitInformation::UnitInformation(const std::string& name) {
  // To check constraints
  this->name(name);
}

UnitInformation& UnitInformation::name(const std::string& name) noexcept {
  assert(!name.empty());
  _name = name;

  return *this;
}

const std::string& UnitInformation::getName() const noexcept {
  return _name;
}

UnitInformation& UnitInformation::addRegisters(List regs) {
  assert(regs.size() > 0);
  return addRegisters<List>(regs);
}

UnitInformation& UnitInformation::addRegister(const RegisterInformation& reg) {
  _container.emplace_back(reg);

  return *this;
}

bool UnitInformation::isValid() const noexcept {
  if (_name.empty()) return false;
  // clang-format off
  return Utility::allOf(_container, [](auto& reg) {
    return reg.isValid();
  });
  // clang-format on
}
