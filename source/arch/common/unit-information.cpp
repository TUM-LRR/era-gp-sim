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

UnitInformation::UnitInformation(const InformationInterface::Format& data) {
  _deserialize(data);
}

UnitInformation::UnitInformation(const std::string& name) {
  // To check constraints
  this->name(name);
}

UnitInformation& UnitInformation::deserialize(
    const InformationInterface::Format& data) {
  _deserialize(data);
  return *this;
}

UnitInformation& UnitInformation::name(const std::string& name) noexcept {
  assert(!name.empty());
  _name = name;

  return *this;
}

const std::string& UnitInformation::getName() const noexcept {
  return _name;
}

const RegisterInformation& UnitInformation::getSpecialRegister(Type type) const
    noexcept {
  assert(hasSpecialRegister(type));
  return _specialRegisters.at(type);
}

bool UnitInformation::hasSpecialRegister(Type type) const noexcept {
  return _specialRegisters.count(type);
}

UnitInformation& UnitInformation::addRegisters(InitializerList regs) {
  assert(regs.size() > 0);
  return addRegisters<InitializerList>(regs);
}

UnitInformation& UnitInformation::addRegister(
    const RegisterInformation& registerInformation) {
  if (registerInformation.isSpecial()) {
    // clang-format off
    _specialRegisters.emplace(
        registerInformation.getType(),
        registerInformation
    );
    // clang-format on
  } else {
    _container.emplace_back(registerInformation);
  }

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

void UnitInformation::_deserialize(const InformationInterface::Format& data) {
  assert(data.count("name"));
  assert(data.count("registers"));
  assert(!data["registers"].empty());

  name(data["name"]);

  for (auto& registerInformation : data["registers"]) {
    addRegister(RegisterInformation{registerInformation});
  }
}
