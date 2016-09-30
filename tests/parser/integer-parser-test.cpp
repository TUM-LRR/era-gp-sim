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


TEST(IntegerParser, IntDecimalPass) {
  EXPECT_EQ(IntegerParser<int32_t>::parse("  0"), 0);
  EXPECT_EQ(IntegerParser<int32_t>::parse("126"), 126);
  EXPECT_EQ(IntegerParser<int32_t>::parse("-084"), -84);
  EXPECT_EQ(IntegerParser<int32_t>::parse("xx9645abc", 2), 9645);
  
  EXPECT_EQ(IntegerParser<int32_t>::parse("-2147483648"), -2147483648);
  EXPECT_EQ(IntegerParser<int32_t>::parse("2147483647"), 2147483647);
  
  EXPECT_EQ(IntegerParser<int16_t>::parse("-32768"), -32768);
  EXPECT_EQ(IntegerParser<int16_t>::parse("32767"), 32767);
}

TEST(IntegerParser, IntDecimalFail) {
  ASSERT_THROW(IntegerParser<int32_t>::parse("x10"), std::invalid_argument);
  ASSERT_THROW(IntegerParser<int32_t>::parse("dfghfae", 3), std::invalid_argument);
  
  ASSERT_THROW(IntegerParser<int32_t>::parse("-2147483649"), std::out_of_range);
  ASSERT_THROW(IntegerParser<int32_t>::parse("2147483648"), std::out_of_range);
  
  ASSERT_THROW(IntegerParser<int16_t>::parse("-32769"), std::out_of_range);
  ASSERT_THROW(IntegerParser<int16_t>::parse("32768"), std::out_of_range);
}

TEST(IntegerParser, UIntDecimalPass) {
  EXPECT_EQ(IntegerParser<uint16_t>::parse("  0"), 0);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("126"), 126);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("084"), 84);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("32767"), 32767);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("9645abc"), 9645);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("65535"), 65535);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("-65535"), 1);
  EXPECT_EQ(IntegerParser<uint32_t>::parse("4294967295"), 4294967295);
  EXPECT_EQ(IntegerParser<uint32_t>::parse("-4294967295"), 1);
}

TEST(IntegerParser, UIntDecimalFail) {
  ASSERT_THROW(IntegerParser<uint16_t>::parse("x10"), std::invalid_argument);
  ASSERT_THROW(IntegerParser<uint16_t>::parse("dfghfae"), std::invalid_argument);
  
  ASSERT_THROW(IntegerParser<uint16_t>::parse("-65539"), std::out_of_range);
  ASSERT_THROW(IntegerParser<uint16_t>::parse("65536"), std::out_of_range);
  
  ASSERT_THROW(IntegerParser<uint32_t>::parse("4294967296"), std::out_of_range);
  ASSERT_THROW(IntegerParser<uint32_t>::parse("-4294967296"), std::out_of_range);
}

TEST(IntegerParser, Bases) {
  EXPECT_EQ(IntegerParser<int32_t>::parse("ab345", 0, 16), 0xab345);
  EXPECT_EQ(IntegerParser<int32_t>::parse("0xab345", 0, 16), 0xab345);
  EXPECT_EQ(IntegerParser<int32_t>::parse("0xab345", 0, 0), 0xab345);
  EXPECT_EQ(IntegerParser<int32_t>::parse("-0xab345", 0, 0), -0xab345);
  EXPECT_EQ(IntegerParser<int32_t>::parse("0ab345", 0, 16), 0xab345);
  
  EXPECT_EQ(IntegerParser<int32_t>::parse("71345", 0, 8), 071345);
  EXPECT_EQ(IntegerParser<int32_t>::parse("071345", 0, 8), 071345);
  EXPECT_EQ(IntegerParser<int32_t>::parse("071345", 0, 0), 071345);
  EXPECT_EQ(IntegerParser<int32_t>::parse("-071345", 0, 0), -071345);
  
  EXPECT_EQ(IntegerParser<int32_t>::parse("1001", 0, 2), 9);
  EXPECT_EQ(IntegerParser<int32_t>::parse("-1001", 0, 2), -9);
  EXPECT_EQ(IntegerParser<int32_t>::parse("01001", 0, 2), 9);
  EXPECT_EQ(IntegerParser<int32_t>::parse("0x1001", 0, 2), 0);
  
  EXPECT_EQ(IntegerParser<int32_t>::parse("30HG", 0, 20), 24356);
  EXPECT_EQ(IntegerParser<int32_t>::parse("-30HG", 0, 20), -24356);
  
  EXPECT_EQ(IntegerParser<int32_t>::parse("0x2N", 0, 36), 42863);
  EXPECT_EQ(IntegerParser<int32_t>::parse("-0x2N", 0, 36), -42863);
  
  
  EXPECT_EQ(IntegerParser<uint16_t>::parse("ab45", 0, 16), 0xab45);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("0xab45", 0, 16), 0xab45);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("0xab45", 0, 0), 0xab45);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("-0xab45", 0, 0), 21691);
  EXPECT_EQ(IntegerParser<uint16_t>::parse("0ab45", 0, 16), 0xab45);
                             
  EXPECT_EQ(IntegerParser<int16_t>::parse("7145", 0, 8), 07145);
  EXPECT_EQ(IntegerParser<int16_t>::parse("07145", 0, 8), 07145);
  EXPECT_EQ(IntegerParser<int16_t>::parse("07145", 0, 0), 07145);
  EXPECT_EQ(IntegerParser<int16_t>::parse("-07145", 0, 0), -07145);
                             
  EXPECT_EQ(IntegerParser<int16_t>::parse("1001", 0, 2), 9);
  EXPECT_EQ(IntegerParser<int16_t>::parse("-1001", 0, 2), -9);
  EXPECT_EQ(IntegerParser<int16_t>::parse("01001", 0, 2), 9);
  EXPECT_EQ(IntegerParser<int16_t>::parse("0x1001", 0, 2), 0);
                             
  EXPECT_EQ(IntegerParser<int16_t>::parse("30HG", 0, 20), 24356);
  EXPECT_EQ(IntegerParser<int16_t>::parse("-30HG", 0, 20), -24356);
                             
  EXPECT_EQ(IntegerParser<int16_t>::parse("0x2", 0, 36), 1190);
  EXPECT_EQ(IntegerParser<int16_t>::parse("-0x2", 0, 36), -1190);
}
