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

class Architecture;
class SymbolReplacer;
class MemoryAllocator;

class MemoryValue;
class FinalRepresentation;
class FinalCommand;
class MemoryAccess;
class CompileErrorAnnotator;
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

  virtual void execute(const ExecuteImmutableArguments& immutable,
                       const CompileErrorAnnotator& annotator,
                       FinalRepresentation& finalRepresentator,
                       MemoryAccess& memoryAccess) = 0;

  virtual void
  enhanceSymbolTable(const EnhanceSymbolTableImmutableArguments& immutable,
                     const CompileErrorAnnotator& annotator,
                     SymbolGraph& graph);

  virtual void allocateMemory(const PreprocessingImmutableArguments& immutable,
                              const CompileErrorAnnotator& annotator,
                              MemoryAllocator& allocator,
                              SectionTracker& tracker);

  virtual void precompile(const PreprocessingImmutableArguments& immutable,
                          const CompileErrorAnnotator& annotator,
                          MacroDirectiveTable& macroTable);

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

  const LineInterval& lines() const noexcept;

  const std::vector<std::string>& labels() const noexcept;

  const std::string& name() const noexcept;

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
