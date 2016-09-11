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

#include "core/memory.hpp"

Memory::Memory() : Memory(64, 8) {
}

Memory::Memory(std::size_t cellCount, std::size_t byteSize)
: _cellCount{cellCount}, _byteSize{byteSize}, _data{cellCount * byteSize} {
  assert(cellCount > 0);
  assert(byteSize > 0);
}

MemoryValue
Memory::get(const std::size_t address, const std::size_t amount) {
  assert(address >= 0);
  assert(amount >= 0);
  assert(address + amount <= _cellCount);
  return _data.subSet(address * _byteSize, (address + amount) * _byteSize);
}

void Memory::put(const std::size_t address, const MemoryValue& value) {
  assert(address >= 0);
  assert(value.getSize() % _byteSize == 0);
  const std::size_t amount{value.getSize() / _byteSize};
  assert(amount >= 0);
  assert(address + amount <= _cellCount);
  _data.write(value, address * _byteSize);
}

MemoryValue
Memory::set(const std::size_t address, const MemoryValue& value) {
  const std::size_t amount{value.getSize() / _byteSize};
  MemoryValue prev{get(address, amount)};
  put(address, value);
  return prev;
}
