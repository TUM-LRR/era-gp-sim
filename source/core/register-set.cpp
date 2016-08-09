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

RegisterSet::RegisterSet(const std::size_t defaultByteSize)
: _dict{}, _defaultByteSize{defaultByteSize}, _register{} {
}

RegisterSet::RegisterSet(const std::size_t defaultByteSize,
                         const std::size_t bucketCount)
: _dict{bucketCount}, _defaultByteSize{defaultByteSize}, _register{} {
}

MemoryValue RegisterSet::get(const std::string &name) const {
  return MemoryValue{};
}

MemoryValue
RegisterSet::get(const std::string &name, const std::size_t byteSize) const {
  return MemoryValue{};
}

void RegisterSet::get(const std::string &name, MemoryValue &out) const {
}

MemoryValue RegisterSet::get(const std::string &name, MemoryValue &&out) const {
  return MemoryValue{};
}

void RegisterSet::put(const std::string &name, const MemoryValue &value) {
}

MemoryValue
RegisterSet::set(const std::string &name, const MemoryValue &value) {
  return MemoryValue{};
}

MemoryValue RegisterSet::set(const std::string &name, MemoryValue &&value) {
  return MemoryValue{};
}

void RegisterSet::exchange(const std::string &name, MemoryValue &value) {
}

void RegisterSet::createRegister(const std::string &name,
                                 const std::size_t width) {
  createRegister(name, width, _defaultByteSize);
}

void RegisterSet::createRegister(const std::string &name,
                                 const std::size_t width,
                                 const std::size_t byteSize) {
}

void RegisterSet::createRegister(const std::string &name,
                                 const MemoryValue &value) {
}

void RegisterSet::createRegister(const std::string &name,
                                 const MemoryValue &&value) {
}

void RegisterSet::aliasRegister(const std::string &name,
                                const std::string &parent,
                                const std::size_t begin,
                                std::size_t end) {
}

void RegisterSet::aliasRegister(const std::string &name,
                                const std::string &parent,
                                const std::size_t begin) {
}
