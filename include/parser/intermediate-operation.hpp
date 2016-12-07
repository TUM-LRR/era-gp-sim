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

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "parser/line-interval.hpp"

class MemoryValue;
class CompileState;
class Architecture;
class SyntaxTreeGenerator;
class SymbolTable;
class FinalRepresentation;
class FinalCommand;
class MemoryAccess;
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

/**
 * Specifies whene an instruction should be executed.
 *
 * Some directives like macros need to be executed before memory allocation,
 * otherwise they wouldn't be known during `allocateMemory`.
 */
enum class IntermediateExecutionTime { BEFORE_ALLOCATION, AFTER_ALLOCATION };

class IntermediateOperation;
class Architecture;
class MemoryAllocator;

/**
 * \brief Convenience class for a pointer to an operation.
 */
using IntermediateOperationPointer = std::shared_ptr<IntermediateOperation>;

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
                        const std::string& name);

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
                              CompileState& state);

  /**
   * \brief Enhances the symbol table by the labels of the operation.
   * \param table The SymbolTable to insert into.
   * \param allocator The MemoryAllocator to get the memory positions from.
   * \param state The CompileState to log possible errors.
   */
  virtual void enhanceSymbolTable(SymbolTable& table,
                                  const MemoryAllocator& allocator,
                                  CompileState& state);

  /**
   * \brief Specifies if the this operation should be processed.
   * \return True, if so, else false.
   */
  virtual bool shouldInsert() const;

  /**
   * \brief Specifies the new target for operations after this command.
   * \return Normally, we keep the target.
   */
  virtual TargetSelector newTarget() const;

  /**
   * Returns when to execute this operation.
   */
  virtual IntermediateExecutionTime executionTime() const;

  /**
   * \brief Inserts an operation into a possible internal command list.
   * \param pointer The operation to insert.
   */
  virtual void insert(IntermediateOperationPointer pointer);

  /**
   * Inserts a value for the variable parameter called name.
   * \param name  Name of the parameter.
   * \param value Value of the parameter.
   */
  virtual void
  insertIntoArguments(const std::string& name, const std::string& value);

  /**
   * Clones the operation if supported.
   * \return Pointer to cloned operation if supported. Otherwise `nullptr`.
   */
  virtual IntermediateOperationPointer clone();

  const LineInterval& lines() const;

  const std::vector<std::string>& labels() const;

  const std::string& name() const;

  virtual std::string toString() const {
    return _name + "\n";
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

using CommandList = std::vector<IntermediateOperationPointer>;
using CommandIterator = typename CommandList::iterator;

#endif
