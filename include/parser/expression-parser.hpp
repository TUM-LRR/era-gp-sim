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

#ifndef ERAGPSIM_PARSER_EXPRESSION_PARSER_HPP
#define ERAGPSIM_PARSER_EXPRESSION_PARSER_HPP

#include "expression-compiler-definitions.hpp"
#include <unordered_map>
#include <regex>
#include <stack>
#include <algorithm>
#include <cassert>

template<typename T>
class ExpressionParser
{
public:
	ExpressionParser(const ExpressionParserDefinition<T>& definition)
	{
		for (const auto& i : definition.binaryOperators)
		{
			_binaryOperators[i.identifier] = i;
		}
		for (const auto& i : definition.unaryOperators)
		{
			_unaryOperators[i.identifier] = i;
		}

		_literalDecoders.reserve(definition.literalDecoders.size());
		for (const auto& i : definition.literalDecoders)
		{
			_literalDecoders.push_back(ILiteralDecoder{ std::regex("^" + i.regex + "$"), i.decoder });
		}
	}

	T parse(const std::vector<ExpressionToken>& tokens, CompileState& externalState) const
	{
		ParseState state(tokens, externalState);
		for (const auto& i : tokens)
		{
			state.curr = i;

			if (!handleToken(state))
			{
				return T();
			}

			state.last = state.curr;
		}

		if (!decreaseStack(state, IToken{ ITokenType::EXPRESSION, "" }))
		{
			return T();
		}
		
		if (state.outputStack.size() != 1)
		{
			recordError(state, "Malformed expression!");
			return T();
		}

		return state.outputStack.top();
	}
private:
	enum class ITokenType
	{
		BINARY_OPERATOR,
		UNARY_OPERATOR,
		BRACKET,
		EXPRESSION
	};

	struct IToken
	{
		ITokenType type;
		std::string data;
	};

	struct ILiteralDecoder
	{
		std::regex regex;
		std::function<T(std::string)> decoder;
	};

	struct ParseState
	{
		ExpressionToken last = ExpressionToken{ "", ExpressionTokenType::INVALID };
		ExpressionToken curr = ExpressionToken{ "", ExpressionTokenType::INVALID };
		const std::vector<ExpressionToken>& tokens;
        CompileState& state;
		std::stack<T> outputStack;
		std::stack<IToken> operatorStack;

        ParseState(const std::vector<ExpressionToken>& xtokens, CompileState& xstate)
            :tokens(xtokens),state(xstate)
        {

        }
	};

	void recordError(ParseState& state, const std::string& message) const
	{
        auto pos = state.state.position;
        pos.second += state.curr.index;
		state.state.errorList.push_back(CompileError(message, pos, CompileErrorSeverity::ERROR));
	}

	bool handleToken(ParseState& state) const
	{
		switch (state.curr.type)
		{
		case ExpressionTokenType::OPERATOR:
		{
			auto no = IToken{ isUnary(state) ? ITokenType::UNARY_OPERATOR : ITokenType::BINARY_OPERATOR, state.curr.data };
			return pushOperator(state, no);
		}
		case ExpressionTokenType::LEFT_BRACKET:
			state.operatorStack.push(IToken{ ITokenType::BRACKET, "" });
			return true;
		case ExpressionTokenType::LITERAL:
			return parseLiteral(state);
		case ExpressionTokenType::RIGHT_BRACKET:
			return handleBorder(state, IToken{ ITokenType::BRACKET, "" });
		case ExpressionTokenType::NAME:			
		default:
			recordError(state, "Invalid token found!");
			return false;
		}

		assert(false);
		return false;
	}

	bool pushOperator(ParseState& state, const IToken& token) const
	{
		if (!valid(token))
		{
			if (token.type == ITokenType::UNARY_OPERATOR)
			{
				recordError(state, "'" + token.data + "' is not a valid unary operator!");
			}
			else if (token.type == ITokenType::BINARY_OPERATOR)
			{
				recordError(state, "'" + token.data + "' is not a valid binary operator!");
			}
			else
			{
				assert(false);
			}
			return false;
		}
		if (!decreaseStack(state, token))
		{
			return false;
		}
		state.operatorStack.push(token);
		return true;
	}

