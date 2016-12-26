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

#include "parser/independent/intermediate-macro-instruction.hpp"

#include "parser/common/compile-error-list.hpp"
#include "parser/independent/intermediate-instruction.hpp"
#include "parser/independent/macro-directive-table.hpp"
#include "parser/independent/macro-directive.hpp"
#include "parser/independent/memory-allocator.hpp"
#include "parser/independent/symbol-graph.hpp"
#include "parser/independent/symbol-replacer.hpp"


void IntermediateMacroInstruction::replaceWithMacros(
    CommandIterator begin,
    CommandIterator end,
    MacroDirectiveTable& macroTable,
    CompileErrorList& errors) {
  for (auto i = begin; i != end; ++i) {
    // Try casting to IntermediateInstruction, skip instruction if it doesn't
    // work
    if ((*i)->getType() != IntermediateOperation::Type::INSTRUCTION) continue;

    IntermediateInstruction& inst = static_cast<IntermediateInstruction&>(**i);

    auto macro = macroTable.find(inst._name.string(),
                                 inst._sources.size() + inst._targets.size());
    if (!macro.found()) {
      continue;
    }

    if (macro.isCyclic()) {
      errors.pushError(macro.name().positionInterval(),
                       "Cyclic macro call detected.");
      continue;
    }

    IntermediateOperationPointer newPtr =
        std::make_shared<IntermediateMacroInstruction>(
            inst, macro->second, macroTable, errors);
    *i = std::move(newPtr);
  }
}

IntermediateMacroInstruction::IntermediateMacroInstruction(
    const IntermediateInstruction& ins,
    const MacroDirective& macro,
    MacroDirectiveTable& macroTable,
    CompileErrorList& errors)
: IntermediateOperation(ins.positionInterval(), ins.labels(), ins.name()) {
  for (auto i : Utility::range<size_t>(0, macro.getOperationCount())) {
    if (i == (size_t)macro.firstInstructionIndex() &&
        macro.firstInstructionIndex() >= 0) {
      _firstInstruction = _operations.size();
    }

    IntermediateOperationPointer ptr =
        macro.getOperation(i, ins.getArgsVector());

    if (ptr != nullptr) {
      _operations.push_back(std::move(ptr));
    } else {
      errors.pushError(macro.getOperationName(i).positionInterval(),
                       "Macro contains unsupported instruction '%1'.",
                       macro.getOperationName(i).string());
    }
  }

  // Recursively replace macro instructions to support recursive macro calls
  replaceWithMacros(_operations.begin(), _operations.end(), macroTable, errors);
}
void IntermediateMacroInstruction::execute(
    const ExecuteImmutableArguments& immutable,
    CompileErrorList& errors,
    FinalCommandVector& commandOutput,
    MemoryAccess& memoryAccess) {
  for (const auto& operation : _operations) {
    operation->execute(immutable, errors, commandOutput, memoryAccess);
  }
}

void IntermediateMacroInstruction::allocateMemory(
    const PreprocessingImmutableArguments& immutable,
    CompileErrorList& errors,
    MemoryAllocator& allocator,
    SectionTracker& tracker) {
  for (const auto& operation : _operations) {
    operation->allocateMemory(immutable, errors, allocator, tracker);
  }
}

void IntermediateMacroInstruction::enhanceSymbolTable(
    const EnhanceSymbolTableImmutableArguments& immutable,
    CompileErrorList& errors,
    SymbolGraph& graph) {
  for (const auto& operation : _operations) {
    operation->enhanceSymbolTable(immutable, errors, graph);
  }

  if (_labels.size() > 0 && _firstInstruction < 0) {
    std::vector<CodePositionInterval> wholeRegion;// TODO
    for (const auto& label : _labels) {
      wholeRegion.push_back(label.positionInterval());
    }
    errors.pushError(
        CodePositionInterval().unite(wholeRegion.begin(), wholeRegion.end()),
        "Labels cannot point to macros without instructions!");
  } else {
    for (const auto& label : _labels) {
      graph.addNode(
          Symbol(label,
                 PositionedString(
                     std::to_string(static_cast<IntermediateInstruction*>(
                                        _operations[_firstInstruction].get())
                                        ->address()),
                     label.positionInterval() /*TODO?*/),
                 SymbolBehavior::DYNAMIC));
    }
  }
}

std::string IntermediateMacroInstruction::toString() const {
  std::string str;
  for (const auto& operation : _operations) {
    str += operation->toString();
  }
  return str;
}

IntermediateOperation::Type IntermediateMacroInstruction::getType() const {
  return Type::MACRO_INSTRUCTION;
}
