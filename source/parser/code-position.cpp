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

#include "parser/code-position.hpp"
#include "common/assert.hpp"

CodePosition::CodePosition(CodeCoordinate iy, CodeCoordinate ix)
: x(ix), y(iy) {
}

CodeCoordinate CodePosition::line() const noexcept {
  return y;
}

CodeCoordinate CodePosition::row() const noexcept {
  return y;
}

CodeCoordinate CodePosition::character() const noexcept {
  return x;
}

CodeCoordinate CodePosition::column() const noexcept {
  return x;
}

CodePosition CodePosition::operator+(const CodePosition& other) const noexcept {
  return CodePosition(this->y + other.y, this->x + other.x);
}

CodePosition CodePosition::operator-(const CodePosition& other) const {
  assert::that(this->x >= other.x);
  assert::that(this->y >= other.y);
  return CodePosition(this->y - other.y, this->x - other.x);
}

CodePosition CodePosition::operator>>(const CodeCoordinate& other) const
    noexcept {
  return moveRight(other);
}

CodePosition CodePosition::operator<<(const CodeCoordinate& other) const
    noexcept {
  return moveLeft(other);
}

CodePosition CodePosition::moveDown(const CodeCoordinate& c) const noexcept {
  return CodePosition(y + c, x);
}

CodePosition CodePosition::moveUp(const CodeCoordinate& c) const {
  assert::that(y >= c);
  return CodePosition(y - c, x);
}

CodePosition CodePosition::moveLeft(const CodeCoordinate& c) const {
  assert::that(x >= c);
  return CodePosition(y, x - c);
}

CodePosition CodePosition::moveRight(const CodeCoordinate& c) const {
  return CodePosition(y, x + c);
}

CodePosition CodePosition::newLine() const {
  return CodePosition(y + 1, 0);
}
