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

#include "parser/memory-reservation-directive.hpp"
#include <cstdint>
#include "arch/common/architecture.hpp"
#include "core/memory-access.hpp"
#include "parser/expression-compiler-clike.hpp"
#include "parser/memory-allocator.hpp"
#include "parser/symbol-table.hpp"

MemoryReservationDirective::MemoryReservationDirective(
    const LineInterval& lines,
    const std::vector<std::string>& labels,
    const std::string& name,
    std::size_t cellSize,
    const std::vector<std::string>& values,
    const ArgumentCompileFunction& argumentCompile)
: IntermediateDirective(lines, labels, name)
, _cellSize(cellSize)
, _values(values)
, _argumentCompile(argumentCompile) {
}

void MemoryReservationDirective::allocateMemory(
    const Architecture& architecture,
    MemoryAllocator& allocator,
    CompileState& state) {
  if (_values.empty()) {
    state.addWarning("Implicit reservation of 0 bytes, missing arguments?",
                     CodePosition(_lines.lineStart, _lines.lineEnd));
  }
  // So, we simply calculate and sum up our arguments.
  std::size_t sizeInCells = 0;
  for (const auto& i : _values) {
    // b/c of the definition of argumentCompile and the C standard, the result
    // is non-negative.
    auto result = _argumentCompile(i, state);
    if (result > 0) {
      sizeInCells += result;
    } else {
      state.addWarning("Reserving 0 bytes",
                       CodePosition(_lines.lineStart, _lines.lineEnd));
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
    table.insertEntry(
        i,
        std::to_string(_absolutePosition),
        /*TODO*/ CodePositionInterval(CodePosition(0), CodePosition(0)),
        state);
  }
}

void MemoryReservationDirective::execute(
    FinalRepresentation& finalRepresentator,
    const SymbolTable& table,
    const SyntaxTreeGenerator& generator,
    CompileState& state,
    MemoryAccess& memoryAccess) {
  // Finally, we may put some zeros into memory.
  if (_size > 0) {
    memoryAccess.putMemoryValueAt(_absolutePosition, MemoryValue(_size));
  }
}
