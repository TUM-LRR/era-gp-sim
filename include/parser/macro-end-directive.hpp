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

#ifndef ERAGPSIM_PARSER_MACRO_END_DIRECTIVE_HPP
#define ERAGPSIM_PARSER_MACRO_END_DIRECTIVE_HPP

#include "parser/intermediate-directive.hpp"

class MacroEndDirective : public IntermediateDirective {
 public:
  /**
  * \brief Instantiates a new MacroEndDirective with the given arguments.
  * \param lines The line interval the operation occupies.
  * \param labels The vector of labels assigned to the operation.
  * \param name The name of the operation.
  */
  MacroEndDirective(const LineInterval& lines,
                    const std::vector<std::string>& labels,
                    const std::string& name)
  : IntermediateDirective(lines, labels, name) {
  }

  /**
   * \brief Does probably nothing.
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
   * \brief Specifies if the this operation should be processed. In this case:
   * never!
   * \return Always false.
   */
  virtual bool shouldInsert() const {
    return false;
  }

  /**
   * \brief Specifies the new target for operations after this command.
   * \return Switch back to the main target.
   */
  virtual TargetSelector newTarget() const {
    return TargetSelector::MAIN;
  }
};

#endif /* ERAGPSIM_PARSER_MACRO_END_DIRECTIVE_HPP */
