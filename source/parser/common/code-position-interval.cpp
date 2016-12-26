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

#include "parser/common/code-position-interval.hpp"

CodePositionInterval::CodePositionInterval(CodePosition start, CodePosition end)
: _codePositionStart(start), _codePositionEnd(end) {
}

/**
 * \brief Creates a new codePosition interval containing only one codePosition.
 * \param codePosition The upper and lower bound of the interval.
 */
CodePositionInterval::CodePositionInterval(CodePosition codePosition)
: CodePositionInterval(codePosition, codePosition >> 1) {
}

/**
 * \brief Creates a codePosition interval containing only codePosition 0.
 */
CodePositionInterval::CodePositionInterval()
: CodePositionInterval(CodePosition(1), CodePosition(0)) {
}

const CodePosition& CodePositionInterval::start() const noexcept {
  return _codePositionStart;
}

const CodePosition& CodePositionInterval::end() const noexcept {
  return _codePositionEnd;
}

CodeCoordinate CodePositionInterval::startLine() const noexcept {
  return _codePositionStart.line();
}

CodeCoordinate CodePositionInterval::startCharacter() const noexcept {
  return _codePositionStart.character();
}

CodeCoordinate CodePositionInterval::endLine() const noexcept {
  return _codePositionEnd.line();
}

CodeCoordinate CodePositionInterval::endCharacter() const noexcept {
  return _codePositionEnd.character();
}

bool CodePositionInterval::empty() const noexcept {
  return _codePositionStart.x() > _codePositionEnd.x() ||
         _codePositionStart.y() > _codePositionEnd.y();
}

CodePositionInterval
CodePositionInterval::unite(const CodePositionInterval& other) const {
  if (empty()) {
    return other;
  } else if (other.empty()) {
    return *this;
  } else {
    auto nstart = start().max(other.start());
    auto nend = end().max(other.end());
    return CodePositionInterval(nstart, nend);
  }
}

CodePositionInterval
CodePositionInterval::cut(const CodePositionInterval& other) const {
  if (empty()) {
    return other;
  } else if (other.empty()) {
    return *this;
  } else {
    auto nstart = start().min(other.start());
    auto nend = end().min(other.end());
    return CodePositionInterval(nstart, nend);
  }
}

CodePositionInterval CodePositionInterval::unite(
    const std::vector<CodePositionInterval>::const_iterator& start,
    const std::vector<CodePositionInterval>::const_iterator& end) {
  CodePositionInterval acc;
  for (auto it = start; it != end; ++it) {
    acc = acc.unite(*it);
  }
  return acc;
}

CodePositionInterval CodePositionInterval::cut(
    const std::vector<CodePositionInterval>::const_iterator& start,
    const std::vector<CodePositionInterval>::const_iterator& end) {
  CodePositionInterval acc;
  for (auto it = start; it != end; ++it) {
    acc = acc.cut(*it);
  }
  return acc;
}
