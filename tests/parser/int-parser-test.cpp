#include "parser/int-parser.hpp"
#include "gtest/gtest.h"

class IntParserTest : public ::testing::Test {
 public:
  CompileState state;

  void Test(const std::string &input, long long expected) {
    long long result;
    std::size_t before = state.errorList.size();
    EXPECT_TRUE(IntParser::parseInteger(input, result, state));
    std::size_t after = state.errorList.size();
    EXPECT_EQ(before, after);
    EXPECT_EQ(result, expected);
  }

  void TestFail(const std::string &input) {
    long long result;
    std::size_t before = state.errorList.size();
    EXPECT_FALSE(IntParser::parseInteger(input, result, state));
    std::size_t after = state.errorList.size();
    EXPECT_EQ(after, before + 1);
  }
};

TEST_F(IntParserTest, Decimals) {
  Test("12", 12);
  Test("0", 0);
  Test("-15", -15);
  Test("123456789012", 123456789012);
  Test("-123456789012", -123456789012);
}

TEST_F(IntParserTest, Octals) {
  Test("0123", 0123);
  Test("051", 051);
  Test("003", 03);
  Test("-0476", -0476);
  Test("03276537353", 03276537353);
}

TEST_F(IntParserTest, Hex) {
  Test("0x123", 0x123);
  Test("0x05d", 0x5d);
  Test("0xdEadBeEf", 0xdeadbeef);
  Test("-0x476f", -0x476f);
  Test("0x123456789012", 0x123456789012);
}

TEST_F(IntParserTest, Empty) {
  TestFail("");
  TestFail(" ");
  TestFail("\t");
}

TEST_F(IntParserTest, BadNumbers) {
  TestFail("123.643");
  TestFail("0xabfdg");
  TestFail("0821");
  TestFail("*87kj%$dljk");
  TestFail("ðŸ…±ðŸ…»ðŸ…¾ðŸ†‡ðŸ†‡");
  if (sizeof(long long) == 8) {
    TestFail("0x12345678901234567");
    TestFail("98437859734985739745873947593794515752308124");
  }
}
