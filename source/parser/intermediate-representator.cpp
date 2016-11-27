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

#include "parser/intermediate-representator.hpp"

#include "arch/common/architecture.hpp"
#include "core/memory-access.hpp"
#include "parser/intermediate-macro-instruction.hpp"
#include "parser/symbol-table.hpp"

FinalRepresentation
IntermediateRepresentator::transform(const Architecture& architecture,
                                     const SyntaxTreeGenerator& generator,
                                     MemoryAllocator& allocator,
                                     CompileState& state,
                                     MemoryAccess& memoryAccess) {
  // Before everything begins, we got to check if we are still in a macro.
  if (_currentOutput) {
    state.addError("Macro not closed. Missing a macro end directive?");
  }

  FinalRepresentation representation;
  SymbolTable table;

  // Some directives need to be executed before memory allocation.
  for (const auto& i : _commandList) {
    if (i->executionTime() == IntermediateExecutionTime::BEFORE_ALLOCATION) {
      i->execute(representation, table, generator, state, memoryAccess);
    }
  }

  IntermediateMacroInstruction::replaceWithMacros(
      _commandList.begin(), _commandList.end(), state);

  allocator.clear();

  // We reserve our memory.
  for (const auto& i : _commandList) {
    i->allocateMemory(architecture, allocator, state);
  }

  std::size_t allocatedSize = allocator.calculatePositions();
  auto allowedSizeFuture = memoryAccess.getMemorySize();

  // Not sure about this... (if needed or not)
  allowedSizeFuture.wait();
  std::size_t allowedSize = allowedSizeFuture.get();

  if (allocatedSize > allowedSize) {
    state.addError("Too much memory allocated: " +
                   std::to_string(allocatedSize) + " requested, maximum is " +
                   std::to_string(allowedSize) +
                   " (please note: because of aligning memory, the first value "
                   "might be actually bigger than the memory allocated)");
  }

  // Next, we insert all our labels/constants into the SymbolTable.
  for (const auto& i : _commandList) {
    i->enhanceSymbolTable(table, allocator, state);
  }

  if (allocatedSize <= allowedSize) {
    // Then, we execute their values.
    for (const auto& i : _commandList) {
      if (i->executionTime() == IntermediateExecutionTime::AFTER_ALLOCATION) {
        i->execute(representation, table, generator, state, memoryAccess);
      }
    }
  }

  representation.errorList = state.errorList;

  // Now, we are done.
  return representation;
}
