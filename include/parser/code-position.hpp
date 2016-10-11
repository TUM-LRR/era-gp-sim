/* C++ Assembler Interpreter
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

#ifndef ERAGPSIM_PARSER_CODE_POSITION_HPP_
#define ERAGPSIM_PARSER_CODE_POSITION_HPP_

#include <utility>
#include "common/assert.hpp"

/**
 * \brief The coordinate component type for denoting positions in code.
 */
using CodeCoordinate = unsigned int;

/**
 * \brief A position in code.
 */
struct CodePosition {
  /**
   * \brief The x position in code (column/character).
   */
  CodeCoordinate x;

  /**
   * \brief The y position in code (row/line).
   */
  CodeCoordinate y;

  /**
   * \brief Creates a new CodePosition at (0,0).
   */
  CodePosition() : CodePosition(0, 0) {
  }

  /**
   * \brief Creates a new CodePosition with the given values (IMPORTANT: y
   * before x coordinate! Because row is more important than column).
   * \param iy The given y position.
   * \param ix The given x position.
   */
  CodePosition(CodeCoordinate iy, CodeCoordinate ix) : x(ix), y(iy) {
  }

  /**
   * \brief Alias for the y position.
   * \return The y position.
   */
  CodeCoordinate line() const {
    return y;
  }

  /**
   * \brief Alias for the y position.
   * \return The y position.
   */
  CodeCoordinate row() const {
    return y;
  }

  /**
   * \brief Alias for the x position.
   * \return The x position.
   */
  CodeCoordinate character() const {
    return x;
  }

  /**
   * \brief Alias for the x position.
   * \return The x position.
   */
  CodeCoordinate column() const {
    return x;
  }

  /**
   * \brief Sums up two code position.
   * \param a The first code position summand.
   * \param b The second code position summand.
   * \return The sum of the two code positions.
   */
  friend CodePosition operator+(const CodePosition& a, const CodePosition& b) {
    return CodePosition(a.y + b.y, a.x + b.x);
  }

  /**
   * \brief Subtracts one code position from another.
   * \param a The first code position to subtract from.
   * \param b The second code position denoting the value to subtract.
   * \return The difference between the first and the second code position.
   */
  friend CodePosition operator-(const CodePosition& a, const CodePosition& b) {
    assert::that(a.x >= b.x);
    assert::that(a.y >= b.y);
    return CodePosition(a.y - b.y, a.x - b.x);
  }

  /**
   * \brief Moves a code position to the right by the given coordinate.
   * \param a The code position to move.
   * \param c The given coordinate.
   * \return The transformed code position.
   */
  friend CodePosition operator>>(const CodePosition& a, CodeCoordinate c) {
    return a.moveRight(c);
  }

  /**
   * \brief Moves a code position to the left by the given coordinate.
   * \param a The code position to move.
   * \param c The given coordinate.
   * \return The transformed code position.
   */
  friend CodePosition operator<<(const CodePosition& a, CodeCoordinate c) {
    return a.moveLeft(c);
  }

  /**
   * \brief Moves a code position down by the given coordinate.
   * \param c The given coordinate.
   * \return The transformed code position.
   */
  CodePosition moveDown(CodeCoordinate c) const {
    return CodePosition(y + c, x);
  }

  /**
   * \brief Moves a code position up by the given coordinate.
   * \param c The given coordinate.
   * \return The transformed code position.
   */
  CodePosition moveUp(CodeCoordinate c) const {
    assert::that(y >= c);
    return CodePosition(y - c, x);
  }

  /**
   * \brief Moves a code position to the left by the given coordinate.
   * \param c The given coordinate.
   * \return The transformed code position.
   */
  CodePosition moveLeft(CodeCoordinate c) const {
    assert::that(x >= c);
    return CodePosition(y, x - c);
  }

  /**
   * \brief Moves a code position to the right by the given coordinate.
   * \param c The given coordinate.
   * \return The transformed code position.
   */
  CodePosition moveRight(CodeCoordinate c) const {
    return CodePosition(y, x + c);
  }

  /**
   * \brief Increments the line position, but sets the x coordinate to 0 again.
   * \return The transformed code position.
   */
  CodePosition newLine() const {
    return CodePosition(y + 1, 0);
  }
};

using CodePositionInterval = std::pair<CodePosition, CodePosition>;

#endif