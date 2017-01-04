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

#include "parser/common/compile-error-list.hpp"

#include <algorithm>
#include <cstddef>

#include "common/translateable.hpp"
#include "common/utility.hpp"

using size_t = std::size_t;

// Getter.

const CompileErrorVector& CompileErrorList::errors() const noexcept {
  return _errors;
}

// Some internal helper methods.
namespace {
bool existsError(const CompileErrorVector& errors,
                 CompileErrorSeverity severity) noexcept {
  return Utility::anyOf(errors, [severity](const CompileError& error) {
    return error.severity() == severity;
  });
}

size_t countError(const CompileErrorVector& errors,
                  CompileErrorSeverity severity) noexcept {
  return std::count_if(
      errors.begin(), errors.end(), [severity](const CompileError& error) {
        return error.severity() == severity;
      });
}
}  // namespace

// Some meta info methods about the compile error vector.
bool CompileErrorList::hasErrors() const noexcept {
  return existsError(_errors, CompileErrorSeverity::ERROR);
}
bool CompileErrorList::hasWarnings() const noexcept {
  return existsError(_errors, CompileErrorSeverity::WARNING);
}
bool CompileErrorList::hasInformation() const noexcept {
  return existsError(_errors, CompileErrorSeverity::INFORMATION);
}
bool CompileErrorList::empty() const noexcept {
  return _errors.empty();
}
size_t CompileErrorList::errorCount() const noexcept {
  return countError(_errors, CompileErrorSeverity::ERROR);
}
size_t CompileErrorList::warningCount() const noexcept {
  return countError(_errors, CompileErrorSeverity::WARNING);
}
size_t CompileErrorList::informationCount() const noexcept {
  return countError(_errors, CompileErrorSeverity::INFORMATION);
}
size_t CompileErrorList::size() const noexcept {
  return _errors.size();
}

// Enhancement methods.

void CompileErrorList::addRaw(const CompileError& error) {
  _errors.emplace_back(error);
}
