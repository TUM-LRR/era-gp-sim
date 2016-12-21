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

#include "parser/positioned-string.hpp"

PositionedString::PositionedString(const std::string& string,
                                   const CodePositionInterval& positionInterval)
: _string(string), _positionInterval(positionInterval) {
}

const std::string& PositionedString::string() const noexcept {
  return _string;
}
const CodePositionInterval& PositionedString::positionInterval() const
    noexcept {
  return _positionInterval;
}
