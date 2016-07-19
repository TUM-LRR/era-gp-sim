#include "parser/riscv-parser.hpp"
#include "gtest/gtest.h"

TEST(RiscParser, EmptyString) {
  RiscvParser parser;
  parser.parse("", ParserMode::COMPILE);
}

TEST(RiscParser, EmptyMultilineString) {
  RiscvParser parser;
  parser.parse("\n\n", ParserMode::COMPILE);
}

TEST(RiscParser, SingleInstruction) {
  RiscvParser parser;
  parser.parse("ADD r13, r4,7", ParserMode::COMPILE);
}

TEST(RiscParser, MultipleInstructions) {
  RiscvParser parser;
  parser.parse(
      "ADD r13, r4, 7 ;kommentar\n"
      " label  : SUB r5, r5, 1\n"
	  ";kommentar\n"
      "addition123:\n\n"
      "ADD r0, r0, 0; kommentar",
      ParserMode::COMPILE);
}

TEST(RiscParser, MalformedInstructions) {
  RiscvParser parser;
  parser.parse("label ADD r13, r4,7\nadd r13 r4 ,7\nble r15 , 7",
               ParserMode::COMPILE);
}

TEST(RiscParser, BadCharacters) {
  RiscvParser parser;
  parser.parse("ðŸ…±ðŸ…»ðŸ…¾ðŸ†‡ðŸ†‡ r13, r4,7", ParserMode::COMPILE);
}