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

#include "parser/expression-compiler.hpp"
#include "gtest/gtest.h"

TEST(ExpressionTokenizer, tokenizeEmpty) {
    CompileState state;
    ExpressionTokenizer tokenizer;
    auto result = tokenizer.tokenize("", state);
    ASSERT_EQ(state.errorList.size(), 0);
}

TEST(ExpressionTokenizer, tokenizeComplex) {
    CompileState state;
    ExpressionTokenizer tokenizer;
    auto result = tokenizer.tokenize("1+2+3-6?(4%123-!3<<123==~6):1<2>+3<=-4>=5", state);
    ASSERT_EQ(state.errorList.size(), 0);
}

TEST(ExpressionCompiler, compileComplex) {
    CompileState state;
    ExpressionCompiler compiler;
    auto result = compiler.compile("1+2+3-6?(4%123-!3<<123==~6):1<2>+3<=-4>=5", state);
    ASSERT_EQ(state.errorList.size(), 0);
    ASSERT_EQ(result, 0);
}

TEST(ExpressionCompiler, compileComplex2) {
    CompileState state;
    ExpressionCompiler compiler;
    auto result = compiler.compile("1+2|3-6*(4%123-!3<<123/6)+12314^56672+3+-4&5", state);
    ASSERT_EQ(state.errorList.size(), 0);
    ASSERT_EQ(result, -25153509);
}
