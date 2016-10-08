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

#ifndef ERAGPSIM_CORE_REGISTERSET_HPP_
#define ERAGPSIM_CORE_REGISTERSET_HPP_

#include <cassert>
#include <cstddef>
#include <unordered_map>
#include <vector>

#include "core/memory-value.hpp"
#include "core/register-id.hpp"

class RegisterSet {
 public:
  RegisterSet(const std::size_t defaultByteSize = 8);
  RegisterSet(const std::size_t defaultByteSize, const std::size_t bucketCount);
  RegisterSet(RegisterSet &&) = default;
  RegisterSet &operator=(RegisterSet &&) = default;
  RegisterSet(const RegisterSet &)       = default;
  RegisterSet &operator=(const RegisterSet &) = default;
  ~RegisterSet()                              = default;

  MemoryValue get(const std::string &name) const;
  MemoryValue get(const std::string &name, const std::size_t byteSize) const;
  //Don'tuse yet, potential threading issues
  void get(const std::string &name, MemoryValue &out) const;
  MemoryValue get(const std::string &name, MemoryValue &&out) const;
  void put(const std::string &name, const MemoryValue &value);
  MemoryValue set(const std::string &name, const MemoryValue &value);
  MemoryValue set(const std::string &name, MemoryValue &&value);
  //Don'tuse yet, potential threading issues
  void exchange(const std::string &name, MemoryValue &value);

  void createRegister(const std::string &name, const std::size_t width);
  void createRegister(const std::string &name,
                      const std::size_t width,
                      const std::size_t byteSize);
  void createRegister(const std::string &name, const MemoryValue &value);
  void createRegister(const std::string &name, const MemoryValue &&value);
  void aliasRegister(const std::string &name,
                     const std::string &parent,
                     const std::size_t begin,
                     std::size_t end);
  void aliasRegister(const std::string &name,
                     const std::string &parent,
                     const std::size_t begin = 0);

 private:
  std::unordered_map<std::string, RegisterID> _dict;
  std::vector<MemoryValue> _register;
  std::size_t _defaultByteSize;
};

#endif// ERAGPSIM_CORE_REGISTERSET_HPP_
