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

#ifndef ERAGPSIM_PARSER_INTERMEDIATE_MACRO_INSTRUCTION_HPP
#define ERAGPSIM_PARSER_INTERMEDIATE_MACRO_INSTRUCTION_HPP

#include "parser/intermediate-instruction.hpp"
#include "parser/intermediate-operation.hpp"

/**
 * Wrapper instruction for an instanced macro.
 */
class IntermediateMacroInstruction : public IntermediateOperation {
 public:
  IntermediateMacroInstruction(const IntermediateInstruction&,
                               const MacroDirective&,
                               CompileState& state);

  virtual void execute(FinalRepresentation& finalRepresentator,
                       const SymbolTable& table,
                       const SyntaxTreeGenerator& generator,
                       CompileState& state,
                       MemoryAccess& memoryAccess);

  virtual void allocateMemory(const Architecture& architecture,
                              MemoryAllocator& allocator,
                              CompileState& state);

  virtual void enhanceSymbolTable(SymbolTable& table,
                                  const MemoryAllocator& allocator,
                                  CompileState& state);

  /**
   * Replaces `IntermediateInstruction`s with `IntermediateMacroInstructions` in
   * iterable containers of `IntermediateOperationPointer`s.
   */
  template <typename It>
  static void replaceWithMacros(It begin, It end, CompileState& state) {
    for (It i = begin; i != end; ++i) {
      // Try casting to IntermediateInstruction, skip instruction if it doesn't
      // work
      if (dynamic_cast<IntermediateInstruction*>(i->get()) == nullptr) continue;

      IntermediateInstruction& inst =
          static_cast<IntermediateInstruction&>(**i);

      auto macro = state.macros.find(
          inst._name, inst._sources.size() + inst._targets.size());
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

  virtual std::string toString() const;

 private:
  std::vector<IntermediateOperationPointer> _operations;
  int _firstInstruction = -1;
};


#endif
