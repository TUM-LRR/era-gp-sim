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

#ifndef ERAGPSIM_PARSER_LINE_INTERVAL_HPP
#define ERAGPSIM_PARSER_LINE_INTERVAL_HPP

#include "parser/code-position-interval.hpp"

/**
 * \brief Represents an interval of lines, denoted by an upper and lower line
 * bound.
 */
class LineInterval {
 public:
  /**
   * \brief Creates a new line interval with a given upper and lower bound.
   * \param start The lower bound of the interval.
   * \param end The upper bound of the interval.
   */
  LineInterval(CodeCoordinate start, CodeCoordinate end);

  /**
   * \brief Creates a new line interval containing only one line.
   * \param line The upper and lower bound of the interval.
   */
  explicit LineInterval(CodeCoordinate line);

  /**
   * \brief Creates a line interval containing only line 0.
   */
  LineInterval();

  const CodeCoordinate& lineStart() const noexcept;

  const CodeCoordinate& lineEnd() const noexcept;

 private:
  /**
   * \brief The upper line bound of the interval.
   */
  CodeCoordinate _lineStart;

  /**
   * \brief The lower line bound of the interval.
   */
  CodeCoordinate _lineEnd;
};

#endif /* ERAGPSIM_PARSER_LINE_INTERVAL_HPP */
