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

template <typename T>
static void
testPass(T expected, const std::string &str, size_t start = 0, int base = 10) {
  CompileState state;
  EXPECT_EQ(IntegerParser<T>::parse(str, state, start, base), expected)
      << '"' << str << "\" not equal to " << expected;
  EXPECT_EQ(state.errorList.size(), 0) << "Error while parsing \"" << str
                                       << '"';
}

template <typename T>
static void testFail(const std::string &str, size_t start = 0, int base = 10) {
  CompileState state;
  IntegerParser<T>::parse(str, state, start, base);
  EXPECT_EQ(state.errorList.size(), 1) << "No exptected errors while parsing \""
                                       << str << '"';
}

TEST(IntegerParser, IntDecimalPass) {
  testPass<int32_t>(0, "0");
  testPass<int32_t>(126, "126");
  testPass<int32_t>(-84, "-084");
  testPass<int32_t>(9645, "xx9645", 2);

  testPass<int32_t>(-2147483648, "-2147483648");
  testPass<int32_t>(2147483647, "2147483647");

  testPass<int16_t>(-32768, "-32768");
  testPass<int16_t>(32767, "32767");
}

TEST(IntegerParser, IntDecimalFail) {
  testFail<int32_t>("10x");
  testFail<int32_t>("dfghfae", 3);

  testFail<int32_t>("-2147483649");
  testFail<int32_t>("2147483648");

  testFail<int16_t>("-32769");
  testFail<int16_t>("32768");
}

TEST(IntegerParser, UIntDecimalPass) {
  testPass<uint16_t>(0, "0");
  testPass<uint16_t>(126, "126");
  testPass<uint16_t>(84, "084");
  testPass<uint16_t>(32767, "32767");
  testPass<uint16_t>(9645, "9645");
  testPass<uint16_t>(65535, "65535");
  testPass<uint16_t>(1, "-65535");
  testPass<uint32_t>(4294967295, "4294967295");
  testPass<uint32_t>(1, "-4294967295");
}

TEST(IntegerParser, UIntDecimalFail) {
  testFail<uint16_t>("x10");
  testFail<uint16_t>("100t");
  testFail<uint16_t>("dfghfae");

  testFail<uint16_t>("-65539");
  testFail<uint16_t>("65536");

  testFail<uint32_t>("4294967296");
  testFail<uint32_t>("-4294967296");
}

TEST(IntegerParser, Bases) {
  testPass<int32_t>(0xab345, "ab345", 0, 16);
  testPass<int32_t>(0xab345, "0xab345", 0, 16);
  testPass<int32_t>(0xab345, "0xab345", 0, 0);
  testPass<int32_t>(-0xab345, "-0xab345", 0, 0);
  testPass<int32_t>(0xab345, "0ab345", 0, 16);

  testPass<int32_t>(071345, "71345", 0, 8);
  testPass<int32_t>(071345, "071345", 0, 8);
  testPass<int32_t>(071345, "071345", 0, 0);
  testPass<int32_t>(-071345, "-071345", 0, 0);

  testPass<int32_t>(9, "1001", 0, 2);
  testPass<int32_t>(-9, "-1001", 0, 2);
  testPass<int32_t>(9, "01001", 0, 2);
  testFail<int32_t>("0x1001", 0, 2);

  testPass<int32_t>(24356, "30HG", 0, 20);
  testPass<int32_t>(-24356, "-30HG", 0, 20);

  testPass<int32_t>(42863, "0x2N", 0, 36);
  testPass<int32_t>(-42863, "-0x2N", 0, 36);


  testPass<uint16_t>(0xab45, "ab45", 0, 16);
  testPass<uint16_t>(0xab45, "0xab45", 0, 16);
  testPass<uint16_t>(0xab45, "0xab45", 0, 0);
  testPass<uint16_t>(21691, "-0xab45", 0, 0);
  testPass<uint16_t>(0xab45, "0ab45", 0, 16);

  testPass<int16_t>(07145, "7145", 0, 8);
  testPass<int16_t>(07145, "07145", 0, 8);
  testPass<int16_t>(07145, "07145", 0, 0);
  testPass<int16_t>(-07145, "-07145", 0, 0);

  testPass<int16_t>(9, "1001", 0, 2);
  testPass<int16_t>(-9, "-1001", 0, 2);
  testPass<int16_t>(9, "01001", 0, 2);
  testFail<int16_t>("0x1001", 0, 2);

  testPass<int16_t>(24356, "30HG", 0, 20);
  testPass<int16_t>(-24356, "-30HG", 0, 20);

  testPass<int16_t>(1190, "0x2", 0, 36);
  testPass<int16_t>(-1190, "-0x2", 0, 36);
}
