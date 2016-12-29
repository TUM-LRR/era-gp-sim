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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_INTERMEDIATE_INSTRUCTION_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_INTERMEDIATE_INSTRUCTION_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "arch/common/abstract-syntax-tree-node.hpp"
#include "parser/independent/intermediate-operation.hpp"
#include "parser/independent/memory-allocator.hpp"

class Architecture;

/**
 * \brief Represents a machine instruction in the parser-internal intermediate
 * form.
 */
class IntermediateInstruction : public IntermediateOperation {
 public:
  /**
   * \brief Instantiates a new compile error with the given arguments.
   * \param positionInterval The line interval the operation occupies.
   * \param labels The vector of labels assigned to the operation.
   * \param name The name of the operation.
   * \param sources The source operands of the instruction.
   * \param targets The target operands of the instruction.
   */
  IntermediateInstruction(const CodePositionInterval& positionInterval,
                          const std::vector<PositionedString>& labels,
                          const PositionedString& name,
                          const std::vector<PositionedString>& sources,
                          const std::vector<PositionedString>& targets);

  /**
* \brief Executes the operation (i.e. it is inserted into the commandOutput
* list).
* \param immutable Some constant arguments which might be helpful.
* \param errors The compile error list to note down any errors.
* \param commandOutput The final command output vector to record all finalized
* commands.
* \param memoryAccess The memory access used to reserve memory and validate
* instructions.
*/
  virtual void execute(const ExecuteImmutableArguments& immutable,
                       CompileErrorList& errors,
                       FinalCommandVector& commandOutput,
                       MemoryAccess& memoryAccess) override;

  /**
     * \brief Reserves entries for this operation in the symbol table, inserts
   * all labels.
     * \param immutable Some constant arguments which might be helpful.
     * \param errors The compile error list to note down any errors.
     * \param graph The symbol graph for taking care of symbols (to check their
   * dependencies).
     */
  virtual void
  enhanceSymbolTable(const EnhanceSymbolTableImmutableArguments& immutable,
                     CompileErrorList& errors,
                     SymbolGraph& graph) override;

  /**
     * \brief Reserves memory for this operation.
     * \param immutable Some constant arguments which might be helpful.
     * \param errors The compile error list to note down any errors.
     * \param allocator The allocator to reserve memory.
     * \param tracker The section tracker so we know in which section to reserve
   * our data.
     */
  virtual void allocateMemory(const PreprocessingImmutableArguments& immutable,
                              CompileErrorList& errors,
                              MemoryAllocator& allocator,
                              SectionTracker& tracker) override;


  /**
   * \brief Converts this instruction into a syntax tree.
   * \param table The SymbolTable required for replacing the arguments.
   * \param generator The generator to transform the instructions.
   * \param state The CompileState logging all errors occuring.
   * \return The resulting syntax tree of the node.
   */
  FinalCommand compileInstruction(const ExecuteImmutableArguments& immutable,
                                  CompileErrorList& errors,
                                  MemoryAccess& memoryAccess);

  /**
   * \brief Returns the memory address of this operation.
   * \return The memory address of this operation.
   */
  MemoryAddress address() const noexcept;

  virtual void insertIntoArguments(const PositionedString& name,
                                   const PositionedString& value) override;

  virtual IntermediateOperationPointer clone() override;

  virtual std::string toString() const override;

  virtual Type getType() const override;

  /**
   * \brief Finalizes an intermediate instruction.
   */
  virtual ~IntermediateInstruction() = default;

  const std::vector<PositionedString>& sources() const noexcept;
  const std::vector<PositionedString>& targets() const noexcept;

 protected:
  /**
   * \brief Compiles a vector of arguments (i.e. inserts symbols and converts to
   * syntax tree nodes).
   * \param vector The vector to compile.
   * \param table The SymbolTable required for replacing the arguments.
   * \param generator The generator to transform the instructions.
   * \param state The CompileState logging all errors occuring.
   * \return The compiled vector of arguments.
   */
  std::vector<std::shared_ptr<AbstractSyntaxTreeNode>>
  compileArgumentVector(const std::vector<PositionedString>& vector,
                        const ExecuteImmutableArguments& immutable,
                        CompileErrorList& errors,
                        MemoryAccess& memoryAccess);

 private:
  friend class IntermediateMacroInstruction;

  /**
   * \brief The internal source arguments.
   */
  std::vector<PositionedString> _sources;

  /**
   * \brief The internal target arguments.
   */
  std::vector<PositionedString> _targets;

  /**
   * \brief The internal memory address.
   */
  MemoryAddress _address;

  /**
   * \brief The memory address inside the code section.
   */
  RelativeMemoryPosition _relativeAddress;

  /**
   * Constructs an argument vector from the sources and targets vectors.
   * \return New vector containing all instruction arguments.
   */
  std::vector<PositionedString> getArgsVector() const;
};

#endif
