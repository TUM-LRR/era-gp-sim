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

#include "arch/common/instruction-key.hpp"


InstructionKey::InstructionKey(InformationInterface::Format& data) {
  _deserialize(data);
}


InstructionKey::InstructionKey(InitializerList list) : super(list) {
}

bool InstructionKey::operator==(const InstructionKey& other) const noexcept {
  return _container == other._container;
}

bool InstructionKey::operator!=(const InstructionKey& other) const noexcept {
  return !(*this == other);
}

InstructionKey&
InstructionKey::deserialize(InformationInterface::Format& data) {
  _deserialize(data);
  return *this;
}

InstructionKey& InstructionKey::addEntry(const Key& key, const Value& value) {
  assert(!hasKey(key));
  _container.emplace(key, value);

  return *this;
}

InstructionKey& InstructionKey::addEntries(InitializerList list) {
  assert(list.size() > 0);
  addEntries<InitializerList>(list);

  return *this;
}

const InstructionKey::Value& InstructionKey::opcode() const {
  return get("opcode");
}

const InstructionKey::Value& InstructionKey::get(const Key& key) const
noexcept {
  assert(hasKey(key));
  return _container.at(key);
}

const InstructionKey::Value& InstructionKey::operator[](const Key& key) const {
  return get(key);
}

bool InstructionKey::hasKey(const Key& key) const noexcept {
  return _container.count(key);
}

InstructionKey::KeyCollection InstructionKey::getKeys() const noexcept {
  KeyCollection collection;
  collection.reserve(_container.size());
  for (auto& pair : _container) {
    collection.emplace_back(pair.first);
  }

  return collection;
}

InstructionKey::ValueCollection InstructionKey::getValues() const noexcept {
  ValueCollection collection;
  collection.reserve(_container.size());
  for (auto& pair : _container) {
    collection.emplace_back(pair.second);
  }

  return collection;
}

bool InstructionKey::isValid() const noexcept {
  return hasKey("opcode");
}

void InstructionKey::_deserialize(InformationInterface::Format& data) {
  assert(!data.empty());
  for (auto pair = data.begin(); pair != data.end(); ++pair) {
    addEntry(pair.key(), pair.value());
  }
}
