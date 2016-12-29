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

#ifndef ERAGPSIM_PARSER_COMMON_COMPILE_ERROR_SEVERITY_HPP
#define ERAGPSIM_PARSER_COMMON_COMPILE_ERROR_SEVERITY_HPP

/**
 * \brief Denotes the severity of a compiler error.
 */
enum class CompileErrorSeverity {
  /**
   * \brief The severity is an error, i.e. the program will not compile
   * properly.
   */
  ERROR,
  /**
   * \brief The severity is a warning, i.e. the program will compile, but might
   * show unexpected behavior.
   */
  WARNING,
  /**
   * \brief The severity is just an information, e.g. a hint for the user.
   */
  INFORMATION
};

#endif /* ERAGPSIM_PARSER_COMMON_COMPILE_ERROR_SEVERITY_HPP */
