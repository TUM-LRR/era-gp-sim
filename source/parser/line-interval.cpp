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

#include "parser/line-interval.hpp"

LineInterval::LineInterval(CodeCoordinate start, CodeCoordinate end)
: lineStart(start), lineEnd(end) {
}

/**
 * \brief Creates a new line interval containing only one line.
 * \param line The upper and lower bound of the interval.
 */
LineInterval::LineInterval(CodeCoordinate line) : LineInterval(line, line) {
}

/**
 * \brief Creates a line interval containing only line 0.
 */
LineInterval::LineInterval() : LineInterval(0, 0) {
}
