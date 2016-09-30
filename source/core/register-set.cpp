/* C++ Assembler Interpreter
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

#include "core/register-set.hpp"

RegisterSet::RegisterSet() : _dict{}, _register{}, _updateSet{} {
}

RegisterSet::RegisterSet(const std::size_t bucketCount)
: _dict{bucketCount}, _register{}, _updateSet{} {
}

MemoryValue RegisterSet::get(const std::string &name) const {
  auto registerIterator = _dict.find(name);
  assert::that(registerIterator != _dict.end());
  return _register[registerIterator->second.adress].subSet(
      registerIterator->second.begin, registerIterator->second.end);
}

void RegisterSet::get(const std::string &name, MemoryValue &out) const {
  // I don't see much need for this anymore, probably should be removed entirely
  auto registerIterator = _dict.find(name);
  assert::that(registerIterator != _dict.end());
  out = _register[registerIterator->second.adress].subSet(
      registerIterator->second.begin, registerIterator->second.end);
}

void RegisterSet::put(const std::string &name, const MemoryValue &value) {
  auto registerIterator = _dict.find(name);
  assert::that(registerIterator != _dict.end());
  assert::that(value.getSize() ==
               (registerIterator->second.end - registerIterator->second.begin));
  _register[registerIterator->second.adress].write(
      value, registerIterator->second.begin);
  wasUpdated(registerIterator->second.adress);
}

MemoryValue
RegisterSet::set(const std::string &name, const MemoryValue &value) {
  auto registerIterator = _dict.find(name);
  assert::that(registerIterator != _dict.end());
  assert::that(value.getSize() ==
               (registerIterator->second.end - registerIterator->second.begin));
  MemoryValue previous{_register[registerIterator->second.adress].subSet(
      registerIterator->second.begin, registerIterator->second.end)};
  _register[registerIterator->second.adress].write(
      value, registerIterator->second.begin);
  wasUpdated(registerIterator->second.adress);
  return previous;
}


MemoryValue &
RegisterSet::exchange(const std::string &name, MemoryValue &value) {
  // I don't see much need for this, probably should be removed entirely
  return value = set(name, value);
}

void RegisterSet::createRegister(const std::string &name,
                                 const std::size_t size) {
  // could optimize this using moves, maybe
  createRegister(name, MemoryValue{size});
}

void RegisterSet::createRegister(const std::string &name,
                                 const MemoryValue &value) {
  assert::that(_dict.find(name) == _dict.end());
  _dict.emplace(name, RegisterID(_register.size(), 0, value.getSize()));
  _register.emplace_back(MemoryValue(value));
  _updateSet.push_back(std::set<std::string>{name});
  wasUpdated(_register.size() - 1);
}

void RegisterSet::aliasRegister(const std::string &name,
                                const std::string &parent,
                                const std::size_t begin,
                                std::size_t end) {
  assert::that(_dict.find(name) == _dict.end());
  auto parentIterator = _dict.find(parent);
  assert::that(parentIterator != _dict.end());
  assert::that(begin >= 0);
  assert::that(end <= _register[parentIterator->second.adress].getSize());
  assert::that(end - begin > 0);
  _dict.emplace(name,
                RegisterID(parentIterator->second.adress,
                           begin + parentIterator->second.begin,
                           end + parentIterator->second.begin));
  _updateSet[parentIterator->second.adress].emplace(name);
}

void RegisterSet::aliasRegister(const std::string &name,
                                const std::string &parent,
                                const std::size_t begin) {
  assert::that(_dict.find(name) == _dict.end());
  auto parentIterator = _dict.find(parent);
  assert::that(parentIterator != _dict.end());
  assert::that(begin >= 0);
  assert::that(_register[parentIterator->second.adress].getSize() - begin > 0);
  _dict.emplace(
      name,
      RegisterID(parentIterator->second.adress,
                 begin + parentIterator->second.begin,
                 parentIterator->second.end + parentIterator->second.begin));
  _updateSet[parentIterator->second.adress].emplace(name);
}
