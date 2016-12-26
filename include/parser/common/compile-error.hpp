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

#ifndef ERAGPSIM_COMPILE_ERROR_HPP
#define ERAGPSIM_COMPILE_ERROR_HPP

#include <QtGlobal>
#include <memory>
#include <string>
#include <vector>

class Translateable;

#include "parser/common/code-position-interval.hpp"
#include "parser/common/compile-error-severity.hpp"

/**
 * \brief Represents a mistake the compiler noticed.
 */
class CompileError {
 public:
  using TranslateablePtr = std::shared_ptr<Translateable>;
  /**
   * \brief Instantiates a new compile error with the given arguments. Marks
   * only a positional error.
   * \param message The error message.
   * \param position The position of the error in the code.
   * \param severity The severity of the error.
   */
  CompileError(const TranslateablePtr& message,
               const CodePosition& position,
               CompileErrorSeverity severity);

  /**
   * \brief Instantiates a new compile error with the given arguments.
   * \param message The error message.
   * \param startPosition The start position of the error in the code.
   * \param endPosition The end position of the error in the code.
   * \param severity The severity of the error.
   */
  CompileError(const TranslateablePtr& message,
               const CodePosition& startPosition,
               const CodePosition& endPosition,
               CompileErrorSeverity severity);

  /**
   * \brief Instantiates a new compile error with the given arguments.
   * \param message The error message.
   * \param position The position interval of the error in the code.
   * \param severity The severity of the error.
   */
  CompileError(const TranslateablePtr& message,
               const CodePositionInterval& position,
               CompileErrorSeverity severity);

  /**
   * \brief Returns the message of this error.
   * \return The message of the error.
   */
  const Translateable& message() const noexcept;

  /**
   * \brief Returns the position where this error occured.
   * \return The position of the error.
   */
  const CodePositionInterval& position() const noexcept;

  /**
   * \brief Returns the severity of the error.
   * \return The severity of the error.
   */
  const CompileErrorSeverity severity() const noexcept;

 private:
  /**
   * \brief The internal message attribute.
   */
  TranslateablePtr _message;

  /**
   * \brief The internal position attribute.
   */
  CodePositionInterval _position;

  /**
   * \brief The internal severity attribute.
   */
  CompileErrorSeverity _severity;
};

/**
 * \brief A vector of compile errors.
 */
using CompileErrorVector = std::vector<CompileError>;

#endif
