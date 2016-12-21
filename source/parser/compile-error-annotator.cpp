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
#include "common/translateable.hpp"
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

static void enhanceList(CompileErrorList& list,
                        const char* message,
                        const std::initializer_list<std::string>& parameters,
                        const CodePositionInterval& interval,
                        CompileErrorSeverity severity) {
  auto error = CompileError(
      std::make_shared<Translateable>(message, parameters), interval, severity);
  list.add(error);
}

void CompileErrorAnnotator::addErrorInternal(
    const CodePositionInterval& interval,
    const char* message,
    const std::initializer_list<std::string>& parameters) const {
  enhanceList(
      _list, message, parameters, interval, CompileErrorSeverity::ERROR);
}
void CompileErrorAnnotator::addErrorDeltaInternal(
    const CodePosition& deltaFirst,
    const CodePosition& deltaSecond,
    const char* message,
    const std::initializer_list<std::string>& parameters) const {
  addErrorInternal(CodePositionInterval(_position.start() + deltaFirst,
                                        _position.end() + deltaSecond),
                   message,
                   parameters);
}
void CompileErrorAnnotator::addErrorHereInternal(
    const char* message,
    const std::initializer_list<std::string>& parameters) const {
  addErrorInternal(_position, message, parameters);
}

void CompileErrorAnnotator::addWarningInternal(
    const CodePositionInterval& interval,
    const char* message,
    const std::initializer_list<std::string>& parameters) const {
  enhanceList(
      _list, message, parameters, interval, CompileErrorSeverity::WARNING);
}
void CompileErrorAnnotator::addWarningDeltaInternal(
    const CodePosition& deltaFirst,
    const CodePosition& deltaSecond,
    const char* message,
    const std::initializer_list<std::string>& parameters) const {
  addWarningInternal(CodePositionInterval(_position.start() + deltaFirst,
                                          _position.end() + deltaSecond),
                     message,
                     parameters);
}
void CompileErrorAnnotator::addWarningHereInternal(
    const char* message,
    const std::initializer_list<std::string>& parameters) const {
  addWarningInternal(_position, message, parameters);
}

void CompileErrorAnnotator::addInformationInternal(
    const CodePositionInterval& interval,
    const char* message,
    const std::initializer_list<std::string>& parameters) const {
  enhanceList(
      _list, message, parameters, interval, CompileErrorSeverity::INFORMATION);
}
void CompileErrorAnnotator::addInformationDeltaInternal(
    const CodePosition& deltaFirst,
    const CodePosition& deltaSecond,
    const char* message,
    const std::initializer_list<std::string>& parameters) const {
  addInformationInternal(CodePositionInterval(_position.start() + deltaFirst,
                                              _position.end() + deltaSecond),
                         message,
                         parameters);
}
void CompileErrorAnnotator::addInformationHereInternal(
    const char* message,
    const std::initializer_list<std::string>& parameters) const {
  addInformationInternal(_position, message, parameters);
}

CompileErrorList& CompileErrorAnnotator::errorList() const noexcept {
  return _list;
}
const CodePositionInterval CompileErrorAnnotator::position() const noexcept {
  return _position;
}
