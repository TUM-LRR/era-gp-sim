/* ERASIM Assembler Interpreter
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

#ifndef ERASIM_PARSER_COMMON_CODE_POSITION_HPP
#define ERASIM_PARSER_COMMON_CODE_POSITION_HPP

#include <cstddef>
#include <utility>

/**
 * The coordinate component type for denoting positions in code.
 */
using CodeCoordinate = std::size_t;

/**
 * A position in code.
 */
class CodePosition {
 public:
  /**
   * Creates a new CodePosition with the given values (IMPORTANT: y
   * before x coordinate! Because row is more important than column).
   *
   * \param y The given y position.
   * \param x The given x position.
   */
  explicit CodePosition(CodeCoordinate y = 0, CodeCoordinate x = 0);

  /**
   * \return The y position.
   */
  CodeCoordinate line() const noexcept;

  /**
   * \return The y position.
   */
  CodeCoordinate row() const noexcept;

  /**
   * \return The x position.
   */
  CodeCoordinate character() const noexcept;

  /**
   * \return The x position.
   */
  CodeCoordinate column() const noexcept;

  /**
   * \return The y position.
   */
  CodeCoordinate y() const noexcept;

  /**
   * \return The x position.
   */
  CodeCoordinate x() const noexcept;

  /**
   * Adds a code position to this one.
   *
   * \param other The code position summand.
   * \return The sum of the two code positions.
   */
  CodePosition operator+(const CodePosition& other) const noexcept;

  /**
   * Subtracts one code position from another.
   *
   * \param other The code position denoting the value to subtract.
   * \return The difference between the first and the second code position.
   */
  CodePosition operator-(const CodePosition& other) const;

  /**
   * Moves a code position to the right by the given coordinate.
   *
   * \param other The given coordinate.
   * \return The transformed code position.
   */
  CodePosition operator>>(const CodeCoordinate& other) const noexcept;

  /**
   * Moves a code position to the left by the given coordinate.
   *
   * \param other The given coordinate.
   * \return The transformed code position.
   */
  CodePosition operator<<(const CodeCoordinate& other) const noexcept;

  /**
   * Moves a code position down by the given coordinate.
   *
   * \param c The given coordinate.
   * \return The transformed code position.
   */
  CodePosition moveDown(const CodeCoordinate& c) const noexcept;

  /**
   * Moves a code position up by the given coordinate.
   *
   * \param c The given coordinate.
   * \return The transformed code position.
   */
  CodePosition moveUp(const CodeCoordinate& c) const;

  /**
   * Moves a code position to the left by the given coordinate.
   *
   * \param c The given coordinate.
   * \return The transformed code position.
   */
  CodePosition moveLeft(const CodeCoordinate& c) const;

  /**
   * Moves a code position to the right by the given coordinate.
   *
   * \param c The given coordinate.
   * \return The transformed code position.
   */
  CodePosition moveRight(const CodeCoordinate& c) const;

  /**
   * Increments the line position, but sets the x coordinate to 0 again.
   *
   * \return The transformed code position.
   */
  CodePosition newLine() const;

  /**
   * Creates a code position which takes the maximum of two code
   * positions for each argument.
   *
   * \param c The other code position.
   * \return The maximized code position.
   */
  CodePosition max(const CodePosition& c) const;

  /**
   * Creates a code position which takes the minimum of two code
   * positions for each argument.
   *
   * \param c The other code position.
   * \return The minimized code position.
   */
  CodePosition min(const CodePosition& c) const;

 private:
  /** The x position in code (column/character). */
  CodeCoordinate _x;

  /** The y position in code (row/line). */
  CodeCoordinate _y;
};

#endif