	bool handleBorder(ParseState& state, const IToken& token) const
	{
		if (!decreaseStack(state, token))
		{
			return false;
		}
		if (state.operatorStack.empty())
		{
			recordError(state, "There are some opening brackets missing!");
			return false;
		}
		state.operatorStack.pop();
		return true;
	}

	bool isUnary(ParseState& state) const
	{
		return !(state.last.type == ExpressionTokenType::LITERAL || state.last.type == ExpressionTokenType::RIGHT_BRACKET);
	}

	bool parseLiteral(ParseState& state) const
	{
		for (const auto& i : _literalDecoders)
		{
			if (std::regex_match(state.curr.data, i.regex))
			{
				state.outputStack.push(i.decoder(state.curr.data));
				return true;
			}
		}
		return false;
	}

	bool getValues(ParseState& state, size_t count, std::vector<T>& output) const
	{
		for (size_t i = 0; i < count; ++i)
		{
			if (state.outputStack.empty())
			{
				recordError(state, "There is an operand missing!");
				return false;
			}
			output.push_back(state.outputStack.top());
			state.outputStack.pop();
		}
		std::reverse(output.begin(), output.end());
		return true;
	}

	bool handleUnaryOperator(ParseState& state, const IToken& token) const
	{
		std::vector<T> args;
		if (!getValues(state, 1, args))
		{
			return false;
		}
		T outputValue = _unaryOperators.at(token.data).handler(args[0]);
		state.outputStack.push(outputValue);
		return true;
	}

	bool handleBinaryOperator(ParseState& state, const IToken& token) const
	{
		std::vector<T> args;
		if (!getValues(state, 2, args))
		{
			return false;
		}
		T outputValue = _binaryOperators.at(token.data).handler(args[0], args[1]);
		state.outputStack.push(outputValue);
		return true;
	}

	bool applyTopmostToken(ParseState& state) const
	{
		assert(!state.operatorStack.empty());

		IToken token = state.operatorStack.top();
		
		switch (token.type)
		{
		case ITokenType::BRACKET:
		case ITokenType::EXPRESSION:
			recordError(state, "There are some closing brackets missing!");
			return false;
		case ITokenType::UNARY_OPERATOR:
			return handleUnaryOperator(state, token);
		case ITokenType::BINARY_OPERATOR:
			return handleBinaryOperator(state, token);
		}

		assert(false);
		return false;
	}

	bool decreaseStack(ParseState& state, const IToken& token) const
	{
		int mPrec = precedence(token);
		bool mEqual = popSamePrecedence(token);

		while (!state.operatorStack.empty()
			&& (mPrec > precedence(state.operatorStack.top())
				|| (mEqual && mPrec == precedence(state.operatorStack.top()))))
		{
			if (!applyTopmostToken(state))
			{
				return false;
			}
			state.operatorStack.pop();
		}
		return true;
	}

	bool valid(const IToken& token) const
	{
		if (token.type == ITokenType::UNARY_OPERATOR)
		{
			return _unaryOperators.find(token.data) != _unaryOperators.end();
		}
		else if (token.type == ITokenType::BINARY_OPERATOR)
		{
			return _binaryOperators.find(token.data) != _binaryOperators.end();
		}
		else
		{
			return true;
		}
	}

	bool popSamePrecedence(const IToken& token) const
	{
		switch (token.type)
		{
		case ITokenType::UNARY_OPERATOR:
		case ITokenType::BRACKET:
		case ITokenType::EXPRESSION:
			return false;
		case ITokenType::BINARY_OPERATOR:
			return _binaryOperators.at(token.data).associativity == ExpressionOperatorAssociativity::LEFT;
		}

		assert(false);
		return false;
	}

	int precedence(const IToken& token) const
	{
		switch (token.type)
		{
		case ITokenType::UNARY_OPERATOR:
			return -1;
		case ITokenType::BRACKET:
			return 0x10000;
		case ITokenType::EXPRESSION:
			return 0x1ffff;
		case ITokenType::BINARY_OPERATOR:
			return _binaryOperators.at(token.data).precedence;
		}

		assert(false);
		return false;
	}

	std::unordered_map<std::string, ExpressionBinaryOperator<T>> _binaryOperators;
	std::unordered_map<std::string, ExpressionUnaryOperator<T>> _unaryOperators;
	std::vector<ILiteralDecoder> _literalDecoders;	
};

#endif /* ERAGPSIM_PARSER_EXPRESSION_PARSER_HPP */
