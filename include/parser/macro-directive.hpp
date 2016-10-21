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

#ifndef ERAGPSIM_PARSER_MACRO_DIRECTIVE_HPP
#define ERAGPSIM_PARSER_MACRO_DIRECTIVE_HPP

#include "parser/intermediate-directive.hpp"

class MacroDirective : public IntermediateDirective {
 public:
  /**
   * \brief Instantiates a new MacroDirective with the given arguments.
   * \param lines     The line interval the operation occupies.
   * \param labels    The vector of labels assigned to the operation.
   * \param name      The name of the operation. (e.g. '.macro')
   * \param arguments Arguments of the directive. First one should be the name
   *                  of the macro.
   */
  MacroDirective(const LineInterval& lines,
                 const std::vector<std::string>& labels,
                 const std::string& name,
                 const std::vector<std::string>& arguments)
  : IntermediateDirective(lines, labels, name)
  , _macroName(arguments[0])
  , _macroParameters(arguments.begin() + 1, arguments.end())
  , _operations() {
  }

  /**
   * \brief Instantiates a new MacroDirective with the given arguments.
   * \param lines The line interval the operation occupies.
   * \param labels The vector of labels assigned to the operation.
   * \param name The name of the operation. (e.g. '.macro')
   * \param macroName The name of the macro.
   * \param macroParameters The parameter names of the macro, if any.
   */
  MacroDirective(const LineInterval& lines,
                 const std::vector<std::string>& labels,
                 const std::string& name,
                 const std::string& macroName,
                 const std::vector<std::string>& macroParameters)
  : IntermediateDirective(lines, labels, name)
  , _macroName(macroName)
  , _macroParameters(macroParameters)
  , _operations() {
  }

  /**
   * \brief Executes the given macro (somehow).
   * \param finalRepresentator The FinalRepresentation for possible output.
   * \param table The SymbolTable for possible replacements.
   * \param generator The generator to transform the instructions.
   * \param state The CompileState to log possible errors.
   */
  virtual void execute(FinalRepresentation& finalRepresentator,
                       const SymbolTable& table,
                       const SyntaxTreeGenerator& generator,
                       CompileState& state,
                       MemoryAccess& memoryAccess);


  /**
   * \brief Specifies the new target for operations after this command.
   * \return Set ourselves as target.
   */
  virtual TargetSelector newTarget() const {
    return TargetSelector::THIS;
  }

  /**
   * \brief Inserts an operation into the internal command list.
   * \param pointer The operation to insert.
   */
  virtual void insert(IntermediateOperationPointer pointer) {
    _operations.push_back(std::move(pointer));
  }

  /**
   * \brief Returns the macro name.
   * \return The macro name.
   */
  const std::string& macroName() {
    return _macroName;
  }

  size_t getOperationCount() {
    return _operations.size();
  }

  IntermediateOperationPointer& getOperation(size_t index) {
    return _operations[index];
  }

  template <typename T, typename... U>
  void callOperationFunction(size_t index,
                             const std::vector<std::string>& arguments,
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
  }

 protected:
  class MacroParameters {
   public:
    MacroParameters(std::vector<std::string>::const_iterator begin,
                    std::vector<std::string>::const_iterator end);

    MacroParameters(const std::vector<std::string>& arguments)
    : MacroParameters(arguments.begin(), arguments.end()) {
    }

    void validate(CompileState& state);

    void insertParameters(IntermediateOperationPointer& operation,
                          const std::vector<std::string>& values);

   private:
    std::vector<std::pair<std::string, Optional<std::string>>> _params;
    size_t _minParams;
  };


 private:
  /**
   * \brief The name of the macro.
   */
  std::string _macroName;

  /**
   * \brief The parameters of the macro.
   */
  MacroParameters _macroParameters;

  /**
   * \brief The operation list of the macro.
   */
  std::vector<IntermediateOperationPointer> _operations;
};

#endif /* ERAGPSIM_PARSER_MACRO_DIRECTIVE_HPP */
