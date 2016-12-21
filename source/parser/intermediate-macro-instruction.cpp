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

#include "parser/intermediate-macro-instruction.hpp"

#include "parser/compile-error-annotator.hpp"
#include "parser/intermediate-instruction.hpp"
#include "parser/macro-directive-table.hpp"
#include "parser/macro-directive.hpp"
#include "parser/memory-allocator.hpp"
#include "parser/symbol-graph.hpp"
#include "parser/symbol-replacer.hpp"


void IntermediateMacroInstruction::replaceWithMacros(
    CommandIterator begin,
    CommandIterator end,
    MacroDirectiveTable& macroTable,
    const CompileErrorAnnotator& annotator) {
  for (auto i = begin; i != end; ++i) {
    // Try casting to IntermediateInstruction, skip instruction if it doesn't
    // work
    if (dynamic_cast<IntermediateInstruction*>(i->get()) == nullptr) continue;

    IntermediateInstruction& inst = static_cast<IntermediateInstruction&>(**i);

    auto macro = macroTable.find(inst._name.string(),
                                 inst._sources.size() + inst._targets.size());
    if (!macro.found()) {
      continue;
    }

    if (macro.isCyclic()) {
      annotator.addError(macro.name().positionInterval(),
                         "Cyclic macro call detected.");
      continue;
    }

    IntermediateOperationPointer newPtr =
        std::make_shared<IntermediateMacroInstruction>(
            inst, macro->second, macroTable, annotator);
    *i = std::move(newPtr);
  }
}

IntermediateMacroInstruction::IntermediateMacroInstruction(
    const IntermediateInstruction& ins,
    const MacroDirective& macro,
    MacroDirectiveTable& macroTable,
    const CompileErrorAnnotator& annotator)
: IntermediateOperation(ins.lines(), ins.labels(), ins.name()) {
  for (size_t i = 0; i < macro.getOperationCount(); i++) {
    if (i == macro.firstInstructionIndex()) {
      _firstInstruction = _operations.size();
    }

    IntermediateOperationPointer ptr =
        macro.getOperation(i, ins.getArgsVector());

    if (ptr != nullptr) {
      _operations.push_back(std::move(ptr));
    } else {
      annotator.addError(macro.getOperationName(i).positionInterval(),
                         "Macro contains unsupported instruction '%1'.",
                         macro.getOperationName(i).string());
    }
  }

  // Recursively replace macro instructions to support recursive macro calls
  replaceWithMacros(
      _operations.begin(), _operations.end(), macroTable, annotator);
}
void IntermediateMacroInstruction::execute(
    const ExecuteImmutableArguments& immutable,
    const CompileErrorAnnotator& annotator,
    FinalRepresentation& finalRepresentator,
    MemoryAccess& memoryAccess) {
  for (const auto& operation : _operations) {
    operation->execute(immutable, annotator, finalRepresentator, memoryAccess);
  }
}

void IntermediateMacroInstruction::allocateMemory(
    const PreprocessingImmutableArguments& immutable,
    const CompileErrorAnnotator& annotator,
    MemoryAllocator& allocator,
    SectionTracker& tracker) {
  for (const auto& operation : _operations) {
    operation->allocateMemory(immutable, annotator, allocator, tracker);
  }
}

void IntermediateMacroInstruction::enhanceSymbolTable(
    const EnhanceSymbolTableImmutableArguments& immutable,
    const CompileErrorAnnotator& annotator,
    SymbolGraph& graph) {
  for (const auto& operation : _operations) {
    operation->enhanceSymbolTable(immutable, annotator, graph);
  }

  if (_labels.size() > 0 && _firstInstruction < 0) {
    std::vector<CodePositionInterval> wholeRegion;// TODO
    for (const auto& label : _labels) {
      wholeRegion.push_back(label.positionInterval());
    }
    annotator.addError(
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
