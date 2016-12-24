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

#include <string>
#include <vector>
#include "parser/code-position-interval.hpp"
#include "common/assert.hpp"

template<typename CharT>
class PositionedBasicString {
 public:
  using CharType = CharT;
  using String = std::basic_string<CharType>;
  PositionedString() = default;
  PositionedString(const String& string,
                   const CodePositionInterval& positionInterval)
                   : PositionedString(string, positionInterval, {positionInterval}) {
}
PositionedString(const String& string,
                   const CodePositionInterval& positionInterval,
                   const std::vector<CodePositionInterval>& intervals)
                   : PositionedString(string, positionInterval, fromIntervals(string, intervals)) {
                     assert::that(!intervals.empty());
}

PositionedString(const String& string, const CodePositionInterval& positionInterval, const std::vector<CodePosition>& positions)
: _string(string), _positionInterval(positionInterval), _positions(positions)
{assert::that(positions.size() == string.size());}

  const String& string() const noexcept
  {
    return _string;
  }
  const CodePositionInterval& positionInterval() const noexcept{
    return _positionInterval;
  }
  const std::vector<CodePosition> positions() const noexcept{
    return _positions;
  }

  PositionedBasicString<CharType> slice(std::size_t start, std::size_t length) {
    
  }

 private:
  static CodePosition nextIntervalPosition(const std::vector<CodePositionInterval>& intervals, std::size_t& currentInterval)
  {
      assert::that(intervals.size() > currentInterval);
        while (intervals[currentInterval].empty())
        {
          ++currentInterval;
          assert::that(intervals.size() > currentInterval);
        }
        assert::that(intervals.size() > currentInterval);
        return intervals[currentInterval].start();
  }

  static std::vector<CodePosition> fromIntervals(const std::string& string, const std::vector<CodePositionInterval>& intervals) const
  {
    assert::that(!intervals.empty());
    std::vector<CodePosition> positions;
    std::size_t currentInterval = ~0;
    auto current = CodePosition(1);
    for (std::size_t i = 0; i < string.size(); ++i)
    {
      const auto& compare = currentInterval == ~0 ? CodePosition(0) : intervals[currentInterval];
      if (current.y() > compare.y() || (current.y() == compare.y() && current.x() > compare.x()))
      {
        ++currentInterval;
        current = nextIntervalPosition(intervals, currentInterval);
      }
      positions.push_back(current);
      if (string[i] == '\n')
      {
        current = current.newLine();
      }
      else {
        current = current >> 1;
      }
    }
    return positions;
  }

  String _string;
  CodePositionInterval _positionInterval;
  std::vector<CodePosition> _positions;
};

using PositionedString = PositionedBasicString<char>;

#endif /* ERAGPSIM_PARSER_POSITIONED_STRING_HPP */
