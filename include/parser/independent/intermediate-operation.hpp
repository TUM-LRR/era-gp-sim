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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_INTERMEDIATE_OPERATION_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_INTERMEDIATE_OPERATION_HPP

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "parser/common/code-position-interval.hpp"
#include "parser/common/final-command.hpp"
#include "parser/independent/positioned-string.hpp"

class Architecture;
class SymbolReplacer;
class MemoryAllocator;

class MemoryValue;
class MemoryAccess;
class CompileErrorList;
class SectionTracker;
class MacroDirectiveTable;
class SymbolGraph;

class IntermediateOperation;
class Architecture;
class MemoryAllocator;

class ExecuteImmutableArguments;
class EnhanceSymbolTableImmutableArguments;
class PreprocessingImmutableArguments;

/**
 * A memory address substitute as long as we do not have one.
 */
using MemoryAddress = std::size_t;

/**
 * Specifies the target for operations to put.
 *
 * This feature has been implemented to support macros. It allows that the
 * operations are placed inside of other operations on syntax level.
 */
enum class TargetSelector { KEEP, MAIN, THIS };

/**
 * Convenience class for a pointer to an operation.
 */
using IntermediateOperationPointer = std::shared_ptr<IntermediateOperation>;

/**
 * Represents an abstract assembler operation in the parser-internal
 * intermediate form.
 */
class IntermediateOperation {
 public:
  enum class Type { OTHER, INSTRUCTION, MACRO_INSTRUCTION };

  /**
   * Instantiates a new IntermediateOperation with the given arguments.
   * (only for subclass use!)
   * \param positionInterval The code position interval the operation occupies.
   * \param labels The vector of labels assigned to the operation.
   * \param name The name of the operation.
   */
  IntermediateOperation(const CodePositionInterval& positionInterval,
                        const PositionedStringVector& labels,
                        const PositionedString& name);

  /**
   * Executes the operation (e.g. it is inserted into the commandOutput
   * list).
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
   * Reserves entries for this operation in the symbol table.
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
* Reserves memory for this operation.
* \param immutable Some constant arguments which might be helpful.
* \param errors The compile error list to note down any errors.
* \param allocator The allocator to reserve memory.
* \param tracker The section tracker so we know in which section to reserve our
* data.
*/
  virtual void allocateMemory(const PreprocessingImmutableArguments& immutable,
                              CompileErrorList& errors,
                              MemoryAllocator& allocator,
                              SectionTracker& tracker);

  /**
* Preprocesses some operations, mostly used for macros.
* \param immutable Some constant arguments which might be helpful.
* \param errors The compile error list to note down any errors.
* \param macroTable A table to record occured macros.
*/
  virtual void precompile(const PreprocessingImmutableArguments& immutable,
                          CompileErrorList& errors,
                          MacroDirectiveTable& macroTable);

  /**
   * Specifies if the this operation should be processed.
   * \return True, if so, else false.
   */
  virtual bool shouldInsert() const;

  /**
   * Specifies the new target for operations after this command.
   * \return Normally, we keep the target.
   */
  virtual TargetSelector newTarget() const;

  /**
   * Inserts an operation into a possible internal command list.
   * \param pointer The operation to insert.
   */
  virtual void insert(const IntermediateOperationPointer& pointer);

  /**
   * Inserts a value for the variable parameter called name.
   * \param name  Name of the parameter.
   * \param value Value of the parameter.
   */
  virtual void insertIntoArguments(const PositionedString& name,
                                   const PositionedString& value);

  /**
   * Clones the operation if supported.
   * \return Pointer to cloned operation if supported. Otherwise `nullptr`.
   */
  virtual IntermediateOperationPointer clone();

  /**
   * Returns the internal code position interval.
   * \return The internal code position interval.
   */
  const CodePositionInterval& positionInterval() const noexcept;

  /**
   * Returns the internal label list.
   * \return The internal label list.
   */
  const PositionedStringVector& labels() const noexcept;

  /**
   * Returns the internal operation name.
   * \return The internal operation name.
   */
  const PositionedString& name() const noexcept;

  /**
   * Converts the operation with all its arguments into a readable
   * representation.
   * \return A readable representation of this operation.
   */
  virtual std::string toString() const;

  /**
   * Returns the type of this operation.
   * \return The type of this operation (instruction, macro instruction, other,
   * etc.).
   */
  virtual Type getType() const;

  /**
   * Finalizes an intermediate operation.
   */
  virtual ~IntermediateOperation() = default;

 private:
  /**
   * The internal code position interval.
   */
  CodePositionInterval _positionInterval;

  /**
   * The internal label list.
   */
  PositionedStringVector _labels;

  /**
   * The internal operation name.
   */
  PositionedString _name;
};

using IntermediateOperationVector = std::vector<IntermediateOperationPointer>;
using IntermediateOperationVectorIterator =
    typename IntermediateOperationVector::iterator;

#endif
