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

#include <string>

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
   * validation failed. A message, describing the problem, should be
   * given.
   *
   * \param message The message indicating the problem. Should not be empty.
   */
  static const ValidationResult fail(std::string message);

  /**
   * Check if this validation result indicates, that the validation
   * succeeded.
   * \return true if validation succeeded.
   */
  bool isSuccess() const;

  /**
   * Returns the message, that describes the validation result. The string is
   * empty, if the validation succeeded.
   * \return The message of the validation.
   */
  const std::string& getMessage() const;

 private:
  ValidationResult(bool success, std::string message);

  bool _success;
  std::string _message;
};

#endif /* ERAGPSIM_ARCH_COMMON_VALIDATION_RESULT_HPP_ */
