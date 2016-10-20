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

#include "gtest/gtest.h"

#include "parser/syntax-information.hpp"

TEST(SyntaxInformation, EmptyObject) {
  SyntaxInformation info;

  // Loops shouldn't execute
  for (auto regex : info.getSyntaxRegex(SyntaxInformation::Token::Register)) {
    FAIL();
  }
  for (auto regex :
       info.getSyntaxRegex(SyntaxInformation::Token::Instruction)) {
    FAIL();
  }
  for (auto regex : info.getSyntaxRegex(SyntaxInformation::Token::Comment)) {
    FAIL();
  }
  for (auto regex : info.getSyntaxRegex(SyntaxInformation::Token::Label)) {
    FAIL();
  }
  for (auto regex : info.getSyntaxRegex(SyntaxInformation::Token::Immediate)) {
    FAIL();
  }
  for (auto regex : info.getEntries()) {
    FAIL();
  }
}

TEST(SyntaxInformation, FilledObject) {
  SyntaxInformation info;
  int i;

  // Add 1 Register, 2 Instructions, 3 Comments, 4 Labels, 5 Immediates
  info.addSyntaxRegex("aaa", SyntaxInformation::Token::Register);
  info.addSyntaxRegex("aab", SyntaxInformation::Token::Instruction);
  info.addSyntaxRegex("aac", SyntaxInformation::Token::Comment);
  info.addSyntaxRegex("aba", SyntaxInformation::Token::Comment);
  info.addSyntaxRegex("abb", SyntaxInformation::Token::Immediate);
  info.addSyntaxRegex("abc", SyntaxInformation::Token::Comment);
  info.addSyntaxRegex("aca", SyntaxInformation::Token::Label);
  info.addSyntaxRegex("acb", SyntaxInformation::Token::Immediate);
  info.addSyntaxRegex("acc", SyntaxInformation::Token::Immediate);
  info.addSyntaxRegex("baa", SyntaxInformation::Token::Label);
  info.addSyntaxRegex("bab", SyntaxInformation::Token::Instruction);
  info.addSyntaxRegex("bac", SyntaxInformation::Token::Label);
  info.addSyntaxRegex("bba", SyntaxInformation::Token::Immediate);
  info.addSyntaxRegex("bbb", SyntaxInformation::Token::Label);
  info.addSyntaxRegex("bbc", SyntaxInformation::Token::Immediate);

  i = 0;
  for (auto regex : info.getSyntaxRegex(SyntaxInformation::Token::Register))
    i++;
  ASSERT_EQ(i, 1);

  i = 0;
  for (auto regex : info.getSyntaxRegex(SyntaxInformation::Token::Instruction))
    i++;
  ASSERT_EQ(i, 2);

  i = 0;
  for (auto regex : info.getSyntaxRegex(SyntaxInformation::Token::Comment)) i++;
  ASSERT_EQ(i, 3);

  i = 0;
  for (auto regex : info.getSyntaxRegex(SyntaxInformation::Token::Label)) i++;
  ASSERT_EQ(i, 4);

  i = 0;
  for (auto regex : info.getSyntaxRegex(SyntaxInformation::Token::Immediate))
    i++;
  ASSERT_EQ(i, 5);

  i = 0;
  for (auto regex : info.getEntries()) i++;
  ASSERT_EQ(i, 1 + 2 + 3 + 4 + 5);
}
