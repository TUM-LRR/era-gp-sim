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

#ifndef ERAGPSIM_CORE_MEMORY_HPP_
#define ERAGPSIM_CORE_MEMORY_HPP_

#include <cassert>
#include <cstddef>

#include "core/memory-value.hpp"

class Memory {
 public:
  Memory();
  Memory(std::size_t cellCount, std::size_t byteSize = 8);
  Memory(const Memory &other) = default;
  Memory &operator=(const Memory &other) = default;
  Memory(Memory &&other)                 = default;
  Memory &operator=(Memory &&other) = default;
  ~Memory()                                 = default;

  MemoryValue get(const std::size_t address, const std::size_t amount = 1);
  void put(const std::size_t address, const MemoryValue &value);
  MemoryValue set(const std::size_t address, const MemoryValue &value);

 private:
  std::size_t _byteSize;
  std::size_t _cellCount;
  MemoryValue _data;

  void wasUpdated(const std::size_t address, const std::size_t amount = 1) {
  }
};

#endif// ERAGPSIM_CORE_MEMORY_HPP_
