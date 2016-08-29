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

#include "parser/expression-compiler-clike.hpp"
#include "common/utility.hpp"
#include "gtest/gtest.h"

#define TEST_CASE_P(expr) \
  { \
    CompileState state;\
    auto output = EXPRESSION_COMPILER_CLIKE.compile(STRINGIFY(expr), state); \
    auto expected = (expr); \
    ASSERT_EQ(output, expected); \
    ASSERT_TRUE(state.errorList.empty()); \
  }

#define TEST_CASE_E(expr) \
  { \
    CompileState state; \
    auto output = EXPRESSION_COMPILER_CLIKE.compile(expr, state); \
    ASSERT_TRUE(!state.errorList.empty()); \
  }

TEST(ExpressionCompilerClike, simple)
{
  TEST_CASE_P(1)
  TEST_CASE_P(0xff)
  TEST_CASE_P(65536)
}

TEST(ExpressionCompilerClike, simpleExpression)
{
  TEST_CASE_P(1+1)
  TEST_CASE_P(1+2*3)
  TEST_CASE_P(1<<16)
  TEST_CASE_P(0x2b || !0x2b)
}

TEST(ExpressionCompilerClike, bracketsExpression)
{
  TEST_CASE_P((2+3)*(4+5))
  TEST_CASE_P(((((((((((((((((((1)))))))))))))))))))
  TEST_CASE_P(312 * ((1 << 16) | 123))
}

TEST(ExpressionCompilerClike, complexExpression)
{
  TEST_CASE_P((0b101 << 12) & 0xaaaa - 1)
  TEST_CASE_P(1 | 2 ^ 3 & 4 << 5 >> 6 + 7 - 8 * 9 / 10 % 11)
  TEST_CASE_P(1+-1+ +2+ +3+ +4+ + + + +5+ + + + + + +6+ +-+-+-+-+-+-+-+7+-+-+-+-+- -+-+-+-8)
  TEST_CASE_P((1 | 2 | 4 | 8 | 16 | 32 | 64 | 128 | 256 & 0xff) ^ 0xaa == (0x55 << 1))
  TEST_CASE_P(-10*~10*-10*~10*-10*~10*-10*~10 / ~(1*2*3*4*5*6*7*8*9*10) + !0)
}

TEST(ExpressionCompilerClike, errors)
{
  TEST_CASE_E("1+")
  TEST_CASE_E("(1+2)*(3+4")
  TEST_CASE_E("1+2)*(3+4)")
  TEST_CASE_E("****1")
  TEST_CASE_E("Hi!")
}
