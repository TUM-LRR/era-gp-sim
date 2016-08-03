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
#include <vector>

#include "arch/common/register-information.hpp"
#include "common/utility.hpp"

typename RegisterInformation::id_t RegisterInformation::_rollingID = 0;

bool RegisterInformation::isSpecialType(Type type) noexcept {
  return type != Type::INTEGER && type != Type::FLOAT && type != Type::VECTOR;
}

RegisterInformation::RegisterInformation()
: _id(_rollingID++), _type(Type::INTEGER) {
}

RegisterInformation::RegisterInformation(InformationInterface::Format& data)
: RegisterInformation() {
  _deserialize(data);
}

// clang-format off
RegisterInformation::RegisterInformation(const std::string& name)
: RegisterInformation() {
  this->name(name);
}
// clang-format on

bool RegisterInformation::operator==(const RegisterInformation& other) const
    noexcept {
  if (this->_id != other._id) return false;
  if (this->_type != other._type) return false;
  if (this->_name != other._name) return false;
  if (this->_size != other._size) return false;
  if (this->_constant != other._constant) return false;
  if (this->_enclosing != other._enclosing) return false;
  if (this->_constituents != other._constituents) return false;
  if (this->_aliases != other._aliases) return false;

  return true;
}

bool RegisterInformation::operator!=(const RegisterInformation& other) const
    noexcept {
  return !(*this == other);
}

RegisterInformation&
RegisterInformation::deserialize(InformationInterface::Format& data) {
  _deserialize(data);
  return *this;
}

RegisterInformation& RegisterInformation::name(const std::string& name) {
  _name = name;

  return *this;
}

const std::string& RegisterInformation::getName() const noexcept {
  assert(hasName());
  return _name;
}

bool RegisterInformation::hasName() const noexcept {
  return !_name.empty();
}

RegisterInformation& RegisterInformation::size(size_t bit_size) {
  assert(bit_size > 0);
  _size = bit_size;

  return *this;
}

RegisterInformation::size_t RegisterInformation::getSize() const noexcept {
  assert(hasSize());
  return *_size;
}

bool RegisterInformation::hasSize() const noexcept {
  return static_cast<bool>(_size);
}

RegisterInformation& RegisterInformation::id(id_t id) {
  _id = id;
  return *this;
}

RegisterInformation::id_t RegisterInformation::getID() const noexcept {
  return _id;
}

RegisterInformation& RegisterInformation::type(Type type) {
  _type = type;
  return *this;
}

RegisterInformation::Type RegisterInformation::getType() const noexcept {
  return _type;
}

bool RegisterInformation::isSpecial() const noexcept {
  return RegisterInformation::isSpecialType(_type);
}

bool RegisterInformation::isConstant() const noexcept {
  return static_cast<bool>(_constant);
}

RegisterInformation& RegisterInformation::addAlias(const std::string& alias) {
  _aliases.emplace_back(alias);
  return *this;
}

RegisterInformation& RegisterInformation::addAliases(AliasList aliases) {
  _aliases.insert(_aliases.end(), aliases);
  return *this;
}

const std::vector<std::string>& RegisterInformation::getAliases() const
    noexcept {
  return _aliases;
}

bool RegisterInformation::hasAliases() const noexcept {
  return !_aliases.empty();
}

RegisterInformation& RegisterInformation::enclosing(id_t id) {
  assert(id != _id);
  _enclosing = id;

  return *this;
}

RegisterInformation::id_t RegisterInformation::getEnclosing() const noexcept {
  assert(hasEnclosing());
  return *_enclosing;
}

bool RegisterInformation::hasEnclosing() const noexcept {
  return static_cast<bool>(_enclosing);
}

RegisterInformation& RegisterInformation::addConstituents(IDList constituents) {
  if (_enclosing) {
    assert(Utility::noneOf(constituents,
                           [this](auto& id) { return id == *_enclosing; }));
  }
  _constituents.insert(_constituents.end(), constituents);
  return *this;
}

RegisterInformation& RegisterInformation::addConstituent(id_t id) {
  _constituents.emplace_back(id);
  return *this;
}

const std::vector<RegisterInformation::id_t>&
RegisterInformation::getConstituents() const noexcept {
  return _constituents;
}

bool RegisterInformation::hasConstituents() const noexcept {
  return !_constituents.empty();
}

bool RegisterInformation::isValid() const noexcept {
  // Cannot check the ID, because we don't know what it means for an ID to be
  // valid. One thing to check would be for duplicate IDs of course, but for
  // that we would need to keep some symbol table.
  // The type has a default value, so no need to validate.
  return !_name.empty() && _size;
}

void RegisterInformation::_deserialize(InformationInterface::Format& data) {
  assert(data.count("name"));
  assert(data.count("size"));

  Utility::doIfThere(data, "id", [this](auto& id) { this->id(id); });

  _parseType(data);
  this->name(Utility::toLower(data["name"]));
  this->size(data["size"]);

  Utility::doIfThere(data, "constant", [this](auto& constant) {
    this->constant(static_cast<double>(constant));
  });

  Utility::doIfThere(data, "enclosing", [this](auto& enclosing) {
    assert(enclosing.is_number());
    this->enclosing(enclosing);
  });

  Utility::doIfThere(data, "constituents", [this](auto& constituents) {
    this->addConstituents(constituents);
  });

  Utility::doIfThere(data, "constituent", [this](auto& constituent) {
    this->addConstituent(constituent);
  });

  // clang-format off
  Utility::doIfThere(data, "aliases", [this](auto& aliases) {
      for (const auto& alias : aliases) {
        this->addAlias(alias);
      }
  });

  Utility::doIfThere(data, "alias", [this](auto& alias) {
      this->addAlias(alias);
  });
  // clang-format on
}

void RegisterInformation::_parseType(InformationInterface::Format& data) {
  auto type = data.find("type");

  if (type == data.end() || *type == "integer") {
    _type = Type::INTEGER;
  } else if (*type == "float") {
    _type = Type::FLOAT;
  } else if (*type == "vector") {
    _type = Type::VECTOR;
  } else if (*type == "flag") {
    _type = Type::FLAG;
  } else if (*type == "link") {
    _type = Type::LINK;
  } else if (*type == "program-counter") {
    _type = Type::PROGRAM_COUNTER;
  } else {
    assert(false);
  }
}
