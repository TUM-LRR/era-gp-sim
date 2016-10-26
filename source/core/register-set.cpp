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

const std::string RegisterSet::_registerStringIdentifier = "register_";

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
  if (!_constant[registerID.address]) {
    assert::that(value.getSize() == (registerID.end - registerID.begin));
    _register[registerID.address].write(value, registerID.begin);
    _wasUpdated(registerID.address);
  }
}

MemoryValue
RegisterSet::set(const std::string &name, const MemoryValue &value) {
  auto registerIterator = _dict.find(name);
  assert::that(registerIterator != _dict.end());
  auto registerID = registerIterator->second;
  MemoryValue previous{
      _register[registerID.address].subSet(registerID.begin, registerID.end)};
  if (!_constant[registerID.address]) {
    assert::that(value.getSize() == (registerID.end - registerID.begin));
    _register[registerID.address].write(value, registerID.begin);
    _wasUpdated(registerID.address);
  }
  return previous;
}

std::size_t RegisterSet::getSize(const std::string &name) const {
  assert::that(existsRegister(name));
  auto registerIterator = _dict.find(name);
  auto registerID = registerIterator->second;
  return registerID.end - registerID.begin;
}

void RegisterSet::createRegister(const std::string &name,
                                 std::size_t size,
                                 bool constant) {
  // could optimize this using moves, maybe
  createRegister(name, MemoryValue{size});
}

void RegisterSet::createRegister(const std::string &name,
                                 const MemoryValue &value,
                                 bool constant) {
  assert::that(_dict.find(name) == _dict.end());
  _dict.emplace(name, RegisterID(_register.size(), 0, value.getSize()));
  _register.emplace_back(MemoryValue(value));
  _updateSet.push_back(std::set<std::string>{name});
  _parentVector.push_back(name);
  _constant.push_back(constant);
  _wasUpdated(_register.size() - 1);
}
void RegisterSet::createRegister(const std::vector<std::string> &nameList,
                                 const MemoryValue &value,
                                 bool constant,
                                 bool silent) {
  assert::that(nameList.size() > 0);
  // this could maybe be optimized with move stuff
  createRegister(nameList[0], value, constant);
  for (std::size_t i = 1; i < nameList.size(); ++i) {
    aliasRegister(nameList[i], nameList[0], 0, silent);
  }
}
void RegisterSet::createRegister(const std::vector<std::string> &nameList,
                                 std::size_t size,
                                 bool constant,
                                 bool silent) {
  createRegister(nameList, MemoryValue{size}, constant, silent);
}

void RegisterSet::aliasRegister(const std::string &name,
                                const std::string &parent,
                                const std::size_t begin,
                                std::size_t end,
                                const bool silent) {
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
  if (!silent) {
    _updateSet[parentID.address].emplace(name);
  }
}

void RegisterSet::aliasRegister(const std::string &name,
                                const std::string &parent,
                                const std::size_t begin,
                                const bool silent) {
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
  if (!silent) {
    _updateSet[parentID.address].emplace(name);
  }
}

void RegisterSet::aliasRegister(const std::vector<std::string> &nameList,
                                const std::string &parent,
                                const std::size_t begin,
                                const std::size_t end,
                                const bool silent) {
  for (auto name : nameList) {
    aliasRegister(name, parent, begin, end, silent);
  }
}

void RegisterSet::aliasRegister(const std::vector<std::string> &nameList,
                                const std::string &parent,
                                const std::size_t begin,
                                const bool silent) {
  for (auto name : nameList) {
    aliasRegister(name, parent, begin, silent);
  }
}

// there is currently still a slight edge case where this is not fully reflexive
bool RegisterSet::operator==(const RegisterSet &other) const {
  if (_parentVector.size() != other._parentVector.size()) {
    return false;
  }
  for (const auto &name : _parentVector) {
    if (!other.existsRegister(name)) {
      return false;
    } else {
      if (getSize(name) != other.getSize(name)) {
        return false;
      } else {
        if (get(name) != other.get(name)) {
          return false;
        }
      }
    }
  }
  return true;
}

bool RegisterSet::existsRegister(const std::string &name) const {
  auto iterator = _dict.find(name);
  return iterator != _dict.end();
}

nlohmann::json &RegisterSet::serializeJSON(nlohmann::json &json) const {
  auto data = _serializeRaw();
  for (const auto &i : data) {
    json[i.first] = i.second.toHexString(false, false);
  }
  return json;
}
nlohmann::json RegisterSet::serializeJSON(nlohmann::json &&json) const {
  return serializeJSON(json);
}
std::ostream &operator<<(std::ostream &stream, const RegisterSet &value) {
  // TODO::Do all the things
  return stream;
}
std::map<std::string, MemoryValue> RegisterSet::_serializeRaw() const {
  std::map<std::string, MemoryValue> map{};
  for (std::size_t i = 0; i < _register.size(); ++i) {
    map.emplace(_registerStringIdentifier + _parentVector[i], _register[i]);
  }
  return map;
}


void RegisterSet::_wasUpdated(const std::size_t address) {
  for (auto name : _updateSet[address]) {
    _callback(name);
  }
}
