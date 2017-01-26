/*
* ERASIM Assembler Interpreter
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

#ifndef ERASIM_PARSER_INDEPENDENT_POSITIONED_STRING_HPP
#define ERASIM_PARSER_INDEPENDENT_POSITIONED_STRING_HPP

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

#include "common/assert.hpp"
#include "common/utility.hpp"
#include "parser/common/code-position-interval.hpp"

/**
 * A string enhanced with a position interval which denotes where it is
 * located in code.
 *
 * \tparam CharT The char type on which the string is based on.
 */
template <typename CharT>
class PositionedBasicString {
 public:
  /**
   * The same character type as above, just ensured to be numeric.
   */
  using CharType = Utility::TypeBarrier<CharT, std::is_integral>;

  /**
   * The string type we will use in the following.
   */
  using String = std::basic_string<CharType>;

  /**
   * Alias for std::size_t .
   */
  using size_t = std::size_t;

  /**
   * Creates a new positioned basic string with the given parameters.
   *
   * \param string The string which is represented by this positioned basic
   * string (defaults to an empty string).
   * \param positionInterval The code position interval where the string is
   * located in code (defaults to an empty interval).
   */
  explicit PositionedBasicString(
      const String& string = "",
      const CodePositionInterval& positionInterval = CodePositionInterval())
  : _string(string), _positionInterval(positionInterval) {
  }

  /**
   * Returns the string which is represented by this positioned basic
   * string.
   *
   * \return The string which is represented by this positioned basic string
   */
  const String& string() const noexcept {
    return _string;
  }

  /**
   * Returns the code position interval where the string is located in
   * code (defaults to an empty interval).
   *
   * \return The code position interval where the string is located in code
   * (defaults to an empty interval).
   */
  const CodePositionInterval& positionInterval() const noexcept {
    return _positionInterval;
  }

  /**
   * Gets a part of the string, with reduced position interval in code.
   *
   * \param start The start of the slice.
   * \param length The length of the slice. $$start+length$$ must be at most as
   * large as the whole string.
   * \return The specified slice of this positioned string.
   */
  PositionedBasicString<CharType> slice(size_t start, size_t length) const {
    assert::that((start + length) <= _string.size());
    assert::that(length <= _string.size());
    assert::that(start < _string.size());

    // basically, we get start and end of our part-interval and take the
    // substring of our source string.
    auto startPosition = nthCharacterPosition(start);
    auto endPosition = nthCharacterPosition(length + start - 1);
    auto newInterval = CodePositionInterval(startPosition, endPosition);
    auto stringSlice = _string.substr(start, length);

    return PositionedBasicString(stringSlice, newInterval);
  }

  /**
   * Gets the estimated position of the nth character of this positioned
   * string.
   *
   * \param n The character to get (0-based!).
   * \return The estimated code position where this character could be.
   */
  CodePosition nthCharacterPosition(size_t n) const {
    assert::that(n < _string.size());

    // We begin at the beginning of our interval...
    CodePosition position = _positionInterval.start();
    for (auto i : Utility::range<size_t>(0, n)) {
      // ...then iterate, if we find a '\n' (line feed?), we increment the line,
      // else we go forward by one character.
      if (_string[i] == '\n') {
        position = position.newLine();
      } else {
        position = position >> 1;
      }
    }

    // We do this n times, then we are done.
    return position;
  }

  /**
   * Forwarded method from the string, denotes if it is empty.
   *
   * \return True, if the contained string is empty, else false.
   */
  bool empty() const noexcept {
    return _string.empty();
  }

  /**
   * Forwarded method from the string, denotes if its size.
   *
   * \return The size of the string.
   */
  size_t size() const noexcept {
    return _string.size();
  }

 private:
  /**
   * The string which is represented by this positioned basic string.
   */
  String _string;

  /**
   * The code position interval where the string is located in code
   * (defaults to an empty interval).
   */
  CodePositionInterval _positionInterval;
};

/**
 * A positioned basic string with the default 'char' type.
 */
using PositionedString = PositionedBasicString<char>;

/**
 * A vector of positioned strings.
 */
using PositionedStringVector = std::vector<PositionedString>;

#endif /* ERASIM_PARSER_INDEPENDENT_POSITIONED_STRING_HPP */
