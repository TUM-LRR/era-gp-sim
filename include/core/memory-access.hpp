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

#ifndef ERAGPSIM_CORE_MEMORY_ACCESS_HPP
#define ERAGPSIM_CORE_MEMORY_ACCESS_HPP

#include <string>
#include <unordered_map>

#include "core/memory-value.hpp"
#include "core/register.hpp"

struct MemoryAccess {
  void addRegister(const std::string& name, const Register& newRegister) {
    _registers[name] = newRegister;
  }

  void setRegisterValue(const std::string& name, const MemoryValue& value) {
    _registers[name].set(value);
  }

  MemoryValue getRegisterValue(const std::string& name) const {
    return _registers.at(name).get();
  }

  std::unordered_map<std::string, Register> _registers;
};

#endif /* ERAGPSIM_CORE_MEMORY_ACCESS_HPP */
