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

#include "arch/common/validation-result.hpp"
#include <cassert>

const ValidationResult ValidationResult::success() {
  return ValidationResult{true, "", {}};
}

const ValidationResult ValidationResult::fail(const std::string& message) {
  return fail(message, {});
}

const ValidationResult
ValidationResult::fail(const std::string& message,
                       std::initializer_list<std::string> arguments) {
  assert(message != "");
  return ValidationResult{false, message, arguments};
}

bool ValidationResult::isSuccess() const {
  return _success;
}

ValidationResult::operator bool() const {
  return _success;
}

const std::string& ValidationResult::getMessage() const {
  return _message;
}

const std::vector<std::string>& ValidationResult::getArguments() const {
  return _arguments;
}

ValidationResult::ValidationResult(bool success,
                                   const std::string& message,
                                   std::initializer_list<std::string> arguments)
: _success(success), _message(message), _arguments(arguments) {
}

void ValidationResult::addArguments(const std::string& argument) {
  _arguments.push_back(argument);
}
