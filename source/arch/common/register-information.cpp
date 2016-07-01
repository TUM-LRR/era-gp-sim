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

RegisterInformation::RegisterInformation() = default;

RegisterInformation::RegisterInformation(const Information::Format& data) {
  _deserialize(data);
}

// clang-format off
RegisterInformation::RegisterInformation(const std::string& name)
: _id(_rollingID++)
, _type(Type::INTEGER)
, _size(32) {
  this->name(name);
}
// clang-format on

RegisterInformation&
RegisterInformation::deserialize(const Information::Format& data) {
  _deserialize(data);
  return *this;
}

RegisterInformation& RegisterInformation::name(const std::string& name) {
  _name = name;

  return *this;
}

const std::string& RegisterInformation::getName() const noexcept {
  assert(!_name.empty());
  return _name;
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
  // clang-format off
  return _type != Type::INTEGER &&
         _type != Type::FLOAT &&
         _type != Type::VECTOR;
  // clang-format on
}

bool RegisterInformation::isConstant() const noexcept {
  return static_cast<bool>(_constant);
}

RegisterInformation& RegisterInformation::addAlias(const std::string& alias) {
  _aliases.emplace_back(alias);

  return *this;
}

RegisterInformation&
RegisterInformation::addAliases(std::initializer_list<std::string> aliases) {
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
  return *_enclosing;
}

bool RegisterInformation::hasEnclosing() const noexcept {
  return static_cast<bool>(_enclosing);
}

RegisterInformation&
RegisterInformation::addConstituents(std::initializer_list<id_t> constituents) {
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

void RegisterInformation::_deserialize(const Information::Format& data) {
  assert(data.count("id"));
  assert(data.count("name"));
  assert(data.count("size"));

  id(data["id"]);
  _parseType(data);
  name(data["name"]);
  size(data["size"]);

  auto constant = data.find("constant");
  if (constant != data.end()) {
    _constant = static_cast<double>(constant.value());
  }

  auto enclosing = data.find("enclosing");
  if (enclosing != data.end()) {
    _enclosing = static_cast<id_t>(enclosing.value());
  }

  auto constituents = data.find("constituents");
  if (constituents != data.end()) {
    addConstituents(data["constituents"]);
  }

  auto aliases = data.find("constituents");
  if (aliases != data.end()) {
    for (auto& alias : data["aliases"]) {
      addAlias(alias);
    }
  }
}

void RegisterInformation::_parseType(const Information::Format& data) {
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
