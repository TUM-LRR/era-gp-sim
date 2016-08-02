#include "parser/riscv-parser.hpp"
#include "gtest/gtest.h"


TEST(RiscParser, EmptyString) {
  RiscvParser parser;
  FinalRepresentation res;
  res = parser.parse("", ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST(RiscParser, EmptyMultilineString) {
  RiscvParser parser;
  FinalRepresentation res;
  res = parser.parse("\n\n", ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST(RiscParser, SingleInstruction) {
  RiscvParser parser;
  FinalRepresentation res;
  res = parser.parse("ADD x13, x4,7", ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 1);
}

TEST(RiscParser, MultipleInstructions) {
  RiscvParser parser;
  FinalRepresentation res;
  res = parser.parse(
      "ADD x13, x4, 7 ;kommentar\n"
      " label  : SUB x5, x5, 1\n"
      "LUI x5, 123;kommentar\n"
      "addition123:\n"
      "\n"
      "ADD x0, x0, 0; kommentar",
      ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 4);
}

TEST(RiscParser, MalformedInstructions) {
  RiscvParser parser;
  FinalRepresentation res;
  res = parser.parse("label ADD x13, x4,7\nadd x13 x4 ,7\nble x15 ",
                     ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 3);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST(RiscParser, BadCharacters) {
  RiscvParser parser;
  FinalRepresentation res;
  res = parser.parse("ðŸ…±ðŸ…»ðŸ…¾ðŸ†‡ðŸ†‡ x13, x4,7", ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 1);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST(RiscParser, MixedErrors) {
  RiscvParser parser;
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
  EXPECT_EQ(res.errorList.size(), 2);
  EXPECT_EQ(res.commandList.size(), 3);
}