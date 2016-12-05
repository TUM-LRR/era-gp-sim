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
   * validation failed. A message, describing the problem, must be given.
   * This method is equivalent to: ValidationResult::fail(message, {})
   *
   * \param message The message indicating the problem. Must not be empty.
   */
  static const ValidationResult fail(const std::string& message);

  /**
   * Creates a new ValidationResult object that indicates, that the
   * validation failed. A message describing the problem, must be given.
   * For translation purposes, a list of arguments for the message can
   * be given, to support translatable messages with arguments.
   *
   * \param message The message indicating the problem. Must not be empty.
   * \param arguments A list of arguments for translation purposes
   */
  static const ValidationResult
  fail(const std::string& message,
       std::initializer_list<std::string> arguments);

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
  fail(const std::string& message, Args&&... arguments) {
    ValidationResult result{false, message, {}};
    result.addArguments(std::forward<Args>(arguments)...);
    return result;
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
  const std::string& getMessage() const;

  /**
   * Get the list of arguments, that exist for translation purposes.
   * \param A const list of arguments
   */
  const std::vector<std::string>& getArguments() const;

 private:
  ValidationResult(bool success,
                   const std::string& message,
                   std::initializer_list<std::string> arguments);

  /* Function to add a single argument to the argument list */
  void addArguments(const std::string& argument);
  /* Function to add multiple arguments to the argument list */
  template <typename... Args>
  void addArguments(const std::string& firstArgument, Args&&... arguments) {
    _arguments.push_back(firstArgument);
    insert(std::forward<Args>(arguments)...);
  }

  bool _success;
  std::string _message;
  std::vector<std::string> _arguments;
};

#endif /* ERAGPSIM_ARCH_COMMON_VALIDATION_RESULT_HPP_ */
