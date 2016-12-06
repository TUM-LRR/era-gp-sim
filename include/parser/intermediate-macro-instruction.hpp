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

#include "parser/intermediate-operation.hpp"

class CompileState;
class IntermediateInstruction;
class MacroDirective;
class MemoryAllocator;

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
  static void replaceWithMacros(CommandIterator begin,
                                CommandIterator end,
                                CompileState& state);

 private:
  CommandList _operations;
  int _firstInstruction = -1;
};


#endif
