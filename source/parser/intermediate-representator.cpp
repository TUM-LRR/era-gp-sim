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
#include "parser/memory-allocator.hpp"
#include "parser/symbol-table.hpp"

IntermediateRepresentator::IntermediateRepresentator()
: _commandList(), _currentOutput(nullptr) {
}

void IntermediateRepresentator::insertCommandPtr(
    IntermediateOperationPointer&& command, CompileState& state) {
  // We got to handle the three target selector cases right here.
  if (command->newTarget() == TargetSelector::THIS) {
    // If we want the current command as new target, we set it like so.
    if (_currentOutput) {
      // Nested macros are not supported.
      state.addError("Error, nested macros are not supported.");
    }
    _currentOutput = std::move(command);
  } else {
    if (command->newTarget() == TargetSelector::MAIN) {
      // For the main selector, we may also insert the old command (otherwise
      // it and its sub commands might be lost).
      if (!_currentOutput) {
        // Classic bracket forgot to close problem.
        state.addError("The start directive of the macro is missing.");
      }
      internalInsertCommand(std::move(_currentOutput));
    }

    // Finally, we may insert our handed-over command.
    internalInsertCommand(std::move(command));
  }
}

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

  auto noCycle = table.finalizeEntries();
  if (!noCycle)
  {
    state.addError("Detected a cycle between symbols.");
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

void IntermediateRepresentator::internalInsertCommand(
    IntermediateOperationPointer pointer) {
  // Of course, it should be valid and is allowed to be inserted.
  if (pointer && pointer->shouldInsert()) {
    // We got to decide if there is an alternative output.
    if (_currentOutput) {
      _currentOutput->insert(std::move(pointer));
    } else {
      _commandList.push_back(std::move(pointer));
    }
  }
}
