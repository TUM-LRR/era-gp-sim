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
  res = parser.parse("ADD x13, x4,x0", ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 1);
}

TEST_F(RiscParserTest, SingleDirective) {
  FinalRepresentation res;
  res = parser.parse(".section data", ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST_F(RiscParserTest, SingleBadDirective) {
  FinalRepresentation res;
  res = parser.parse(".idontexist lala, la", ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 1);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST_F(RiscParserTest, MultipleDirectives) {
  FinalRepresentation res;
  res = parser.parse(
      ".equ ZAHL, 10\n"
      ".macro add1, x=0\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\x\n"
      ".endm\n"
      ".macro add2, x=0\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\x\n"
      ".endm\n"
      ".macro add2, x, y\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\y\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\y\n"
      "addi x0, x0, \\x\n"
      ".endm\n"
      ".macro add3\n"
      "add2 3,4\n"
      ".endm\n"
      "add1\n"
      "add1\n"
      "add2 5 + 7 << 2\n"
      "add2 ZAHL, 3*9\n"
      "add3\n",
      ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 3 + 3 + 4 + 5 + 5);
}

TEST_F(RiscParserTest, WrongMacros) {
  FinalRepresentation res;
  res = parser.parse(
      ".equ ZAHL, 10\n"
      ".macro add1, x=0\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\x\n"
      ".endm\n"
      ".macro add1, x\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\x\n"
      ".endm\n"
      ".macro add2, x=0\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\x\n"
      ".endm\n"
      ".macro add2, x, y=0\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\y\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\y\n"
      "addi x0, x0, \\x\n"
      ".endm\n"
      ".macro add3, x=0, y\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\y\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\y\n"
      "addi x0, x0, \\x\n"
      ".endm\n"
      ".macro add4, x=0\n"
      "addi x0, x0, \\x0\n"
      ".endm\n"
      ".macro add5\n"
      "addi x0, x0, 5\n"
      "add6\n"
      ".endm\n"
      ".macro add6\n"
      "add5\n"
      ".endm\n"
      "add4\n"
      "add5\n",
      ParserMode::COMPILE);
  EXPECT_GE(res.errorList.size(), 5);
  EXPECT_EQ(res.commandList.size(), 2);
}


TEST_F(RiscParserTest, WrongMacroUnclosed) {
  FinalRepresentation res;
  res = parser.parse(
      ".macro add1, x=0\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\x\n"
      "addi x0, x0, \\x\n",
      ParserMode::COMPILE);
  EXPECT_GE(res.errorList.size(), 1);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST_F(RiscParserTest, MultipleInstructions) {
  FinalRepresentation res;
  res = parser.parse(
      ".equ ZAHL, 13\n"
      "ADDI x13, x4, ZAHL ;kommentar\n"
      " label  : SUB x5, x5, x0\n"
      "LUI x5, 123;kommentar\n"
      "addition123:\n"
      "\n"
      "ADD x0, x0, x0; kommentar",
      ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 4);
}

TEST_F(RiscParserTest, MalformedInstructions) {
  FinalRepresentation res;
  res = parser.parse("label ADD x13, x4,7\nadd x13 x4 ,7, 9\nble  ",
                     ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 3);
  EXPECT_EQ(res.commandList.size(), 3);
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
      "ADD x13, x4, x0 ;kommentar\n"
      " label  : SUB x5, x5, x0\n"
      ";kommentar\n"
      "sub  ;oops missing argument\n"
      "dfklgdjflj\n"
      "addition123:\n"
      "\n"
      "_addition456: ADD x0, x0, x0; kommentar",
      ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 2);
  EXPECT_EQ(res.commandList.size(), 5);
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
