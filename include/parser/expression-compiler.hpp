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

#ifndef ERAGPSIM_PARSER_EXPRESSION_COMPILER_HPP
#define ERAGPSIM_PARSER_EXPRESSION_COMPILER_HPP

#include "expression-compiler-definitions.hpp"
#include "expression-tokenizer.hpp"
#include "expression-parser.hpp"

#include <vector>
#include <set>

template<typename T>
class ExpressionCompiler
{
public:
	ExpressionCompiler(const ExpressionCompilerDefinition<T>& definition)
		: _parser(definition.parserDefinition), _tokenizer(getTokenDefinitions(definition))
	{
		
	}

	ExpressionCompiler(const ExpressionTokenizer& tokenizer, const ExpressionParser<T>& parser)
		: _tokenizer(tokenizer), _parser(parser)
	{}

	T compile(const std::string& str, CompileState& state) const
	{
		auto tokens = _tokenizer.tokenize(str, state);
		return _parser.parse(tokens, state);
	}
private:
	std::vector<ExpressionTokenDefinition> getTokenDefinitions(const ExpressionCompilerDefinition<T>& definition)
	{
		std::vector<ExpressionTokenDefinition> tokens;
		std::set<std::string> operatorStrings;
		for (const auto& i : definition.parserDefinition.binaryOperators)
		{
			operatorStrings.insert(i.identifier);
		}
		for (const auto& i : definition.parserDefinition.unaryOperators)
		{
			operatorStrings.insert(i.identifier);
		}

		std::string operatorRegex;
		for (auto it = operatorStrings.rbegin(); it != operatorStrings.rend(); ++it)
		{
			operatorRegex += "|(";
			operatorRegex += std::regex_replace(*it, std::regex("(\\/|\\^|\\$|\\\\|\\.|\\*|\\+|\\?|\\(|\\)|\\[|\\]|\\{|\\}|\\|)"), "\\$1");
			operatorRegex += ")";
		}
		tokens.push_back(ExpressionTokenDefinition{ operatorRegex.substr(1), ExpressionTokenType::OPERATOR });

		std::string literalRegex;
		for (const auto& i : definition.parserDefinition.literalDecoders)
		{
			literalRegex += "|(" + i.regex + ")";
		}
		tokens.push_back(ExpressionTokenDefinition{ literalRegex.substr(1), ExpressionTokenType::LITERAL });

		tokens.push_back(ExpressionTokenDefinition{ definition.helpers.leftBracket, ExpressionTokenType::LEFT_BRACKET });
		tokens.push_back(ExpressionTokenDefinition{ definition.helpers.rightBracket, ExpressionTokenType::RIGHT_BRACKET });

		return tokens;
	}

	ExpressionTokenizer _tokenizer;
	ExpressionParser<T> _parser;
};

#endif /* ERAGPSIM_PARSER_EXPRESSION_COMPILER_HPP */
