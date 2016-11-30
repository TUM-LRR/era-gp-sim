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

#include "parser/compile-state.hpp"

#include "parser/macro-directive.hpp"

void CompileState::registerMacro(MacroDirective& macro) {
  bool success = macros.insert(macro);
  if (!success) {
    addError("Macro \"" + macro.macroName() + "\" already exists!");
  }
}

  void CompileState::addError(const std::string& message) {
    addError(message, position);
  }

  void CompileState::addError(const std::string& message, const CodePosition& position) {
    errorList.push_back(
        CompileError(message, position, CompileErrorSeverity::ERROR));
  }

  void CompileState::addWarning(const std::string& message) {
    addWarning(message, position);
  }

  void CompileState::addWarning(const std::string& message, const CodePosition& position) {
    errorList.push_back(
        CompileError(message, position, CompileErrorSeverity::WARNING));
  }

  void CompileState::addInformation(const std::string& message) {
    addInformation(message, position);
  }

  void
  CompileState::addInformation(const std::string& message, const CodePosition& position) {
    errorList.push_back(
        CompileError(message, position, CompileErrorSeverity::INFORMATION));
  }

