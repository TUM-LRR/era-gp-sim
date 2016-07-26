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

#include "parser\expression-tokenizer.hpp"

#include <vector>
#include <regex>

#define RULE(type, rx) TokenizeRule{ std::regex("^\\s*(" rx ")"), TokenType::type }

using TokenizeRule = std::pair<std::regex, TokenType>;

static const std::vector<TokenizeRule> RULES_DEFINITION = {
    RULE(NUMBER, "[0-9]+|0x[0-9a-fA-F]+|0b[01]+"),
    RULE(OPERATOR, "\\+|\\-|\\*|\\/|\\%|\\^|\\|\\||\\&\\&|\\||\\&|\\=\\=|\\!\\=|\\<\\<|\\>\\>|\\>\\=|\\<\\=|\\>|\\<|\\!|\\~"),
    RULE(LEFT_BRACKET, "\\("),
    RULE(RIGHT_BRACKET, "\\)"),
    RULE(FUNCTION_NAME, "[a-zA-Z_][a-zA-Z0-9_]*"),
    RULE(LEFT_TERNARY, "\\?"),
    RULE(RIGHT_TERNARY, "\\:"),
    RULE(COMMA, ",")
};

std::vector<Token> ExpressionTokenizer::tokenize(const std::string& input, CompileState& state)
{
	std::vector<Token> tokenVector;
    size_t i = 0;
    while (i < input.length())
    {
        std::smatch match;
        bool matched = false;
        auto sub = input.substr(i);
        for (const auto& j : RULES_DEFINITION)
        {
            if (std::regex_search(sub, match, j.first) && match.size() > 1)
            {
                auto tokenString = match[1].str();
                Token token(j.second, tokenString);
                tokenVector.push_back(token);
                i += match.position(1) + match[1].length();
                matched = true;
                break;
            }
        }
        if (!matched)
        {
            state.errorList.push_back(CompileError("There is some unexpected input at " + sub.substr(0, 20) + "[...]", state.position, CompileErrorSeverity::ERROR));
            return std::vector<Token>();
        }
    }
    return tokenVector;
}