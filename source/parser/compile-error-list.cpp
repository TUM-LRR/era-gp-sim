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

#include "parser/compile-error-list.hpp"
#include <algorithm>

const CompileErrorVector CompileErrorList::errors() const noexcept {
  return _errors;
}

static bool existsError(const CompileErrorVector& errors,
                        CompileErrorSeverity severity) {
  return std::any_of(
      errors.begin(), errors.end(), [severity](const CompileError& error) {
        return error.severity() == severity;
      });
}

static std::size_t countError(const CompileErrorVector& errors,
                              CompileErrorSeverity severity) {
  return std::count_if(
      errors.begin(), errors.end(), [severity](const CompileError& error) {
        return error.severity() == severity;
      });
}

bool CompileErrorList::hasErrors() const {
  return existsError(_errors, CompileErrorSeverity::ERROR);
}
bool CompileErrorList::hasWarnings() const {
  return existsError(_errors, CompileErrorSeverity::WARNING);
}
bool CompileErrorList::hasInformation() const {
  return existsError(_errors, CompileErrorSeverity::INFORMATION);
}
bool CompileErrorList::empty() const {
  return _errors.empty();
}
std::size_t CompileErrorList::errorCount() const {
  return countError(_errors, CompileErrorSeverity::ERROR);
}
std::size_t CompileErrorList::warningCount() const {
  return countError(_errors, CompileErrorSeverity::WARNING);
}
std::size_t CompileErrorList::informationCount() const {
  return countError(_errors, CompileErrorSeverity::INFORMATION);
}
std::size_t CompileErrorList::size() const {
  return _errors.size();
}

void CompileErrorList::add(const CompileError& error) {
  _errors.push_back(error);
}
