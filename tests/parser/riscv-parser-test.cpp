#include "parser/riscv-parser.hpp"
#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "gtest/gtest.h"

// Tests will be refined, once arch classes are ready.

class RiscParserTest : public ::testing::Test {
 public:
  RiscvParser parser;

  RiscParserTest()
  : parser{Architecture::Brew(ArchitectureFormula{"riscv", {"rv32i"}})} {
  }
};


TEST_F(RiscParserTest, EmptyString) {
  FinalRepresentation res;
  res = parser.parse("", ParserMode::COMPILE);
  // EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST_F(RiscParserTest, EmptyMultilineString) {
  FinalRepresentation res;
  res = parser.parse("\n\n", ParserMode::COMPILE);
  // EXPECT_EQ(res.errorList.size(), 0);
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
  // EXPECT_EQ(res.errorList.size(), 3);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST_F(RiscParserTest, BadCharacters) {
  FinalRepresentation res;
  res = parser.parse("ðŸ…±ðŸ…»ðŸ…¾ðŸ†‡ðŸ†‡ x13, x4,7", ParserMode::COMPILE);
  // EXPECT_EQ(res.errorList.size(), 1);
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