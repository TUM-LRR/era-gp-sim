/*
* ERASIM Assembler Interpreter
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

#include "parser/independent/memory-reservation-directive.hpp"

#include <cstddef>
#include <cstdint>

#include "arch/common/architecture.hpp"
#include "core/memory-access.hpp"
#include "parser/independent/allocate-memory-immutable-arguments.hpp"
#include "parser/independent/enhance-symbol-table-immutable-arguments.hpp"
#include "parser/independent/execute-immutable-arguments.hpp"
#include "parser/independent/expression-compiler-clike.hpp"
#include "parser/independent/memory-allocator.hpp"
#include "parser/independent/section-tracker.hpp"
#include "parser/independent/symbol-graph.hpp"
#include "parser/independent/symbol-replacer.hpp"

MemoryReservationDirective::MemoryReservationDirective(
    const CodePositionInterval& positionInterval,
    const PositionedStringVector& labels,
    const PositionedString& name,
    const PositionedStringVector& values,
    const ArgumentCompileFunction& argumentCompile,
    std::size_t cellSize)
: IntermediateDirective(positionInterval, labels, name)
, _cellSize(cellSize)
, _values(values)
, _argumentCompile(argumentCompile) {
}

void MemoryReservationDirective::allocateMemory(
    const AllocateMemoryImmutableArguments& immutable,
    CompileErrorList& errors,
    MemoryAllocator& allocator,
    SectionTracker& tracker) {
  if (tracker.section() == "text") {
    errors.pushWarning(
        name().positionInterval(),
        "Careful, you are trying to reserve memory in the text "
        "section where the program instructions are stored. This "
        "might cause unexpected behavior. Use a '.section "
        "data' directive in front of this to resolve the "
        "issue.");
  }

  if (_values.empty()) {
    errors.pushWarning(name().positionInterval(),
                       "Implicit reservation of 0 bytes, missing arguments?");
  }
  // So, we simply calculate and sum up our arguments.
  std::size_t sizeInCells = 0;
  for (const auto& value : _values) {
    // b/c of the definition of argumentCompile and the C standard, the result
    // is non-negative.
    auto result =
        _argumentCompile(value, immutable.preliminaryReplacer(), errors);
    if (result > 0) {
      sizeInCells += result;
    } else {
      errors.pushWarning(name().positionInterval(), "Reserving 0 bytes");
    }
  }

  // Now, we got the number of cells to reserve. Let's calculate the number of
  // bytes out of it.
  auto sizeInBytes = sizeInCells * _cellSize;

  // Next, we got to allocate our memory.
  _relativePosition =
      allocator[tracker.section()].allocateRelative(sizeInBytes);

  // The bit size is store for further usage.
  _size = sizeInBytes * immutable.architecture().getByteSize();
}

void MemoryReservationDirective::enhanceSymbolTable(
    const EnhanceSymbolTableImmutableArguments& immutable,
    CompileErrorList& errors,
    SymbolGraph& graph) {
  // We calculate the absolute memory position and enhance our symbol table.
  _absolutePosition = immutable.allocator().absolutePosition(_relativePosition);
  auto absolutePositioned = PositionedString(std::to_string(_absolutePosition));
  for (const auto& label : labels()) {
    graph.addNode(Symbol(label, absolutePositioned));
  }
}

void MemoryReservationDirective::execute(
    const ExecuteImmutableArguments& immutable,
    CompileErrorList& errors,
    FinalCommandVector& commandOutput,
    MemoryAccess& memoryAccess) {
  // Finally, we may put some zeros into memory.
  if (_size > 0) {
    memoryAccess.putMemoryValueAt(_absolutePosition, MemoryValue(_size));
  }
}

MemoryAddress MemoryReservationDirective::absolutePosition() const noexcept {
  return _absolutePosition;
}
const PositionedStringVector& MemoryReservationDirective::values() const
    noexcept {
  return _values;
}
