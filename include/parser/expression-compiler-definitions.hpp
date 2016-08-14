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

#ifndef ERAGPSIM_PARSER_EXPRESSION_COMPILER_DEFINITIONS_HPP
#define ERAGPSIM_PARSER_EXPRESSION_COMPILER_DEFINITIONS_HPP

#include "compile-state.hpp"
#include <string>
#include <functional>
#include <vector>

enum class ExpressionTokenType
{
	INVALID,
	OPERATOR,
	LITERAL,
	LEFT_BRACKET,
	RIGHT_BRACKET,
	NAME
};

enum class ExpressionOperatorAssociativity
{
	LEFT,
	RIGHT
};

template<typename T>
struct ExpressionBinaryOperator
{
	ExpressionOperatorAssociativity associativity;
	std::string identifier;
	unsigned short precedence;
	std::function<T(const T&, const T&)> handler;
};

template<typename T>
struct ExpressionUnaryOperator
{
	std::string identifier;
	std::function<T(const T&)> handler;
};

template<typename T>
struct ExpressionLiteralDecoder
{
	std::string regex;
	std::function<T(const std::string&)> decoder;
};

struct ExpressionHelpRegexes
{
	std::string leftBracket;
	std::string rightBracket;
	std::string name;
};

template<typename T>
struct ExpressionParserDefinition
{
	std::vector<ExpressionBinaryOperator<T>> binaryOperators;
	std::vector<ExpressionUnaryOperator<T>> unaryOperators;
	std::vector<ExpressionLiteralDecoder<T>> literalDecoders;
};

template<typename T>
struct ExpressionCompilerDefinition
{
	ExpressionParserDefinition<T> parserDefinition;
	ExpressionHelpRegexes helpers;
};

struct ExpressionTokenDefinition
{
	std::string regex;
	ExpressionTokenType type;
};

struct ExpressionToken
{
	std::string data;
	ExpressionTokenType type;
	size_t index;
};

#endif /* ERAGPSIM_PARSER_EXPRESSION_COMPILER_DEFINITIONS_HPP */
