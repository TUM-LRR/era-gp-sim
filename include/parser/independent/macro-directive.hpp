/*
* C++ Assembler Interpreter
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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_MACRO_DIRECTIVE_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_MACRO_DIRECTIVE_HPP

#include <string>
#include <vector>

#include "common/optional.hpp"
#include "parser/independent/intermediate-directive.hpp"

class SymbolTable;

class MacroDirective : public IntermediateDirective {
 public:
  friend class MacroDirectiveTable;

  /**
   * \brief Instantiates a new MacroDirective with the given arguments.
   * \param positionInterval     The line interval the operation occupies.
   * \param labels    The vector of labels assigned to the operation.
   * \param name      The name of the operation. (e.g. '.macro')
   * \param arguments Arguments of the directive. First one should be the name
   *                  of the macro.
   */
  MacroDirective(const CodePositionInterval& positionInterval,
                 const std::vector<PositionedString>& labels,
                 const PositionedString& name,
                 const std::vector<PositionedString>& arguments);

  /**
   * \brief Instantiates a new MacroDirective with the given arguments.
   * \param positionInterval The line interval the operation occupies.
   * \param labels The vector of labels assigned to the operation.
   * \param name The name of the operation. (e.g. '.macro')
   * \param macroName The name of the macro.
   * \param macroParameters The parameter names of the macro, if any.
   */
  MacroDirective(const CodePositionInterval& positionInterval,
                 const std::vector<PositionedString>& labels,
                 const PositionedString& name,
                 const PositionedString& macroName,
                 const std::vector<PositionedString>& macroParameters = {});

  /**
   * \brief Preprocesses the macro.
   * \param immutable Some constant arguments which might be helpful.
   * \param errors The compile error list to note down any errors.
   * \param macroTable A table to record occured macros.
   */
  virtual void precompile(const PreprocessingImmutableArguments& immutable,
                          CompileErrorList& errors,
                          MacroDirectiveTable& macroTable) override;


  /**
   * \brief Specifies the new target for operations after this command.
   * \return Set ourselves as target.
   */
  virtual TargetSelector newTarget() const override;

  /**
   * \brief Inserts an operation into the internal command list.
   * \param pointer The operation to insert.
   */
  virtual void insert(IntermediateOperationPointer pointer) override;

  /**
   * \brief Returns the macro name.
   * \return The macro name.
   */
  const PositionedString& macroName() const noexcept;

  /**
   * Returns number of operations.
   */
  size_t getOperationCount() const;

  /**
   * Returns a pair with the minimum and maximum amount of parameters for this
   * macro.
   */
  std::pair<size_t, size_t> getParameterCount() const;

  /**
   * Returns if an instance of the macro is currently compiling. Used to detect
   * cyclic macro calls.
   */
  bool isCompiling();

  /**
   * Returns a copy of the instruction with index `index` after inserting
   * arguments.
   */
  IntermediateOperationPointer
  getOperation(size_t index,
               const std::vector<PositionedString>& arguments) const;

  int firstInstructionIndex() const;

  const PositionedString& getOperationName(size_t index) const;

  /**
   * Calls a function on an operation in this macro after given arguments have
   * been inserted.
   * \param index Index of the operation.
   * \param arguments Arguments to be inserted into this macro.
   * \param func Function to call. Has to be a member of IntermediateOperation.
   * \param args Arguments for `func`.
   */
  /*template <typename T, typename... U>
  void callOperationFunction(size_t index,
                             const std::vector<PositionedString>& arguments,
                             T func,
                             U&... args) {
    // Try to clone the operation and insert macro parameters.
    // If that doesnt work, call func directly on operation.
    IntermediateOperationPointer ptr;

    ptr = _operations[index]->clone();
    if (ptr != nullptr) {
      _macroParameters.insertParameters(ptr, arguments);
    }


    IntermediateOperation& op{ptr == nullptr ? *_operations[index] : *ptr};
    (op.*func)(args...);
  }*/

  /**
   * \brief Finalizes a macro directive.
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

    MacroParameters(std::vector<PositionedString>::const_iterator begin,
                    std::vector<PositionedString>::const_iterator end);

    MacroParameters(const std::vector<PositionedString>& arguments)
    : MacroParameters(arguments.begin(), arguments.end()) {
    }

    /**
     * Validates the macro parameters.
     * \param state Compile state to record errors.
     */
    void validate(CompileErrorList& errors) const;

    /**
     * Inserts all parameters into the operation.
     * \param operation Operation to insert into.
     * \param values Values to insert for the parameters.
     */
    void insertParameters(IntermediateOperationPointer& operation,
                          const std::vector<PositionedString>& values) const;

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
   * \brief The name of the macro.
   */
  PositionedString _macroName;

  /**
   * \brief The parameters of the macro.
   */
  MacroParameters _macroParameters;

  /**
   * \brief The operation list of the macro.
   */
  std::vector<IntermediateOperationPointer> _operations;

  bool _isCompiling = false;

  int _firstInstruction = -1;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_MACRO_DIRECTIVE_HPP */
