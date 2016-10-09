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
 * but WITHOUT ANY WARRANTY; {

} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "gtest/gtest.h"

#include "parser/float-parser.hpp"

#include <cmath>

TEST(FloatParser, DecimalPass) {
  CompileState state;
  EXPECT_FLOAT_EQ(FloatParser<float>::parse("1.5436", state), 1.5436f);
  EXPECT_DOUBLE_EQ(FloatParser<double>::parse("1.5436", state), 1.5436);

  EXPECT_FLOAT_EQ(FloatParser<float>::parse("-1.5436", state), -1.5436f);
  EXPECT_DOUBLE_EQ(FloatParser<double>::parse("-1.5436", state), -1.5436);

  EXPECT_FLOAT_EQ(FloatParser<float>::parse("abc1.5436e7", state, 3),
                  1.5436e7f);
  EXPECT_DOUBLE_EQ(FloatParser<double>::parse("1.5436e-15", state), 1.5436e-15);

  EXPECT_EQ(state.errorList.size(), 0);
}

TEST(FloatParser, HexPass) {
  CompileState state;
  EXPECT_FLOAT_EQ(FloatParser<float>::parse("0x1.5436", state), 0x1.5436p0f);
  EXPECT_DOUBLE_EQ(FloatParser<double>::parse("0x1.5436", state), 0x1.5436p0);

  EXPECT_FLOAT_EQ(FloatParser<float>::parse("-0x1.d436", state), -0x1.d436p0f);
  EXPECT_DOUBLE_EQ(FloatParser<double>::parse("-0x1.d436", state), -0x1.d436p0);

  EXPECT_FLOAT_EQ(FloatParser<float>::parse("0x1.5436p7", state), 0x1.5436p7f);
  EXPECT_DOUBLE_EQ(FloatParser<double>::parse("0x1.5436p-15", state),
                   0x1.5436p-15);

  EXPECT_EQ(state.errorList.size(), 0);
}

TEST(FloatParser, InfinityAndNan) {
  CompileState state;
  float pinf  = FloatParser<float>::parse("infinity", state);
  double ninf = FloatParser<double>::parse("-infinity", state);
  EXPECT_TRUE(std::isinf(pinf) && pinf > 0);
  EXPECT_TRUE(std::isinf(ninf) && ninf < 0);

  EXPECT_TRUE(std::isnan(FloatParser<float>::parse("NAN", state)));
  EXPECT_TRUE(std::isnan(FloatParser<double>::parse("NAN", state)));

  EXPECT_EQ(state.errorList.size(), 0);
}

TEST(FloatParser, Fail) {
  CompileState state;

  FloatParser<float>::parse("lkjhsdfg", state);
  FloatParser<double>::parse("lkjhsdfg", state);

  EXPECT_EQ(state.errorList.size(), 2);
}
