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

#ifndef ERAGPSIM_PARSER_INTERMEDIATE_OPERATION_HPP
#define ERAGPSIM_PARSER_INTERMEDIATE_OPERATION_HPP

#include <string>
#include <vector>
#include "common/assert.hpp"
#include "parser/final-representation.hpp"
#include "parser/line-interval.hpp"
#include "parser/memory-allocator.hpp"
#include "parser/symbol-table.hpp"
#include "parser/syntax-tree-generator.hpp"

struct CompileState;

/**
 * \brief A memory address substitute as long as we do not have one.
 */
using MemoryAddress = std::size_t;

/**
 * \brief Specifies the target for operations to put.
 *
 * This feature has been implemented to support macros. It allows that the
 * operations are placed inside of other operations on syntax level.
 */
enum class TargetSelector { KEEP, MAIN, THIS };

class IntermediateOperation;
class Architecture;
class MemoryAllocator;

/**
 * \brief Convenience class for a pointer to an operation.
 */
using IntermediateOperationPointer = std::unique_ptr<IntermediateOperation>;

/**
 * \brief Represents an abstract assembler operation in the parser-internal
 * intermediate form.
 */
class IntermediateOperation {
 public:
  /**
   * \brief Instantiates a new IntermediateOperation with the given arguments.
   * (only for subclass use!)
   * \param lines The line interval the operation occupies.
   * \param labels The vector of labels assigned to the operation.
   * \param name The name of the operation.
   */
  IntermediateOperation(const LineInterval& lines,
                        const std::vector<std::string>& labels,
                        const std::string& name)
  : _lines(lines), _labels(labels), _name(name) {
  }

  /**
   * \brief Executes the given operation (somehow).
   * \param finalRepresentator The FinalRepresentation for possible output.
   * \param table The SymbolTable for possible replacements.
   * \param generator The generator to transform the instructions.
   * \param state The CompileState to log possible errors.
   * \param memoryAccess The MemoryAccess for verifying instructions or
   * reserving data.
   */
  virtual void execute(FinalRepresentation& finalRepresentator,
                       const SymbolTable& table,
                       const SyntaxTreeGenerator& generator,
                       CompileState& state,
                       MemoryAccess& memoryAccess) = 0;

  /**
   * \brief Reserves (not writes!) memory for the operation (if needed).
   * \param architecture The architecture for information about the memory
   * format.
   * \param allocator The allocator to reserve memory.
   * \param state The CompileState to log possible errors.
   */
  virtual void allocateMemory(const Architecture& architecture,
                              MemoryAllocator& allocator,
                              CompileState& state) {
  }

  /**
   * \brief Enhances the symbol table by the labels of the operation.
   * \param table The SymbolTable to insert into.
   * \param allocator The MemoryAllocator to get the memory positions from.
   * \param state The CompileState to log possible errors.
   */
  virtual void enhanceSymbolTable(SymbolTable& table,
                                  const MemoryAllocator& allocator,
                                  CompileState& state) {
  }

  /**
   * \brief Specifies if the this operation should be processed.
   * \return True, if so, else false.
   */
  virtual bool shouldInsert() const {
    return true;
  }

  /**
   * \brief Specifies the new target for operations after this command.
   * \return Normally, we keep the target.
   */
  virtual TargetSelector newTarget() const {
    return TargetSelector::KEEP;
  }

  /**
   * \brief Inserts an operation into a possible internal command list.
   * \param pointer The operation to insert.
   */
  virtual void insert(IntermediateOperationPointer pointer) {
    // If this happens, something has gone wrong in our programming.
    assert::that(false);
  }

 protected:
  /**
   * \brief The internal line interval.
   */
  LineInterval _lines;

  /**
   * \brief The internal label list.
   */
  std::vector<std::string> _labels;

  /**
   * \brief The internal operation name.
   */
  std::string _name;
};

#endif
