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

#ifndef ERAGPSIM_ARCH_COMMON_VALIDATION_RESULT_HPP_
#define ERAGPSIM_ARCH_COMMON_VALIDATION_RESULT_HPP_

#include <initializer_list>
#include <string>
#include <vector>
#include "common/translateable.hpp"

/**
 * Represents the result of a validatate operation on the syntax tree.
 */
class ValidationResult {
 public:
  /**
   * Creates a new ValidationResult object that indicates, that the
   * validation succeded.
   */
  static const ValidationResult success();

  /**
   * Creates a new ValidationResult object that indicates, that the
   * validation failed. A message describing the problem, must be given.
   * For translation purposes, a variable amount of arguments for the message
   * can be given, to support translatable messages with arguments.
   *
   * \param message The message indicating the problem. Must not be empty.
   * \param arguments A list of arguments for translation purposes
   */
  template <typename... Args>
  static const ValidationResult
  fail(const char* message, Args&&... arguments) {
    return ValidationResult{false, message, arguments...};
  }

  /**
   * Check if this validation result indicates, that the validation
   * succeeded.
   * \return true if validation succeeded.
   */
  bool isSuccess() const;

  /**
   * Overwritten bool operator, that does the same thing as isSuccess().
   */
  explicit operator bool() const;

  /**
   * Returns the message, that describes the validation result.
   * \return The message of the validation.
   */
  const Translateable& getMessage() const;

 private:
  ValidationResult() = default;

  template<typename... Args>
  ValidationResult(bool success,
                   const Args&... args) : _success(success), _message(args...) {}

  bool _success;
  Translateable _message;
};

#endif /* ERAGPSIM_ARCH_COMMON_VALIDATION_RESULT_HPP_ */
