#include "parser/riscv-parser.hpp"
#include "gtest/gtest.h"


TEST(RiscParser, EmptyString) {
  RiscvParser parser;
  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>> nodes;
  nodes = parser.parse("", ParserMode::COMPILE);
  EXPECT_EQ(parser.getErrorList().size(), 0);
  EXPECT_EQ(nodes.size(), 0);
}

TEST(RiscParser, EmptyMultilineString) {
  RiscvParser parser;
  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>> nodes;
  nodes = parser.parse("\n\n", ParserMode::COMPILE);
  EXPECT_EQ(parser.getErrorList().size(), 0);
  EXPECT_EQ(nodes.size(), 0);
}

TEST(RiscParser, SingleInstruction) {
  RiscvParser parser;
  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>> nodes;
  nodes = parser.parse("ADD r13, r4,7", ParserMode::COMPILE);
  EXPECT_EQ(parser.getErrorList().size(), 0);
  EXPECT_EQ(nodes.size(), 1);
}

TEST(RiscParser, MultipleInstructions) {
  RiscvParser parser;
  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>> nodes;
  nodes = parser.parse(
      "ADD r13, r4, 7 ;kommentar\n"
      " label  : SUB r5, r5, 1\n"
      ";kommentar\n"
      "addition123:\n"
      "\n"
      "ADD r0, r0, 0; kommentar",
      ParserMode::COMPILE);
  EXPECT_EQ(parser.getErrorList().size(), 0);
  EXPECT_EQ(nodes.size(), 3);
}

TEST(RiscParser, MalformedInstructions) {
  RiscvParser parser;
  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>> nodes;
  nodes = parser.parse("label ADD r13, r4,7\nadd r13 r4 ,7\nble r15 , 7",
                       ParserMode::COMPILE);
  EXPECT_EQ(parser.getErrorList().size(), 3);
  EXPECT_EQ(nodes.size(), 0);
}

TEST(RiscParser, BadCharacters) {
  RiscvParser parser;
  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>> nodes;
  nodes = parser.parse("ðŸ…±ðŸ…»ðŸ…¾ðŸ†‡ðŸ†‡ r13, r4,7", ParserMode::COMPILE);
  EXPECT_EQ(parser.getErrorList().size(), 1);
  EXPECT_EQ(nodes.size(), 0);
}

TEST(RiscParser, MixedErrors) {
  RiscvParser parser;
  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>> nodes;
  nodes = parser.parse(
      "ADD r13, r4, 7 ;kommentar\n"
      " label  : SUB r5, r5, 1\n"
      ";kommentar\n"
      "sub r2, r3 ;oops missing argument\n"
      "dfklgdjflj\n"
      "addition123:\n"
      "\n"
      "_addition456: ADD r0, r0, 0; kommentar",
      ParserMode::COMPILE);
  EXPECT_EQ(parser.getErrorList().size(), 2);
  EXPECT_EQ(nodes.size(), 3);
}