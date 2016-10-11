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

const std::function<void(const std::string &)> RegisterSet::emptyCallback =
    [](const std::string &) {};

RegisterSet::RegisterSet() : _dict{}, _register{}, _updateSet{} {
}

RegisterSet::RegisterSet(const std::size_t bucketCount)
: _dict{bucketCount}, _register{}, _updateSet{} {
}

void RegisterSet::setCallback(
    const std::function<void(const std::string &)> &callback) {
  _callback = callback;
}

MemoryValue RegisterSet::get(const std::string &name) const {
  auto registerIterator = _dict.find(name);
  assert::that(registerIterator != _dict.end());
  auto registerID = registerIterator->second;
  return _register[registerID.address].subSet(registerID.begin, registerID.end);
}

void RegisterSet::put(const std::string &name, const MemoryValue &value) {
  auto registerIterator = _dict.find(name);
  assert::that(registerIterator != _dict.end());
  auto registerID = registerIterator->second;
  assert::that(value.getSize() == (registerID.end - registerID.begin));
  _register[registerID.address].write(value, registerID.begin);
  wasUpdated(registerID.address);
}

MemoryValue
RegisterSet::set(const std::string &name, const MemoryValue &value) {
  auto registerIterator = _dict.find(name);
  assert::that(registerIterator != _dict.end());
  auto registerID = registerIterator->second;
  assert::that(value.getSize() == (registerID.end - registerID.begin));
  MemoryValue previous{
      _register[registerID.address].subSet(registerID.begin, registerID.end)};
  _register[registerID.address].write(value, registerID.begin);
  wasUpdated(registerID.address);
  return previous;
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
  auto parentID = parentIterator->second;
  assert::that(begin >= 0);
  assert::that(end <= _register[parentID.address].getSize());
  assert::that(end - begin > 0);
  _dict.emplace(name,
                RegisterID(parentID.address,
                           begin + parentID.begin,
                           end + parentID.begin));
  _updateSet[parentID.address].emplace(name);
}

void RegisterSet::aliasRegister(const std::string &name,
                                const std::string &parent,
                                const std::size_t begin) {
  assert::that(_dict.find(name) == _dict.end());
  auto parentIterator = _dict.find(parent);
  assert::that(parentIterator != _dict.end());
  auto parentID = parentIterator->second;
  assert::that(begin >= 0);
  assert::that(_register[parentID.address].getSize() - begin > 0);
  _dict.emplace(name,
                RegisterID(parentID.address,
                           begin + parentID.begin,
                           parentID.end + parentID.begin));
  _updateSet[parentID.address].emplace(name);
}

void RegisterSet::wasUpdated(const std::size_t address){
  for (auto name : _updateSet[address]) {
    _callback(name);
  }
}
