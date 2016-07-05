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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.*/

#ifndef ERAGPSIM_PARSER_COMPILE_STATE_HPP_
#define ERAGPSIM_PARSER_COMPILE_STATE_HPP_

#include <vector>
#include "code-position.hpp"
#include "compile-error.hpp"

// To be replaced by the enum in the main parser file, therefore: No comments!
enum class CompileMode { UPDATE, COMPILE };

/**
 \struct CompileState
 \brief Contains all relevant information for a compile run.
 */
struct CompileState {
  /**
   \var CompileState::mode
   \brief Denotes the mode of the compiler.
   */
  CompileMode mode;

  /**
   \var CompileState::position
   \brief Equals the current position in code. TODO: Implement functionality.
   */
  CodePosition position;

  /**
   \var CompileState::errorList
   \brief Contains a list of compiler errors of this pass.
   */
  std::vector<CompileError> errorList;
};

#endif