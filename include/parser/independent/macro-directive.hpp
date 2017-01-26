/*
* ERASIM Assembler Interpreter
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

#ifndef ERASIM_PARSER_INDEPENDENT_MACRO_DIRECTIVE_HPP
#define ERASIM_PARSER_INDEPENDENT_MACRO_DIRECTIVE_HPP

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "common/optional.hpp"
#include "parser/independent/intermediate-directive.hpp"

/**
 * A macro directive, denoting the start of a macro definition in code.
 */
class MacroDirective : public IntermediateDirective {
 public:
  friend class MacroDirectiveTable;

  /**
   * Instantiates a new MacroDirective with the given arguments.
   *
   * \param positionInterval     The line interval the operation occupies.
   * \param labels    The vector of labels assigned to the operation.
   * \param name      The name of the operation. (e.g. '.macro')
   * \param arguments Arguments of the directive. First one should be the name
   *                  of the macro.
   */
  MacroDirective(const CodePositionInterval& positionInterval,
                 const PositionedStringVector& labels,
                 const PositionedString& name,
                 const PositionedStringVector& arguments);

  /**
   * Instantiates a new MacroDirective with the given arguments.
   *
   * \param positionInterval The line interval the operation occupies.
   * \param labels The vector of labels assigned to the operation.
   * \param name The name of the operation. (e.g. '.macro')
   * \param macroName The name of the macro.
   * \param macroParameters The parameter names of the macro, if any.
   */
  MacroDirective(const CodePositionInterval& positionInterval,
                 const PositionedStringVector& labels,
                 const PositionedString& name,
                 const PositionedString& macroName,
                 const PositionedStringVector& macroParameters = {});

  /**
   * Preprocesses the macro.
   *
   * \param immutable Some constant arguments which might be helpful.
   * \param errors The compile error list to note down any errors.
   * \param macroTable A table to record occured macros.
   */
  virtual void precompile(const PrecompileImmutableArguments& immutable,
                          CompileErrorList& errors,
                          SymbolGraph& graph,
                          MacroDirectiveTable& macroTable);


  /**
   * Specifies the new target for operations after this command.
   *
   * \return Set ourselves as target.
   */
  virtual TargetSelector newTarget() const;

  /**
   * Inserts an operation into the internal command list.
   *
   * \param pointer The operation to insert.
   */
  virtual void insert(const IntermediateOperationPointer& pointer);

  /**
   * \return The macro name.
   */
  const PositionedString& macroName() const noexcept;

  /**
   * \return The number of operations.
   */
  size_t getOperationCount() const;

  /**
   * \return A pair with the minimum and maximum amount of parameters for this
   * macro.
   */
  std::pair<size_t, size_t> getParameterCount() const;

  /**
   * \return If an instance of the macro is currently compiling. Used to detect
   * cyclic macro calls.
   */
  bool isCompiling();

  /**
   * \return A copy of the instruction with index `index` after inserting
   * arguments.
   */
  IntermediateOperationPointer
  getOperation(size_t index, const PositionedStringVector& arguments) const;

  /**
   * \return The index of the first real instruction.
   */
  int firstInstructionIndex() const;

  /**
   * Returns the name of the operation at the given index.
   *
   * \param index The given index.
   * \return The resulting operation string.
   */
  const PositionedString& getOperationName(size_t index) const;

  /**
   * Finalizes a macro directive.
   */
  virtual ~MacroDirective() = default;

 protected:
  /**
   * Helper class which manages macro parameters.
   */
  class MacroParameters {
   public:
    using MacroParameter =
        std::pair<PositionedString, Optional<PositionedString>>;

    /**
     * Creates a new `MacroParameters` class from the given iterator bounds.
     *
     * \param begin The begin of the iterator.
     * \param end The end of the iterator.
     */
    MacroParameters(PositionedStringVector::const_iterator begin,
                    PositionedStringVector::const_iterator end);

    /**
     * Creates a new `MacroParameters` class from the given vector.
     *
     * \param arguments The given vector.
     */
    explicit MacroParameters(const PositionedStringVector& arguments);

    /**
     * Validates the macro parameters.
     * \param state Compile state to record errors.
     */
    void validate(CompileErrorList& errors) const;

    /**
     * Inserts all parameters into the operation.
     *
     * \param operation Operation to insert into.
     * \param values Values to insert for the parameters.
     */
    void insertParameters(IntermediateOperationPointer& operation,
                          const PositionedStringVector& values) const;

    /**
     * Returns a pair with the minimum and maximum amount of parameters for this
     * macro.
     */
    std::pair<size_t, size_t> getParameterCount() const {
      return {_minParams, _params.size()};
    }

   private:
    /**
     * Vector of all the parameters as `{name, optional default value}` pairs.
     */
    std::vector<MacroParameter> _params;

    /**
     * Minumum amount of parameters this operation needs.
     */
    size_t _minParams;
  };


 private:
  /**
   * The name of the macro.
   */
  PositionedString _macroName;

  /**
   * The parameters of the macro.
   */
  MacroParameters _macroParameters;

  /**
   * The operation list of the macro.
   */
  std::vector<IntermediateOperationPointer> _operations;

  /**
   * Denotes, if the directive compiles.
   */
  bool _isCompiling = false;

  /**
   * The index of the first instruction in memory.
   */
  int _firstInstruction = -1;
};

#endif /* ERASIM_PARSER_INDEPENDENT_MACRO_DIRECTIVE_HPP */
