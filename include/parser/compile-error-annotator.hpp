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

#ifndef ERAGPSIM_PARSER_COMPILE_ERROR_ANNOTATOR_HPP
#define ERAGPSIM_PARSER_COMPILE_ERROR_ANNOTATOR_HPP

class CompileError;
class CompileErrorList;
#include <string>
#include "parser/code-position.hpp"
#include "parser/compile-error-severity.hpp"

class CompileErrorAnnotator {
 public:
  CompileErrorAnnotator(CompileErrorList& list,
                        const CodePositionInterval& position);
  CompileErrorAnnotator(CompileErrorAnnotator& source,
                        const CodePositionInterval& position);
  CompileErrorAnnotator(CompileErrorList& list,
                        const CodePosition& start,
                        const CodePosition& end);
  CompileErrorAnnotator(CompileErrorAnnotator& source,
                        const CodePosition& start,
                        const CodePosition& end);

  void add(const CompileError& error);
  void add(const std::string& message,
           const CodePositionInterval& interval,
           CompileErrorSeverity severity = CompileErrorSeverity::ERROR);
  void add(const std::string& message,
           CompileErrorSeverity severity = CompileErrorSeverity::ERROR);

  CompileErrorList& errorList() noexcept;
  const CodePositionInterval position() const noexcept;

 private:
  CompileErrorList& _list;
  CodePositionInterval _position;
};

#endif /* ERAGPSIM_PARSER_COMPILE_ERROR_ANNOTATOR_HPP */
