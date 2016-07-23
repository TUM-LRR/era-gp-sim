/*
* C++ Assembler Interpreter
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

#ifndef ERAGPSIM_PARSER_EXPRESSION_COMPILER_HPP_
#define ERAGPSIM_PARSER_EXPRESSION_COMPILER_HPP_

#include <memory>
#include <string>
#include "compile-state.hpp"
#include "expression-parser.hpp"
#include "expression-tokenizer.hpp"

using MemoryValue = long long;

class ExpressionCompiler {
public:
    ExpressionCompiler() = default;
    MemoryValue compile(const std::string& expression, CompileState& state)
    {
        auto tokens = _tokenizer.tokenize(expression, state);
        auto parsed = _parser.parse(tokens, state);
        return parsed;
    }
private:
    ExpressionTokenizer _tokenizer;
    ExpressionParser _parser;
};

#endif /* ERAGPSIM_PARSER_EXPRESSION_COMPILER_HPP_ */
