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

#ifndef ERAGPSIM_PARSER_CODE_POSITION_HPP
#define ERAGPSIM_PARSER_CODE_POSITION_HPP

#include <utility>

/**
 * \brief The coordinate component type for denoting positions in code.
 */
using CodeCoordinate = std::size_t;

/**
 * \brief A position in code.
 */
struct CodePosition {
  /**
   * \brief Creates a new CodePosition with the given values (IMPORTANT: y
   * before x coordinate! Because row is more important than column).
   * \param iy The given y position.
   * \param ix The given x position.
   */
  explicit CodePosition(CodeCoordinate iy = 0, CodeCoordinate ix = 0);

  /**
   * \brief Alias for the y position.
   * \return The y position.
   */
  CodeCoordinate line() const noexcept;

  /**
   * \brief Alias for the y position.
   * \return The y position.
   */
  CodeCoordinate row() const noexcept;

  /**
   * \brief Alias for the x position.
   * \return The x position.
   */
  CodeCoordinate character() const noexcept;

  /**
   * \brief Alias for the x position.
   * \return The x position.
   */
  CodeCoordinate column() const noexcept;

  /**
   * \brief Adds a code position to this one.
   * \param other The code position summand.
   * \return The sum of the two code positions.
   */
  CodePosition operator+(const CodePosition& other) const noexcept;

  /**
   * \brief Subtracts one code position from another.
   * \param other The code position denoting the value to subtract.
   * \return The difference between the first and the second code position.
   */
  CodePosition operator-(const CodePosition& other) const;

  /**
   * \brief Moves a code position to the right by the given coordinate.
   * \param other The given coordinate.
   * \return The transformed code position.
   */
  CodePosition operator>>(const CodeCoordinate& other) const noexcept;

  /**
   * \brief Moves a code position to the left by the given coordinate.
   * \param other The given coordinate.
   * \return The transformed code position.
   */
  CodePosition operator<<(const CodeCoordinate& other) const noexcept;

  /**
   * \brief Moves a code position down by the given coordinate.
   * \param c The given coordinate.
   * \return The transformed code position.
   */
  CodePosition moveDown(const CodeCoordinate& c) const noexcept;

  /**
   * \brief Moves a code position up by the given coordinate.
   * \param c The given coordinate.
   * \return The transformed code position.
   */
  CodePosition moveUp(const CodeCoordinate& c) const;

  /**
   * \brief Moves a code position to the left by the given coordinate.
   * \param c The given coordinate.
   * \return The transformed code position.
   */
  CodePosition moveLeft(const CodeCoordinate& c) const;

  /**
   * \brief Moves a code position to the right by the given coordinate.
   * \param c The given coordinate.
   * \return The transformed code position.
   */
  CodePosition moveRight(const CodeCoordinate& c) const;

  /**
   * \brief Increments the line position, but sets the x coordinate to 0 again.
   * \return The transformed code position.
   */
  CodePosition newLine() const;

  /**
   * \brief The x position in code (column/character).
   */
  CodeCoordinate x;

  /**
   * \brief The y position in code (row/line).
   */
  CodeCoordinate y;
};

using CodePositionInterval = std::pair<CodePosition, CodePosition>;

#endif
