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
  res = parser.parse("ADD r13, r4,7", ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 1);
}

TEST(RiscParser, MultipleInstructions) {
  RiscvParser parser;
  FinalRepresentation res;
  res = parser.parse(
      "ADD r13, r4, 7 ;kommentar\n"
      " label  : SUB r5, r5, 1\n"
      ";kommentar\n"
      "addition123:\n"
      "\n"
      "ADD r0, r0, 0; kommentar",
      ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 0);
  EXPECT_EQ(res.commandList.size(), 3);
}

TEST(RiscParser, MalformedInstructions) {
  RiscvParser parser;
  FinalRepresentation res;
  res = parser.parse("label ADD r13, r4,7\nadd r13 r4 ,7\nble r15 , 7",
                       ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 3);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST(RiscParser, BadCharacters) {
  RiscvParser parser;
  FinalRepresentation res;
  res = parser.parse("ðŸ…±ðŸ…»ðŸ…¾ðŸ†‡ðŸ†‡ r13, r4,7", ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 1);
  EXPECT_EQ(res.commandList.size(), 0);
}

TEST(RiscParser, MixedErrors) {
  RiscvParser parser;
  FinalRepresentation res;
  res = parser.parse(
      "ADD r13, r4, 7 ;kommentar\n"
      " label  : SUB r5, r5, 1\n"
      ";kommentar\n"
      "sub r2, r3 ;oops missing argument\n"
      "dfklgdjflj\n"
      "addition123:\n"
      "\n"
      "_addition456: ADD r0, r0, 0; kommentar",
      ParserMode::COMPILE);
  EXPECT_EQ(res.errorList.size(), 2);
  EXPECT_EQ(res.commandList.size(), 3);
}