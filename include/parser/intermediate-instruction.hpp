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

#ifndef ERAGPSIM_PARSER_INTERMEDIATE_INSTRUCTION_HPP
#define ERAGPSIM_PARSER_INTERMEDIATE_INSTRUCTION_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "arch/common/abstract-syntax-tree-node.hpp"
#include "parser/intermediate-operation.hpp"
#include "parser/memory-allocator.hpp"

class Architecture;

/**
 * \brief Represents a machine instruction in the parser-internal intermediate
 * form.
 */
class IntermediateInstruction : public IntermediateOperation {
 public:
  /**
   * \brief Instantiates a new compile error with the given arguments.
   * \param lines The line interval the operation occupies.
   * \param labels The vector of labels assigned to the operation.
   * \param name The name of the operation.
   * \param sources The source operands of the instruction.
   * \param targets The target operands of the instruction.
   */
  IntermediateInstruction(const LineInterval& lines,
                          const std::vector<std::string>& labels,
                          const std::string& name,
                          const std::vector<std::string>& sources,
                          const std::vector<std::string>& targets);

  virtual void execute(const ExecuteImmutableArguments& immutable,
                       CompileErrorAnnotator& annotator,
                       FinalRepresentation& finalRepresentator,
                       MemoryAccess& memoryAccess);

  virtual void
  enhanceSymbolTable(const EnhanceSymbolTableImmutableArguments& immutable,
                     CompileErrorAnnotator& annotator,
                     SymbolGraph& graph);

  virtual void allocateMemory(const PreprocessingImmutableArguments& immutable,
                              CompileErrorAnnotator& annotator,
                              MemoryAllocator& allocator,
                              SectionTracker& tracker);


  /**
   * \brief Converts this instruction into a syntax tree.
   * \param table The SymbolTable required for replacing the arguments.
   * \param generator The generator to transform the instructions.
   * \param state The CompileState logging all errors occuring.
   * \return The resulting syntax tree of the node.
   */
  FinalCommand compileInstruction(const ExecuteImmutableArguments& immutable,
                                  CompileErrorAnnotator& annotator,
                                  MemoryAccess& memoryAccess);

  MemoryAddress address() const;

  virtual void
  insertIntoArguments(const std::string& name, const std::string& value);

  virtual IntermediateOperationPointer clone();

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
  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>
  compileArgumentVector(const std::vector<std::string>& vector,
                        const ExecuteImmutableArguments& immutable,
                        CompileErrorAnnotator& annotator,
                        MemoryAccess& memoryAccess);

 private:
  friend class IntermediateMacroInstruction;

  /**
   * \brief The internal source arguments.
   */
  std::vector<std::string> _sources;

  /**
   * \brief The internal target arguments.
   */
  std::vector<std::string> _targets;

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
  std::vector<std::string> getArgsVector() const;
};

#endif
