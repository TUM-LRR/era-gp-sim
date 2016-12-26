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

#ifndef ERAGPSIM_PARSER_POSITIONED_STRING_HPP
#define ERAGPSIM_PARSER_POSITIONED_STRING_HPP

#include <algorithm>
#include <string>
#include <vector>
#include "common/assert.hpp"
#include "parser/common/code-position-interval.hpp"

template <typename CharT>
class PositionedBasicString {
 public:
  using CharType = CharT;
  using String = std::basic_string<CharType>;
  explicit PositionedBasicString(
      const String& string = "",
      const CodePositionInterval& positionInterval = CodePositionInterval())
  : _string(string), _positionInterval(positionInterval) {
  }

  const String& string() const noexcept {
    return _string;
  }
  const CodePositionInterval& positionInterval() const noexcept {
    return _positionInterval;
  }

  PositionedBasicString<CharType>
  slice(std::size_t start, std::size_t length) const {
    assert::that((start + length) <= _string.size());
    assert::that(length <= _string.size());
    assert::that(start < _string.size());
    auto startPosition = nthCharacterPosition(start);
    auto endPosition = nthCharacterPosition(length + start - 1);
    auto newInterval = CodePositionInterval(startPosition, endPosition);
    auto stringSlice = _string.substr(start, length);
    return PositionedBasicString(stringSlice, newInterval);
  }

  CodePosition nthCharacterPosition(std::size_t n) const {
    assert::that(n < _string.size());
    CodePosition position = _positionInterval.start();
    for (std::size_t i = 0; i < n; ++i) {
      if (_string[i] == '\n') {
        position = position.newLine();
      } else {
        position = position >> 1;
      }
    }
    return position;
  }

  bool empty() const {
    return _string.empty();
  }

 private:
  String _string;
  CodePositionInterval _positionInterval;
};

using PositionedString = PositionedBasicString<char>;

using PositionedStringVector = std::vector<PositionedString>;

#endif /* ERAGPSIM_PARSER_POSITIONED_STRING_HPP */
