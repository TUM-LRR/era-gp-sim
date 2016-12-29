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

#ifndef ERAGPSIM_PARSER_COMMON_CODE_POSITION_INTERVAL_HPP
#define ERAGPSIM_PARSER_COMMON_CODE_POSITION_INTERVAL_HPP

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

  /**
   * \brief The start code position of the interval.
   * \return The start code position of the interval.
   */
  const CodePosition& start() const noexcept;

  /**
   * \brief The end code position of the interval.
   * \return The end code position of the interval.
   */
  const CodePosition& end() const noexcept;

  /**
   * \brief The start line of the interval.
   * \return The start line of the interval.
   */
  CodeCoordinate startLine() const noexcept;

  /**
   * \brief The start character/row index of the interval.
   * \return The start character/row index of the interval.
   */
  CodeCoordinate startCharacter() const noexcept;

  /**
   * \brief The last line of the interval.
   * \return The last line of the interval.
   */
  CodeCoordinate endLine() const noexcept;

  /**
   * \brief The last character/row index of the interval.
   * \return The last character/row index of the interval.
   */
  CodeCoordinate endCharacter() const noexcept;

  /**
   * \brief Determines, if the code position interval is empty.
   * \return True, if either the line index of the last position is smaller than
   * the index of the first line or the start and last line are the same and the
   * x-indices are reversed.
   */
  bool empty() const noexcept;

  /**
   * \brief Builds the smallest interval containing two code position intervals.
   * \param other The code position interval to unite with.
   * \return The resulting interval.
   */
  CodePositionInterval unite(const CodePositionInterval& other) const;

  /**
   * \brief Builds the biggest interval containing an area contained within two
   * code position intervals.
   * \param other The code position interval to cut with.
   * \return The resulting interval.
   */
  CodePositionInterval cut(const CodePositionInterval& other) const;

  /**
   * \brief Builds the smallest interval containg all code position intervals in
   * the given iterator.
   * \param start The iterator start position.
   * \param end The iterator end position.
   * \return The resulting interval.
   */
  static CodePositionInterval
  unite(const std::vector<CodePositionInterval>::const_iterator& start,
        const std::vector<CodePositionInterval>::const_iterator& end);

  /**
   * \brief Builds the biggest interval containing an area contained within all
   * code position intervals in the given iterator.
   * \param start The iterator start position.
   * \param end The iterator end position.
   * \return The resulting interval.
   */
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

#endif /* ERAGPSIM_PARSER_COMMON_CODE_POSITION_INTERVAL_HPP */
