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

#ifndef ERAGPSIM_COMPILE_ERROR_HPP_
#define ERAGPSIM_COMPILE_ERROR_HPP_

#include <string>
#include "code-position.hpp"

/**
 * \enum CompileErrorSeverity
 * \brief Denotes the severity of a compiler error.
 */
enum class CompileErrorSeverity {
  /**
   * \var CompileErrorSeverity::ERROR
   * \brief The severity is an error, i.e. the program will not compile
   * properly.
   */
  ERROR,
  /**
   * \var CompileErrorSeverity::WARNING
   * \brief The severity is a warning, i.e. the program will compile, but might
   * show unexpected behavior.
   */
  WARNING,
  /**
   * \var CompileErrorSeverity::INFORMATION
   * \brief The severity is just an information, e.g. a hint for the user.
   */
  INFORMATION
};

/**
 * \class CompileError
 * \brief Represents a mistake the compiler noticed.
 */
class CompileError {
 public:
  /**
   * \fn CompileError::CompileError(const std::string &message, const
   * CodePosition& position, CompileErrorSeverity severity)
   * \brief Instantiates a new compile error with the given arguments.
   * \param message The error message.
   * \param position The position of the error in the code.
   * \param severity The severity of the error.
   */
  CompileError(const std::string& message,
               const CodePosition& position,
               CompileErrorSeverity severity)
  : _message(message), _position(position), _severity(severity) {
  }

  /**
   * \fn CompileError::CompileError(const char* message, const CodePosition&
   * position, CompileErrorSeverity severity)
   * \brief Instantiates a new compile error with the given arguments.
   * \param message The error message.
   * \param position The position of the error in the code.
   * \param severity The severity of the error.
   */
  CompileError(const char* message,
               const CodePosition& position,
               CompileErrorSeverity severity)
  : CompileError(std::string(message), position, severity) {
  }

  /**
   * \fn CompileError::CompileError(const CompileError& other)
   * \brief Default copy constructor.
   * \param other The source CompileError instance.
   */
  CompileError(const CompileError& other) = default;

  /**
   * \fn CompileError::CompileError(CompileError&& other)
   * \brief Default move constructor.
   * \param other The source CompileError instance.
   */
  CompileError(CompileError&& other) = default;

  /**
   * \fn CompileError::operator =(const CompileError& other)
   * \brief Default copy assignment operator.
   * \param other The source CompileError instance.
   */
  CompileError& operator=(const CompileError& other) = default;

  /**
   * \fn CompileError::operator =(CompileError&& other)
   * \brief Default move assignment operator.
   * \param other The source CompileError instance.
   */
  CompileError& operator=(CompileError&& other) = default;

  /**
   * \fn CompileError::~CompileError()
   * \brief Default destructor.
   */
  ~CompileError() = default;

  /**
   * \fn CompileError::message()
   * \brief Returns the message of this error.
   * \return The message of the error.
   */
  const std::string& message() const {
    return _message;
  }

  /**
   * \fn CompileError::position()
   * \brief Returns the position where this error occured.
   * \return The position of the error.
   */
  const CodePosition& position() const {
    return _position;
  }

  /**
   * \fn CompileError::severity()
   * \brief Returns the severity of the error.
   * \return The severity of the error.
   */
  const CompileErrorSeverity severity() const {
    return _severity;
  }

 private:
  /**
   * \var CompileError::_message
   * \brief The internal message attribute.
   */
  std::string _message;

  /**
   * \var CompileError::_position
   * \brief The internal position attribute.
   */
  CodePosition _position;

  /**
   * \var CompileError::_severity
   * \brief The internal severity attribute.
   */
  CompileErrorSeverity _severity;
};

#endif