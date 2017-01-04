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

#include "parser/independent/intermediate-instruction.hpp"
#include "gtest/gtest.h"
#include "parser/common/code-position-interval.hpp"
#include "parser/independent/positioned-string.hpp"

// Hehe...
#define ZP(x) PositionedString(x)
#define SAMPLE_COMMAND                                                \
  CodePositionInterval(), {ZP("label1"), ZP("label2"), ZP("label3")}, \
      ZP("mov"), {ZP("eax")}, {                                       \
    ZP("eax")                                                         \
  }

TEST(IntermediateInstruction, initSimple) {
  // I don't know if this is useful, but it might be, so...
  IntermediateInstruction ii(SAMPLE_COMMAND);
}
