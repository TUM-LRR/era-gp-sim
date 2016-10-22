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

#include "parser/riscv-parser.hpp"

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "core/memory-access.hpp"
#include "core/project-module.hpp"
#include "gtest/gtest.h"
#include "parser/syntax-information.hpp"

// This is just for internal testing during development
//#define PARSER_TEST_PRINT
#ifdef PARSER_TEST_PRINT
#include <iostream>

static void printIfDefined(std::string str) {
  std::cout << str << '\n';
}

#else

static void printIfDefined(std::string str) {
}

#endif

// Tests will be refined, once arch classes are ready.

class RiscParserTest : public ::testing::Test {
 public:
  ArchitectureFormula formula;
  Architecture arch;
  ProjectModule projectModule;
  MemoryAccess memoryAccess;
  RiscvParser parser;

  RiscParserTest()
  : formula{ArchitectureFormula{"riscv", {"rv32i"}}}
  , arch{Architecture::Brew(formula)}
  , projectModule{formula, 4096, "riscv"}
  , memoryAccess(projectModule.getMemoryAccess())
  , parser{arch, memoryAccess} {
  }
};


TEST_F(RiscParserTest, EmptyString) {
  FinalRepresentation res;
  res = parser.parse("", ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST_F(RiscParserTest, EmptyMultilineString) {
  FinalRepresentation res;
  res = parser.parse("\n\n", ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST_F(RiscParserTest, SingleInstruction) {
  FinalRepresentation res;
  res = parser.parse("ADD x13, x4,7", ParserMode::COMPILE);
  // EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 1);
}

TEST_F(RiscParserTest, MultipleInstructions) {
  FinalRepresentation res;
  res = parser.parse(
      "ADD x13, x4, 7 ;kommentar\n"
      " label  : SUB x5, x5, 1\n"
      "LUI x5, 123;kommentar\n"
      "addition123:\n"
      "\n"
      "ADD x0, x0, 0; kommentar",
      ParserMode::COMPILE);
  // EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 4);
}

TEST_F(RiscParserTest, MalformedInstructions) {
  FinalRepresentation res;
  res = parser.parse("label ADD x13, x4,7\nadd x13 x4 ,7\nble x15 ",
                     ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 3);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST_F(RiscParserTest, BadCharacters) {
  FinalRepresentation res;
  res = parser.parse("ðŸ…±ðŸ…»ðŸ…¾ðŸ†‡ðŸ†‡ x13, x4,7", ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 1);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST_F(RiscParserTest, MixedErrors) {
  FinalRepresentation res;
  res = parser.parse(
      "ADD x13, x4, 7 ;kommentar\n"
      " label  : SUB x5, x5, 1\n"
      ";kommentar\n"
      "sub x2 ;oops missing argument\n"
      "dfklgdjflj\n"
      "addition123:\n"
      "\n"
      "_addition456: ADD x0, x0, 0; kommentar",
      ParserMode::COMPILE);
  // EXPECT_EQ(res.errorList.size(), 2);
  EXPECT_EQ(res.commandList.size(), 3);
}

TEST_F(RiscParserTest, SyntaxInformation) {
  const SyntaxInformation info{parser.getSyntaxInformation()};

  // Loop through registers
  for (std::string syntax :
       info.getSyntaxRegex(SyntaxInformation::Token::Register)) {
    printIfDefined(syntax);
  }
  printIfDefined("");
  // Loop through instructions
  for (std::string syntax :
       info.getSyntaxRegex(SyntaxInformation::Token::Instruction)) {
    printIfDefined(syntax);
  }
  printIfDefined("");
  // Loop through comments
  for (std::string syntax :
       info.getSyntaxRegex(SyntaxInformation::Token::Comment)) {
    printIfDefined(syntax);
  }
  printIfDefined("");
  // Loop through labels
  for (std::string syntax :
       info.getSyntaxRegex(SyntaxInformation::Token::Label)) {
    printIfDefined(syntax);
  }
  printIfDefined("");
  // Loop through immediates
  for (std::string syntax :
       info.getSyntaxRegex(SyntaxInformation::Token::Immediate)) {
    printIfDefined(syntax);
  }
  printIfDefined("");
}
