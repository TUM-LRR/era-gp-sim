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

#ifndef ERAGPSIM_PARSER_COMPILE_ERROR_LIST_HPP
#define ERAGPSIM_PARSER_COMPILE_ERROR_LIST_HPP

#include <functional>
#include <vector>
#include "parser/code-position-interval.hpp"
#include "parser/compile-error-severity.hpp"
#include "parser/compile-error.hpp"

class CompileErrorList {
 public:
  CompileErrorList() = default;

  const CompileErrorVector errors() const noexcept;
  bool hasErrors() const;
  bool hasWarnings() const;
  bool hasInformation() const;
  bool empty() const;
  std::size_t errorCount() const;
  std::size_t warningCount() const;
  std::size_t informationCount() const;
  std::size_t size() const;
  void add(const CompileError& error);
 private:
  CompileErrorVector _errors;
};

#endif /* ERAGPSIM_PARSER_COMPILE_ERROR_LIST_HPP */
