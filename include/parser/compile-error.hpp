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

#ifndef ERAGPSIM_PARSER_COMPILE_ERROR_HPP_
#define ERAGPSIM_PARSER_COMPILE_ERROR_HPP_

#include <string>

#include "parser/code-position.hpp"

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

/**
 * \brief Represents a mistake the compiler noticed.
 */
class CompileError {
 public:
  /**
   * \brief Instantiates a new compile error with the given arguments. Marks
   * only a positional error.
   * \param message The error message.
   * \param position The position of the error in the code.
   * \param severity The severity of the error.
   */
  CompileError(const std::string& message,
               const CodePosition& position,
               CompileErrorSeverity severity)
  : CompileError(message, position, position >> 1, severity) {
  }

  /**
   * \brief Instantiates a new compile error with the given arguments.
   * \param message The error message.
   * \param startPosition The start position of the error in the code.
   * \param endPosition The end position of the error in the code.
   * \param severity The severity of the error.
   */
  CompileError(const std::string& message,
               const CodePosition& startPosition,
               const CodePosition& endPosition,
               CompileErrorSeverity severity)
  : CompileError(
        message, CodePositionInterval(startPosition, endPosition), severity) {
  }

  /**
   * \brief Instantiates a new compile error with the given arguments.
   * \param message The error message.
   * \param position The position interval of the error in the code.
   * \param severity The severity of the error.
   */
  CompileError(const std::string& message,
               const CodePositionInterval& position,
               CompileErrorSeverity severity)
  : _message(message), _position(position), _severity(severity) {
  }

  /**
   * \brief Returns the message of this error.
   * \return The message of the error.
   */
  const std::string& message() const {
    return _message;
  }

  /**
   * \brief Returns the position where this error occured.
   * \return The position of the error.
   */
  const CodePositionInterval& position() const {
    return _position;
  }

  /**
   * \brief Returns the severity of the error.
   * \return The severity of the error.
   */
  const CompileErrorSeverity severity() const {
    return _severity;
  }

 private:
  /**
   * \brief The internal message attribute.
   */
  std::string _message;

  /**
   * \brief The internal position attribute.
   */
  CodePositionInterval _position;

  /**
   * \brief The internal severity attribute.
   */
  CompileErrorSeverity _severity;
};

#endif