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

#include "parser/integer-parser.hpp"
#include "gtest/gtest.h"


TEST(IntegerParser, Int32Pass) {
  EXPECT_EQ(IntegerParser<int32_t>::parse("  0"), 0);
  EXPECT_EQ(IntegerParser<int32_t>::parse("126"), 126);
  EXPECT_EQ(IntegerParser<int32_t>::parse("084"), 84);
  EXPECT_EQ(IntegerParser<int32_t>::parse("-2147483648"), -2147483648);
  EXPECT_EQ(IntegerParser<int32_t>::parse("9645abc"), 9645);
  EXPECT_EQ(IntegerParser<int32_t>::parse("2147483647"), 2147483647);
}

TEST(IntegerParser, Int32Fail) {
  ASSERT_THROW(IntegerParser<int32_t>::parse("x10"), std::invalid_argument);
  ASSERT_THROW(IntegerParser<int32_t>::parse("-2147483649"), std::out_of_range);
  ASSERT_THROW(IntegerParser<int32_t>::parse("dfghfae"), std::invalid_argument);
  ASSERT_THROW(IntegerParser<int32_t>::parse("2147483648"), std::out_of_range);
}

TEST(IntegerParser, Int16Pass) {
  EXPECT_EQ(IntegerParser<int16_t>::parse("  0"), 0);
  EXPECT_EQ(IntegerParser<int16_t>::parse("126"), 126);
  EXPECT_EQ(IntegerParser<int16_t>::parse("084"), 84);
  EXPECT_EQ(IntegerParser<int16_t>::parse("-32767"), -32767);
  EXPECT_EQ(IntegerParser<int16_t>::parse("9645abc"), 9645);
  EXPECT_EQ(IntegerParser<int16_t>::parse("32767"), 32767);
}

TEST(IntegerParser, Int16Fail) {
  ASSERT_THROW(IntegerParser<int16_t>::parse("x10"), std::invalid_argument);
  ASSERT_THROW(IntegerParser<int16_t>::parse("-32769"), std::out_of_range);
  ASSERT_THROW(IntegerParser<int16_t>::parse("dfghfae"), std::invalid_argument);
  ASSERT_THROW(IntegerParser<int16_t>::parse("32768"), std::out_of_range);
}

TEST(IntegerParser, UInt16Pass) {
  EXPECT_EQ(IntegerParser<uint16_t>::parse("  0"), 0);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("126"), 126);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("084"), 84);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("32767"), 32767);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("9645abc"), 9645);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("65535"), 65535);
}

TEST(IntegerParser, UInt16Fail) {
  ASSERT_THROW(IntegerParser<uint16_t>::parse("x10"), std::invalid_argument);
  ASSERT_THROW(IntegerParser<uint16_t>::parse("-65539"), std::out_of_range);
  ASSERT_THROW(IntegerParser<uint16_t>::parse("dfghfae"), std::invalid_argument);
  ASSERT_THROW(IntegerParser<uint16_t>::parse("65536"), std::out_of_range);
}

TEST(IntegerParser, UInt32Pass) {
  EXPECT_EQ(IntegerParser<uint32_t>::parse("  0"), 0);
  EXPECT_EQ(IntegerParser<uint32_t>::parse("126"), 126);
  EXPECT_EQ(IntegerParser<uint32_t>::parse("084"), 84);
  EXPECT_EQ(IntegerParser<uint32_t>::parse("32767"), 32767);
  EXPECT_EQ(IntegerParser<uint32_t>::parse("9645abc"), 9645);
  EXPECT_EQ(IntegerParser<uint32_t>::parse("65535"), 65535);
}
