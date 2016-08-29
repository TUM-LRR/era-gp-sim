#include "parser/int-parser.hpp"
#include "gtest/gtest.h"

class IntParserTest : public ::testing::Test {
 public:
  CompileState state;

  template <typename OutT>
  void Test(const std::string &input, OutT expected) {
    OutT result;
    std::size_t before = state.errorList.size();
    EXPECT_TRUE(IntParser::parseInteger(input, result, state));
    std::size_t after = state.errorList.size();
    EXPECT_EQ(before, after);
    EXPECT_EQ(result, expected);
  }

  template <typename OutT>
  void TestFail(const std::string &input) {
    OutT result;
    std::size_t before = state.errorList.size();
    EXPECT_FALSE(IntParser::parseInteger(input, result, state));
    std::size_t after = state.errorList.size();
    EXPECT_EQ(after, before + 1);
  }
};

TEST_F(IntParserTest, Decimals) {
  Test("+12", 12);
  Test("0", 0);
  Test("-15", -15);
  Test("123456789012", 123456789012);
  Test("-123456789012", -123456789012);
}

TEST_F(IntParserTest, Octals) {
  Test("0123", 0123);
  Test("+051", 051);
  Test("003", 03);
  Test("-0476", -0476);
  Test("03276537353", 03276537353);
}

TEST_F(IntParserTest, Hex) {
  Test("0x123", 0x123);
  Test("0x05d", 0x5d);
  Test<long long>("0xdEadBeEf", 0xdeadbeef);
  Test("-0x476f", -0x476f);
  Test("+0x123456789012", 0x123456789012);
  Test("0xFFFFFFFFFFFFFFFF", 0xFFFFFFFFFFFFFFFF);// unsigned long long
}

TEST_F(IntParserTest, Empty) {
  TestFail<int>("");
  TestFail<int>(" ");
  TestFail<int>("\t");
}

TEST_F(IntParserTest, BadNumbers) {
  TestFail<int>("+123.643");
  TestFail<int>("0xabfdg");
  TestFail<int>("0821");
  TestFail<int>("*87kj%$dljk");
  TestFail<int>(" 12");
  TestFail<int>("ðŸ…±ðŸ…»ðŸ…¾ðŸ†‡ðŸ†‡");
}

TEST_F(IntParserTest, OverflowNumbers) {
  if (sizeof(int) <= 4) {
    TestFail<int>("3000000000");
    TestFail<int>("-3000000000");
  }
  if (sizeof(long) <= 4) {
    TestFail<long>("0xFFFFFFFF");
    TestFail<long>("-0xFabcdefae");
  }
  TestFail<long long>("873649269587634295827634298576378645283764576283452435");

  TestFail<unsigned long>("-15");
}