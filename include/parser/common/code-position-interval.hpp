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

#ifndef ERAGPSIM_PARSER_CODE_POSITION_INTERVAL_HPP
#define ERAGPSIM_PARSER_CODE_POSITION_INTERVAL_HPP

#include <vector>
#include "parser/common/code-position.hpp"

/**
 * \brief Represents an interval of codePositions, denoted by an upper and lower
 * codePosition
 * bound.
 */
class CodePositionInterval {
 public:
  /**
   * \brief Creates a new codePosition interval with a given upper and lower
   * bound.
   * \param start The lower bound of the interval.
   * \param end The upper bound of the interval.
   */
  CodePositionInterval(CodePosition start, CodePosition end);

  /**
   * \brief Creates a new codePosition interval containing only one
   * codePosition.
   * \param codePosition The upper and lower bound of the interval.
   */
  explicit CodePositionInterval(CodePosition codePosition);

  /**
   * \brief Creates an empty interval.
   */
  CodePositionInterval();

  const CodePosition& start() const noexcept;

  const CodePosition& end() const noexcept;

  bool empty() const noexcept;

  CodePositionInterval unite(const CodePositionInterval& other) const;

  CodePositionInterval cut(const CodePositionInterval& other) const;

  static CodePositionInterval
  unite(const std::vector<CodePositionInterval>::const_iterator& start,
        const std::vector<CodePositionInterval>::const_iterator& end);

  static CodePositionInterval
  cut(const std::vector<CodePositionInterval>::const_iterator& start,
      const std::vector<CodePositionInterval>::const_iterator& end);

 private:
  /**
   * \brief The upper codePosition bound of the interval.
   */
  CodePosition _codePositionStart;

  /**
   * \brief The lower codePosition bound of the interval.
   */
  CodePosition _codePositionEnd;
};

#endif /* ERAGPSIM_PARSER_CODE_POSITION_INTERVAL_HPP */
