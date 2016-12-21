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
#include "parser/compile-error-annotator.hpp"
#include "parser/compile-error-list.hpp"
#include "parser/intermediate-macro-instruction.hpp"
#include "parser/intermediate-parameters.hpp"
#include "parser/macro-directive-table.hpp"
#include "parser/memory-allocator.hpp"
#include "parser/section-tracker.hpp"
#include "parser/symbol-graph-evaluation.hpp"
#include "parser/symbol-graph.hpp"
#include "parser/symbol-replacer.hpp"

IntermediateRepresentator::IntermediateRepresentator()
: _commandList(), _currentOutput(nullptr) {
}

void IntermediateRepresentator::insertCommandPtr(
    IntermediateOperationPointer&& command,
    const CompileErrorAnnotator& annotator) {
  // We got to handle the three target selector cases right here.
  if (command->newTarget() == TargetSelector::THIS) {
    // If we want the current command as new target, we set it like so.
    if (_currentOutput) {
      // Nested macros are not supported.
      annotator.addErrorHere("Error, nested macros are not supported.");
    }
    _currentOutput = std::move(command);
  } else {
    if (command->newTarget() == TargetSelector::MAIN) {
      // For the main selector, we may also insert the old command (otherwise
      // it and its sub commands might be lost).
      if (!_currentOutput) {
        // Classic bracket-forgot-to-close problem.
        annotator.addErrorHere("The start directive of the macro is missing.");
      }
      internalInsertCommand(std::move(_currentOutput));
    }

    // Finally, we may insert our handed-over command.
    internalInsertCommand(std::move(command));
  }
}

FinalRepresentation
IntermediateRepresentator::transform(const TransformationParameters& parameters,
                                     CompileErrorList errorList,
                                     MemoryAccess& memoryAccess) {
  CompileErrorAnnotator annotator(
      errorList, CodePositionInterval(CodePosition(0), CodePosition(0)));// TODO
  if (_currentOutput) {
    annotator.addErrorHere(
        "Macro not closed. Missing a macro end directive?");// TODO Code
                                                            // Position
  }

  FinalRepresentation representation;

  PreprocessingImmutableArguments preprocessingArguments(
      parameters.architecture(), parameters.generator());

  MacroDirectiveTable macroTable;
  for (const auto& command : _commandList) {
    command->precompile(preprocessingArguments, annotator, macroTable);
  }

  IntermediateMacroInstruction::replaceWithMacros(
      _commandList.begin(), _commandList.end(), macroTable, annotator);

  MemoryAllocator allocator(parameters.allocator());
  SectionTracker tracker;
  for (const auto& command : _commandList) {
    command->allocateMemory(
        preprocessingArguments, annotator, allocator, tracker);
  }

  std::size_t allocatedSize = allocator.calculatePositions();
  auto allowedSizeFuture = memoryAccess.getMemorySize();
  std::size_t allowedSize = allowedSizeFuture.get();

  if (allocatedSize > allowedSize) {
    annotator.addErrorHere(
        "Too much memory allocated: %1 requested, maximum is %2"
        " (please note: because of aligning memory, the first value "
        "might be actually bigger than the memory allocated)",
        std::to_string(allocatedSize),
        std::to_string(allowedSize));
    representation.errorList = annotator.errorList().errors();
    return representation;
  }

  SymbolGraph graph;
  EnhanceSymbolTableImmutableArguments symbolTableArguments(
      preprocessingArguments, allocator);
  for (const auto& command : _commandList) {
    command->enhanceSymbolTable(symbolTableArguments, annotator, graph);
  }

  auto graphEvaluation = graph.evaluate();// TODO more graph evaluation!
  SymbolReplacer replacer(graphEvaluation);
  ExecuteImmutableArguments executeArguments(symbolTableArguments, replacer);
  for (const auto& command : _commandList) {
    command->execute(executeArguments, annotator, representation, memoryAccess);
  }

  representation.errorList =
      annotator.errorList()
          .errors();// TODO beautify by making FinalRepresentation a class.

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
