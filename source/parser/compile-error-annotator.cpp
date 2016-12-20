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

#include "parser/compile-error-annotator.hpp"
#include "parser/code-position-interval.hpp"
#include "parser/compile-error-list.hpp"
#include "parser/compile-error.hpp"

CompileErrorAnnotator::CompileErrorAnnotator(
    CompileErrorList& list, const CodePositionInterval& position)
: _list(list), _position(position) {
}
CompileErrorAnnotator::CompileErrorAnnotator(
    const CompileErrorAnnotator& source, const CodePositionInterval& position)
: CompileErrorAnnotator(source.errorList(), position) {
}
CompileErrorAnnotator::CompileErrorAnnotator(CompileErrorList& list,
                                             const CodePosition& start,
                                             const CodePosition& end)
: CompileErrorAnnotator(list, CodePositionInterval(start, end)) {
}
CompileErrorAnnotator::CompileErrorAnnotator(
    const CompileErrorAnnotator& source,
    const CodePosition& start,
    const CodePosition& end)
: CompileErrorAnnotator(source.errorList(), start, end) {
}

void CompileErrorAnnotator::add(const CompileError& error) const {
  _list.add(error);
}
void CompileErrorAnnotator::add(const std::string& message,
                                const CodePosition& deltaFirst,
                                const CodePosition& deltaSecond,
                                CompileErrorSeverity severity) const {
  _list.add(message,
            CodePositionInterval(_position.codePositionStart() + deltaFirst,
                                 _position.codePositionEnd() + deltaSecond),
            severity);
}
void CompileErrorAnnotator::add(const std::string& message,
                                const CodePositionInterval& interval,
                                CompileErrorSeverity severity) const {
  _list.add(message, interval, severity);
}
void CompileErrorAnnotator::add(const std::string& message,
                                CompileErrorSeverity severity) const {
  _list.add(message, _position, severity);
}

CompileErrorList& CompileErrorAnnotator::errorList() const noexcept {
  return _list;
}
const CodePositionInterval CompileErrorAnnotator::position() const noexcept {
  return _position;
}
