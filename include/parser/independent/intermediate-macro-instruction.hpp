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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_INTERMEDIATE_MACRO_INSTRUCTION_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_INTERMEDIATE_MACRO_INSTRUCTION_HPP

#include <string>

#include "parser/independent/intermediate-operation.hpp"

class CompileErrorList;
class IntermediateInstruction;
class MacroDirective;
class MemoryAllocator;

/**
 * Wrapper instruction for an instanced macro.
 */
class IntermediateMacroInstruction : public IntermediateOperation {
 public:
  /**
   * Creates a new `IntermediateMacroInstruction` using an existing macro
   * directive.
   *
   * \param instruction The existing instruction to be replaced.
   * \param macro The macro to replace the instruction with.
   * \param macroTable The macro instruction table to replace every occuring sub
   * macro occurence.
   * \param errors The `CompileErrorList` to take down any errors during
   * generation.
   */
  IntermediateMacroInstruction(const IntermediateInstruction& instruction,
                               const MacroDirective& macro,
                               MacroDirectiveTable& macroTable,
                               CompileErrorList& errors);

  /**
     * Executes each of the sub-operations.
     *
     * \param immutable Some constant arguments which might be helpful.
     * \param errors The compile error list to note down any errors.
     * \param commandOutput The final command output vector to record all
   * finalized commands.
     * \param memoryAccess The memory access used to reserve memory and validate
   * instructions.
     */
  virtual void execute(const ExecuteImmutableArguments& immutable,
                       CompileErrorList& errors,
                       FinalCommandVector& commandOutput,
                       MemoryAccess& memoryAccess);

  /**
     * Reserves entries for this macro instruction in the symbol table.
     *
     * \param immutable Some constant arguments which might be helpful.
     * \param errors The compile error list to note down any errors.
     * \param graph The symbol graph for taking care of symbols (to check their
   * dependencies).
     */
  virtual void
  enhanceSymbolTable(const EnhanceSymbolTableImmutableArguments& immutable,
                     CompileErrorList& errors,
                     SymbolGraph& graph);

  /**
     * Reserves memory for this macro instruction.
     *
     * \param immutable Some constant arguments which might be helpful.
     * \param errors The compile error list to note down any errors.
     * \param allocator The allocator to reserve memory.
     * \param tracker The section tracker so we know in which section to reserve
   * our data.
     */
  virtual void allocateMemory(const AllocateMemoryImmutableArguments& immutable,
                              CompileErrorList& errors,
                              MemoryAllocator& allocator,
                              SectionTracker& tracker);

  /**
   * Replaces `IntermediateInstruction`s with `IntermediateMacroInstructions` in
   * iterable containers of `IntermediateOperationPointer`s.
   * \param begin The operation iterator begin.
   * \param end The operation iterator end.
   * \param macroTable The macro directive table.
   * \param errors The compile error list to note down errors.
   */
  static void replaceWithMacros(IntermediateOperationVectorIterator begin,
                                IntermediateOperationVectorIterator end,
                                MacroDirectiveTable& macroTable,
                                CompileErrorList& errors);

  /**
* Converts the instruction with all its arguments into a readable
* representation.
*
* \return A readable representation of this instruction.
*/
  virtual std::string toString() const;

  /**
   * Finalizes an intermediate macro instruction.
   */
  virtual ~IntermediateMacroInstruction() = default;

 private:
  /**
   * The operations which this macro contains.
   */
  IntermediateOperationVector _operations;

  /**
   * The first real instruction of this macro instruction, used for memory
   * locating.
   */
  int _firstInstruction = -1;
};


#endif
