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
  /**
   * \brief Returns the list of macro parameters.
   * \return The list of macro parameters.
   */
  const std::vector<std::string>& macroParameters() {
    return _macroParameters;
  }

  /**
   * \brief Returns the list of operations.
   * \return The list of operations.
   */
  const std::vector<IntermediateOperationPointer>& operations() {
    return _operations;
  }

 private:
  /**
   * \brief The name of the macro.
   */
  std::string _macroName;

  /**
   * \brief The parameters of the macro.
   */
  std::vector<std::string> _macroParameters;

  /**
   * \brief The operation list of the macro.
   */
  std::vector<IntermediateOperationPointer> _operations;
};

#endif /* ERAGPSIM_PARSER_MACRO_DIRECTIVE_HPP */
