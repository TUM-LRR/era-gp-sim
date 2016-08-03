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

UnitInformation::UnitInformation(InformationInterface::Format& data) {
  _deserialize(data);
}

UnitInformation::UnitInformation(const std::string& name) : _name(name) {
}

UnitInformation::UnitInformation(const std::string& name, InitializerList list)
: _name(name) {
  addRegisters(list);
}

bool UnitInformation::operator==(const UnitInformation& other) const noexcept {
  if (this->_name != other._name) return false;
  if (this->_container != other._container) return false;
  if (this->_specialRegisters != other._specialRegisters) return false;

  return true;
}

bool UnitInformation::operator!=(const UnitInformation& other) const noexcept {
  return !(*this == other);
}

UnitInformation&
UnitInformation::deserialize(InformationInterface::Format& data) {
  _deserialize(data);
  return *this;
}

UnitInformation& UnitInformation::name(const std::string& name) noexcept {
  assert(!name.empty());
  _name = name;

  return *this;
}

const std::string& UnitInformation::getName() const noexcept {
  assert(hasName());
  return _name;
}

bool UnitInformation::hasName() const noexcept {
  return !_name.empty();
}

const RegisterInformation& UnitInformation::getSpecialRegister(Type type) const
    noexcept {
  assert(hasSpecialRegister(type));
  return _specialRegisters.at(type);
}

bool UnitInformation::hasSpecialRegister(Type type) const noexcept {
  return _specialRegisters.count(type);
}

bool UnitInformation::hasSpecialRegisters() const noexcept {
  return !_specialRegisters.empty();
}

UnitInformation& UnitInformation::addRegisters(InitializerList registers) {
  assert(registers.size() > 0);
  return addRegisters<InitializerList>(registers);
}

UnitInformation&
UnitInformation::addRegister(const RegisterInformation& registerInformation) {
  if (registerInformation.isSpecial()) {
    // clang-format off
    _specialRegisters.emplace(
        registerInformation.getType(),
        registerInformation
    );
    // clang-format on
  } else {
    _container.emplace(registerInformation);
  }

  return *this;
}

bool UnitInformation::isValid() const noexcept {
  if (_name.empty()) return false;
  if (isEmpty() && !hasSpecialRegisters()) return false;

  // clang-format off
  auto registersOK = Utility::allOf(_container, [](auto& registerInformation) {
    return registerInformation.isValid();
  });
  // clang-format on

  // clang-format off
  auto specialOK = Utility::allOf(_specialRegisters, [](auto& special) {
    return special.second.isValid();
  });
  // clang-format on

  return registersOK && specialOK;
}

void UnitInformation::_deserialize(InformationInterface::Format& data) {
  assert(data.count("name"));
  assert(data.count("registers"));
  assert(!data["registers"].empty());

  name(data["name"]);

  for (auto& registerInformation : data["registers"]) {
    addRegister(static_cast<RegisterInformation>(registerInformation));
  }

  assert(isValid());
}
