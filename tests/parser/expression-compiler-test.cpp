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

#include "common/utility.hpp"
#include "gtest/gtest.h"
#include "parser/expression-compiler-clike.hpp"

// Passing test case.
#define TEST_CASE_P(expr)                                            \
  {                                                                  \
    CompileState state;                                              \
    int output = CLikeExpressionCompilers::CLikeCompilerI32.compile( \
        STRINGIFY(expr), state);                                     \
    int expected = (expr);                                           \
    ASSERT_EQ(output, expected);                                     \
    ASSERT_TRUE(state.errorList.empty());                            \
  }

// Failing test case.
#define TEST_CASE_E(expr)                                                \
  {                                                                      \
    CompileState state;                                                  \
    int output =                                                         \
        CLikeExpressionCompilers::CLikeCompilerI32.compile(expr, state); \
    ASSERT_TRUE(!state.errorList.empty());                               \
  }

// We need to surpass some warnings, b/c we intentionally want to use
// unusual precedences.
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses"
#pragma clang diagnostic ignored "-Wshift-op-parentheses"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4554)
#endif

TEST(ExpressionCompilerClike, simple) {
  // Parsing some numbers.
  TEST_CASE_P(1)
  TEST_CASE_P(0xff)
  TEST_CASE_P(65536)
}

TEST(ExpressionCompilerClike, simpleExpression) {
  // Parsing some simple expressions.
  TEST_CASE_P(1 + 1)
  TEST_CASE_P(1 + 2 * 3)
  TEST_CASE_P(1 << 16)
  TEST_CASE_P(0x2b || !0x2b)
}

TEST(ExpressionCompilerClike, bracketsExpression) {
  // Parsing with brackets.
  TEST_CASE_P((2 + 3) * (4 + 5))
  TEST_CASE_P(((((((((((((((((((1)))))))))))))))))))
  TEST_CASE_P(312 * ((1 << 16) | 123))
}

TEST(ExpressionCompilerClike, complexExpression) {
  // Parsing more complicated stuff.
  TEST_CASE_P((0b101 << 12) & 0xaaaa - 1)
  TEST_CASE_P(1 | 2 ^ 3 & 4 << 5 >> 6 + 7 - 8 * 9 / 10 % 11)
  TEST_CASE_P(1 + -1 + +2 + +3 + +4 + + + + + 5 + + + + + + + 6 + + - + - + -
              + - + - + - + - +7 + - + - + - + - + - - + - + - + - 8)
  TEST_CASE_P((1 | 2 | 4 | 8 | 16 | 32 | 64 | 128 | 256 & 0xff) ^
              0xaa == (0x55 << 1))
  TEST_CASE_P(-10 * ~10 * -10 * ~10 * -10 * ~10 * -10 * ~10 /
                  ~(1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9 * 10) +
              !0)
}

TEST(ExpressionCompilerClike, errors) {
  // Parsing stuff that fails.
  TEST_CASE_E("1+")
  TEST_CASE_E("(1+2)*(3+4")
  TEST_CASE_E("1+2)*(3+4)")
  TEST_CASE_E("****1")
  TEST_CASE_E("Hi!")
}

// Now we can return the warning stuff to normal.
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif
