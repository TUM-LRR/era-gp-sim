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

#ifndef ERAGPSIM_PARSER_COMPILE_STATE_HPP
#define ERAGPSIM_PARSER_COMPILE_STATE_HPP

#include <vector>

#include "parser/code-position.hpp"
#include "parser/compile-error.hpp"
#include "parser/parser-mode.hpp"

// Note: this class should be reworked and rebuilt to a full-grown class...

/**
 \brief Contains all relevant information for a compile run.
 */
struct CompileState {
  /**
   \brief Denotes the mode of the compiler.
   */
  ParserMode mode;

  /**
   \brief Equals the current position in code. TODO: Implement functionality.
   */
  CodePosition position;

  /**
   \brief Contains a list of compiler errors of this pass.
   */
  std::vector<CompileError> errorList;

  /**
   \brief Specifies the section the compiler is currently in.
   */
  std::string section = "text";

  /**
   * \brief Adds an error to the state-internal error list at the current
   * position.
   * \param message The message for the error.
   */
  void addError(const std::string& message) {
    addError(message, position);
  }

  /**
   * \brief Adds an error to the state-internal error list.
   * \param message The message for the error.
   * \param position The position where the error occurred.
   */
  void addError(const std::string& message, const CodePosition& position) {
    errorList.push_back(
        CompileError(message, position, CompileErrorSeverity::ERROR));
  }

  /**
   * \brief Adds a warning to the state-internal error list at the current
   * position.
   * \param message The message for the warning.
   */
  void addWarning(const std::string& message) {
    addWarning(message, position);
  }

  /**
   * \brief Adds a warning to the state-internal error list.
   * \param message The message for the warning.
   * \param position The position where the warning occurred.
   */
  void addWarning(const std::string& message, const CodePosition& position) {
    errorList.push_back(
        CompileError(message, position, CompileErrorSeverity::WARNING));
  }

  /**
   * \brief Adds an information to the state-internal error list at the current
   * position.
   * \param message The message for the information.
   */
  void addInformation(const std::string& message) {
    addInformation(message, position);
  }

  /**
   * \brief Adds an information to the state-internal error list.
   * \param message The message for the information.
   * \param position The position where the information is needed.
   */
  void
  addInformation(const std::string& message, const CodePosition& position) {
    errorList.push_back(
        CompileError(message, position, CompileErrorSeverity::INFORMATION));
  }
};

#endif
