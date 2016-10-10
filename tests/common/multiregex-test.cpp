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

#include "common/multiregex.hpp"
#include <gtest/gtest.h>

TEST(Multiregex, simple) {
  // Only test if matches.
  MSRegex mr("^", "$", {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"});
  ASSERT_TRUE(mr.search("1"));
  ASSERT_TRUE(mr.search("5"));
  ASSERT_TRUE(mr.search("8"));
  ASSERT_TRUE(mr.search("9"));
  ASSERT_FALSE(mr.search("12"));
  ASSERT_FALSE(mr.search("1337"));
  ASSERT_FALSE(mr.search("a"));
}

TEST(Multiregex, matching) {
  // Also evaluate the matches.
  MSRegex mr("^", "$", {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"});
  MSMatch match;
  ASSERT_TRUE(mr.search("0", match));
  ASSERT_EQ(match.source, "0");
  ASSERT_EQ(match.choice, 0);
  ASSERT_TRUE(mr.search("5", match));
  ASSERT_EQ(match.source, "5");
  ASSERT_EQ(match.choice, 5);
  ASSERT_TRUE(mr.search("9", match));
  ASSERT_EQ(match.source, "9");
  ASSERT_EQ(match.choice, 9);
}

TEST(Multiregex, brackets) {
  // Test with brackets.
  MSRegex mr("((^))", "(($))", {"(\\d+)", "(a|b)(c|d)", "z"});
  MSMatch match;
  ASSERT_TRUE(mr.search("12", match));
  ASSERT_EQ(match.source, "12");
  ASSERT_EQ(match.choice, 0);
  ASSERT_TRUE(mr.search("ad", match));
  ASSERT_EQ(match.source, "ad");
  ASSERT_EQ(match.choice, 1);
  ASSERT_TRUE(mr.search("z", match));
  ASSERT_EQ(match.source, "z");
  ASSERT_EQ(match.choice, 2);
}
