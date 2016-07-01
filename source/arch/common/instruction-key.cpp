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

InstructionKey::InstructionKey() noexcept = default;

InstructionKey::InstructionKey(const Information::Format& data) {
  _deserialize(data);
}


InstructionKey::InstructionKey(InitializerList list) : super(list) {
}

InstructionKey& InstructionKey::deserialize(const Information::Format& data) {
  _deserialize(data);
  return *this;
}

InstructionKey& InstructionKey::add(const Key& key, const Value& value) {
  assert(!hasKey(key));
  _container.insert({key, value});

  return *this;
}

InstructionKey& InstructionKey::add(InitializerList list) {
  assert(list.size() > 0);
  _container.insert(list);

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

void InstructionKey::_deserialize(const Information::Format& data) {
  for (auto pair = data.begin(); pair != data.end(); ++pair) {
    add(pair.key(), pair.value());
  }
}
