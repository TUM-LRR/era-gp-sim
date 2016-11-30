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

#include "parser/compile-state.hpp"
#include "parser/intermediate-instruction.hpp"
#include "parser/macro-directive.hpp"
#include "parser/memory-allocator.hpp"
#include "parser/symbol-table.hpp"


void IntermediateMacroInstruction::replaceWithMacros(CommandIterator begin,
                                                     CommandIterator end,
                                                     CompileState& state) {
  for (auto i = begin; i != end; ++i) {
    // Try casting to IntermediateInstruction, skip instruction if it doesn't
    // work
    if (dynamic_cast<IntermediateInstruction*>(i->get()) == nullptr) continue;

    IntermediateInstruction& inst = static_cast<IntermediateInstruction&>(**i);

    auto macro = state.macros.find(inst._name,
                                   inst._sources.size() + inst._targets.size());
    if (!macro.found()) continue;

    if (macro.isCyclic()) {
      state.addError("Cyclic macro call!");
      continue;
    }

    IntermediateOperationPointer newPtr =
        std::make_unique<IntermediateMacroInstruction>(
            inst, macro->second, state);
    *i = std::move(newPtr);
  }
}

IntermediateMacroInstruction::IntermediateMacroInstruction(
    const IntermediateInstruction& ins,
    const MacroDirective& macro,
    CompileState& state)
: IntermediateOperation(ins.lines(), ins.labels(), ins.name()) {
  for (size_t i = 0; i < macro.getOperationCount(); i++) {
    if (i == macro.firstInstructionIndex()) {
      _firstInstruction = _operations.size();
    }

    IntermediateOperationPointer ptr =
        macro.getOperation(i, ins.getArgsVector());

    if (ptr != nullptr)
      _operations.push_back(std::move(ptr));
    else
      state.addError("Macro contains unsupported instruction '" +
                     macro.getOperationName(i) + "'.");
  }

  // Recursively replace macro instructions to support recursive macro calls
  replaceWithMacros(_operations.begin(), _operations.end(), state);
}

void IntermediateMacroInstruction::execute(
    FinalRepresentation& finalRepresentator,
    const SymbolTable& table,
    const SyntaxTreeGenerator& generator,
    CompileState& state,
    MemoryAccess& memoryAccess) {
  for (const auto& operation : _operations) {
    operation->execute(
        finalRepresentator, table, generator, state, memoryAccess);
  }
}

void IntermediateMacroInstruction::allocateMemory(
    const Architecture& architecture,
    MemoryAllocator& allocator,
    CompileState& state) {
  for (const auto& operation : _operations) {
    operation->allocateMemory(architecture, allocator, state);
  }
}

void IntermediateMacroInstruction::enhanceSymbolTable(
    SymbolTable& table, const MemoryAllocator& allocator, CompileState& state) {
  for (const auto& operation : _operations) {
    operation->enhanceSymbolTable(table, allocator, state);
  }

  if (_labels.size() > 0 && _firstInstruction < 0) {
    state.addError("Labels cant point to macros without instructions!");
  } else {
    for (const auto& i : _labels) {
      table.insertEntry(i,
                        std::to_string(static_cast<IntermediateInstruction*>(
                                           _operations[_firstInstruction].get())
                                           ->address()),
                        state,
                        SymbolTable::SymbolType::LABEL);
    }
  }
}
