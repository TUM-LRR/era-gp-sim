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

#include <cstdint>
#include "parser/memory-reservation-directive.hpp"
#include "arch/common/architecture.hpp"
#include "core/memory-access.hpp"
#include "parser/expression-compiler-clike.hpp"

void MemoryReservationDirective::allocateMemory(
    const Architecture& architecture, MemoryAllocator& allocator,
    CompileState& state) {
  if (_values.size() <= 0) {
    state.addWarning("Implicit reservation of 0 bytes", CodePosition(_lines.lineStart, _lines.lineEnd));
  }
  // So, we simply calculate and sum up our arguments.
  std::size_t sizeInCells = 0;
  for (const auto& i : _values) {
    intmax_t result = _argumentCompile(i, state);
    if(result > 0) {
        sizeInCells += result;
    }else if(result < 0) {
        state.addError("Cannot reserve a negative amount of bytes", CodePosition(_lines.lineStart, _lines.lineEnd));
    }else{
        state.addWarning("Reserving 0 bytes", CodePosition(_lines.lineStart, _lines.lineEnd));
    }
  }

  // Now, we got the number of cells to reserve. Let's calculate the number of
  // bytes out of it.
  auto sizeInBytes = sizeInCells * _cellSize;

  // Next, we got to allocate our memory.
  _relativePosition = allocator[state.section].allocateRelative(sizeInBytes);

  // The bit size is store for further usage.
  _size = sizeInBytes * architecture.getByteSize();
}

void MemoryReservationDirective::enhanceSymbolTable(
    SymbolTable& table, const MemoryAllocator& allocator, CompileState& state) {
  // We calculate the absolute memory position and enhance our symbol table.
  _absolutePosition = allocator.absolutePosition(_relativePosition);
  for (const auto& i : _labels) {
    table.insertEntry(i, std::to_string(_absolutePosition), state);
  }
}

void MemoryReservationDirective::execute(
    FinalRepresentation& finalRepresentator, const SymbolTable& table,
    const SyntaxTreeGenerator& generator, CompileState& state,
    MemoryAccess& memoryAccess) {
  // Finally, we may put some zeros into memory.
  if(_size > 0) {
      memoryAccess.putMemoryValueAt(_absolutePosition, MemoryValue(_size));
  }
}
