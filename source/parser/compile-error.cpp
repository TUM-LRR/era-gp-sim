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

#include "parser/compile-error.hpp"
#include "common/assert.hpp"
#include "common/translateable.hpp"

CompileError::CompileError(const CompileError::TranslateablePtr& message,
                           const CodePosition& position,
                           CompileErrorSeverity severity)
: CompileError(message, position, position >> 1, severity) {
}

CompileError::CompileError(const CompileError::TranslateablePtr& message,
                           const CodePosition& startPosition,
                           const CodePosition& endPosition,
                           CompileErrorSeverity severity)
: CompileError(
      message, CodePositionInterval(startPosition, endPosition), severity) {
}

CompileError::CompileError(const CompileError::TranslateablePtr& message,
                           const CodePositionInterval& position,
                           CompileErrorSeverity severity)
: _message(message), _position(position), _severity(severity) {
}

/**
* \brief Returns the message of this error.
* \return The message of the error.
*/
const Translateable& CompileError::message() const noexcept {
  assert::that(_message);
  return (*_message);
}

/**
 * \brief Returns the position where this error occured.
 * \return The position of the error.
 */
const CodePositionInterval& CompileError::position() const noexcept {
  return _position;
}

/**
 * \brief Returns the severity of the error.
 * \return The severity of the error.
 */
const CompileErrorSeverity CompileError::severity() const noexcept {
  return _severity;
}
