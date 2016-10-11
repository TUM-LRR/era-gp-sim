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

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "common/utility.hpp"

TEST(UtilityTests, TestFileIO) {
  static const std::string testString = R"(
    Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
  )";

  Utility::storeToFile("file", testString);
  EXPECT_EQ(Utility::loadFromFile("file"), testString);

  std::remove("file");
}

TEST(UtilityTest, TestTransformInPlace) {
  std::vector<int> v = {1, 2, 3, 4, 5};
  Utility::transformInPlace(v, [](auto& i) { return i + 1; });
  for (int i = 0; i < v.size(); ++i) {
    EXPECT_EQ(v[i], i + 2);
  }
}

TEST(UtilityTest, TestTransformInto) {
  std::vector<int> v = {1, 2, 3, 4, 5};
  std::vector<int> w(5);
  Utility::transformInto(v, w.begin(), [](auto& i) { return i + 1; });
  for (int i = 0; i < v.size(); ++i) {
    EXPECT_EQ(w[i], i + 2);
  }
}

TEST(UtilityTest, TestTransform) {
  std::vector<int> v = {1, 2, 3, 4, 5};
  auto w             = Utility::transform(v, [](auto& i) { return i + 1; });
  for (int i = 0; i < v.size(); ++i) {
    EXPECT_EQ(w[i], i + 2);
  }
}

TEST(UtilityTest, TestStringTransforms) {
  EXPECT_EQ(Utility::toLower("FOOBAR"), "foobar");
  EXPECT_EQ(Utility::toUpper("foobar"), "FOOBAR");
  EXPECT_EQ(Utility::toLower(Utility::toUpper("foobar")), "foobar");
}

TEST(UtilityTest, TestOther) {
  EXPECT_TRUE(Utility::contains(std::vector<int>{1, 2, 3}, 1));
  EXPECT_FALSE(Utility::contains(std::vector<int>{1, 2, 3}, 5));
}
