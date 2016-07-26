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

#include "parser\expression-parser.hpp"

#include <stack>
#include <functional>
#include <unordered_map>
#include <cmath>

#include <iostream>

struct BinaryOperator {
    unsigned int precedence;
    std::string representation;
    std::function<MemoryValue(const MemoryValue&, const MemoryValue&, CompileState&)> handler;
};

struct UnaryOperator {
	std::string representation;
	std::function<MemoryValue(const MemoryValue&, CompileState&)> handler;
};

struct InlineFunction {
    std::string name;
    std::function<MemoryValue(const std::vector<MemoryValue>&, CompileState&)> handler;
	int operatorCount;
};

#define INLINE_FUNCTION(name, operatorCount, handler) { name, InlineFunction{ name, [](const std::vector<MemoryValue>& arguments, CompileState&) -> MemoryValue { handler }, operatorCount } }
#define BINARY_OPERATOR(representation, precedence, function) { representation, BinaryOperator{precedence,representation,[](const MemoryValue& first, const MemoryValue& second, CompileState& state)->MemoryValue{function} }}
#define UNARY_OPERATOR(representation, function) { representation, UnaryOperator{representation, [](const MemoryValue& operand, CompileState& state)->MemoryValue{function}}}

static const std::unordered_map<std::string, InlineFunction> INLINE_FUNCTION_DEFINITION = {
	INLINE_FUNCTION("abs", 1, { return std::abs(arguments[0]); }),
	INLINE_FUNCTION("max", -1,{ return arguments[0] > arguments[1] ? arguments[0] : arguments[1]; }),
	INLINE_FUNCTION("min", -1,{ return arguments[0] < arguments[1] ? arguments[0] : arguments[1]; })
};

static const std::unordered_map<std::string, BinaryOperator> BINARY_OPERATOR_DEFINITION = {
	BINARY_OPERATOR("||", 12, { return first || second ? MemoryValue(1) : MemoryValue(0); }),
	BINARY_OPERATOR("&&", 11, { return first && second ? MemoryValue(1) : MemoryValue(0); }),
	BINARY_OPERATOR("|", 10,{ return first | second; }),
	BINARY_OPERATOR("^", 9,{ return first ^ second; }),
	BINARY_OPERATOR("&", 8,{ return first & second; }),
	BINARY_OPERATOR("==", 7,{ return first == second ? MemoryValue(1) : MemoryValue(0); }),
	BINARY_OPERATOR("!=", 7,{ return first != second ? MemoryValue(1) : MemoryValue(0); }),
	BINARY_OPERATOR(">", 6,{ return first > second ? MemoryValue(1) : MemoryValue(0); }),
	BINARY_OPERATOR(">=", 6,{ return first >= second ? MemoryValue(1) : MemoryValue(0); }),
	BINARY_OPERATOR("<", 6,{ return first < second ? MemoryValue(1) : MemoryValue(0); }),
	BINARY_OPERATOR("<=", 6,{ return first <= second ? MemoryValue(1) : MemoryValue(0); }),
	BINARY_OPERATOR(">>", 5,{ return first >> second; }),
	BINARY_OPERATOR("<<", 5,{ return first << second; }),
	BINARY_OPERATOR("+", 4,{ return first + second; }),
	BINARY_OPERATOR("-", 4,{ return first - second; }),
	BINARY_OPERATOR("*", 3,{ return first * second; }),
	BINARY_OPERATOR("/", 3,{ return first / second; }),
	BINARY_OPERATOR("%", 3,{ return first % second; })
};

static const std::unordered_map<std::string, UnaryOperator> UNARY_OPERATOR_DEFINITION = {
	UNARY_OPERATOR("+", { return +operand; }),
	UNARY_OPERATOR("-", { return -operand; }),
	UNARY_OPERATOR("~",{ return ~operand; }),
	UNARY_OPERATOR("!",{ return !operand ? MemoryValue(1) : MemoryValue(0); })
};

static int precedence(const Token& curr, CompileState& state)
{
	if (curr.first != TokenType::OPERATOR && curr.first != TokenType::UNARY_OPERATOR)
	{
		return 100;
	}
	else if (curr.first == TokenType::OPERATOR)
	{
		if (BINARY_OPERATOR_DEFINITION.find(curr.second) == BINARY_OPERATOR_DEFINITION.end())
		{
			state.errorList.push_back(CompileError("Unrecognized binary operator: " + curr.second, state.position, CompileErrorSeverity::ERROR));
			return -1;
		}

		return BINARY_OPERATOR_DEFINITION.at(curr.second).precedence;
	}
	else
	{
		if (UNARY_OPERATOR_DEFINITION.find(curr.second) == UNARY_OPERATOR_DEFINITION.end())
		{
			state.errorList.push_back(CompileError("Unrecognized unary operator: " + curr.second, state.position, CompileErrorSeverity::ERROR));
			return -1;
		}
		
		return 1;
	}
	return -1;
}

static void applyOperator(const Token& curr, std::stack<MemoryValue>& outputStack, CompileState& state)
{
	if (curr.first == TokenType::LEFT_BRACKET)
	{
		state.errorList.push_back(CompileError("Parentheses unbalanced. Please add some more!", state.position, CompileErrorSeverity::ERROR));
		return;
	}
	else if (curr.first != TokenType::OPERATOR && curr.first != TokenType::UNARY_OPERATOR && curr.first != TokenType::LEFT_TERNARY)
	{
		state.errorList.push_back(CompileError("Welp some internal error occurred... Please inform the devs and rewrite your expression a bit.", state.position, CompileErrorSeverity::ERROR));
		return;
	}
	else if (curr.first == TokenType::LEFT_TERNARY)
	{
		if (outputStack.size() < 3)
		{
			state.errorList.push_back(CompileError("Parsing error.x", state.position, CompileErrorSeverity::ERROR));
			return;
			//ERROR
		}

		const auto& falseOption = outputStack.top();
		outputStack.pop();
		const auto& trueOption = outputStack.top();
		outputStack.pop();
		const auto& decision = outputStack.top();
		outputStack.pop();

		if (decision != 0)
		{
			outputStack.push(trueOption);
		}
		else
		{
			outputStack.push(falseOption);
		}
	}
	else if (curr.first == TokenType::OPERATOR)//last.first == TokenType::RIGHT_BRACKET || last.first == TokenType::NUMBER)
	{
		if (BINARY_OPERATOR_DEFINITION.find(curr.second) == BINARY_OPERATOR_DEFINITION.end())
		{
			state.errorList.push_back(CompileError("Parsing error.3", state.position, CompileErrorSeverity::ERROR));
			return;
			//ERROR
		}

		if (outputStack.size() < 2)
		{
			state.errorList.push_back(CompileError("Parsing error.4", state.position, CompileErrorSeverity::ERROR));
			return;
			//ERROR
		}

		const auto& op = BINARY_OPERATOR_DEFINITION.at(curr.second);

		const auto& second = outputStack.top();
		outputStack.pop();
		const auto& first = outputStack.top();
		outputStack.pop();

		const auto& result = op.handler(first, second, state);
		outputStack.push(result);
	}
	else
	{
		if (UNARY_OPERATOR_DEFINITION.find(curr.second) == UNARY_OPERATOR_DEFINITION.end())
		{
			state.errorList.push_back(CompileError("Parsing error.5", state.position, CompileErrorSeverity::ERROR));
			return;
		}

		if (outputStack.size() < 1)
		{
			state.errorList.push_back(CompileError("Parsing error.6", state.position, CompileErrorSeverity::ERROR));
			return;
		}

		const auto& op = UNARY_OPERATOR_DEFINITION.at(curr.second);

		const auto& operand = outputStack.top();
		outputStack.pop();

		const auto& result = op.handler(operand, state);
		outputStack.push(result);
	}
}

static bool popOperators(std::stack<Token>& operatorStack, std::stack<MemoryValue>& outputStack, int myPrecedence, CompileState& state)
{
	while (!operatorStack.empty() && (precedence(operatorStack.top(), state) < myPrecedence || (precedence(operatorStack.top(), state) < 50 && precedence(operatorStack.top(), state) == myPrecedence))) {
		if (!state.errorList.empty())
		{
			return false;
		}

		applyOperator(operatorStack.top(), outputStack, state);
		operatorStack.pop();

		if (!state.errorList.empty())
		{
			return false;
		}
	}
	if (!state.errorList.empty())
	{
		return false;
	}
	return true;
}

MemoryValue ExpressionParser::parse(const std::vector<Token>& tokenList, CompileState& state)
{
	std::stack<Token> operatorStack;
	std::stack<MemoryValue> outputStack;
	Token last = Token(TokenType::INVALID, "");
	for (const auto& i : tokenList)
	{
		switch (i.first)
		{
		case TokenType::NUMBER:
			outputStack.push(std::stoi(i.second));
			break;
		case TokenType::OPERATOR:
		{
			Token operatorToken(i);

			if (!(last.first == TokenType::RIGHT_BRACKET || last.first == TokenType::NUMBER))
			{
				operatorToken.first = TokenType::UNARY_OPERATOR;
			}

			int myPrecedence = precedence(operatorToken, state);
			if (!popOperators(operatorStack, outputStack, myPrecedence, state))
			{
				return MemoryValue(0);
			}

			operatorStack.push(operatorToken);
		}
			break;
		case TokenType::LEFT_BRACKET:
			operatorStack.push(i);
			break;
		case TokenType::RIGHT_BRACKET:
		{
			int myPrecedence = 100;
			
			if (!popOperators(operatorStack, outputStack, myPrecedence, state))
			{
				return MemoryValue(0);
			}

			if (operatorStack.empty())
			{
				state.errorList.push_back(CompileError("Error.z", state.position, CompileErrorSeverity::ERROR));
				return MemoryValue(0);
			}
			operatorStack.pop();
		}
			break;
		case TokenType::LEFT_TERNARY:
			if (!popOperators(operatorStack, outputStack, 100, state))
			{
				return MemoryValue(0);
			}
			operatorStack.push(i);
			break;
		case TokenType::RIGHT_TERNARY:
			if (!popOperators(operatorStack, outputStack, 100, state))
			{
				return MemoryValue(0);
			}

			if (operatorStack.empty() || operatorStack.top().first != TokenType::LEFT_TERNARY)
			{
				state.errorList.push_back(CompileError("Error.x", state.position, CompileErrorSeverity::ERROR));
				return MemoryValue(0);
			}

			break;
		default:
			state.errorList.push_back(CompileError("Unrecognized token: " + i.second, state.position, CompileErrorSeverity::ERROR));
			return MemoryValue(0);
		}

		last = i;

		//state.errorList.push_back(CompileError(std::to_string(operatorStack.size()) + "; " + std::to_string(outputStack.size()), state.position, CompileErrorSeverity::INFORMATION));
		std::cout << "OP:";
		std::stack<Token> tos;
		while (!operatorStack.empty())
		{
			auto x = operatorStack.top();
			operatorStack.pop();
			std::cout << x.second << (int)x.first << ";";
			tos.push(x);
		}
		while (!tos.empty())
		{
			operatorStack.push(tos.top());
			tos.pop();
		}

		std::cout << ";;; OU:";
		std::stack<MemoryValue> tou;
		while (!outputStack.empty())
		{
			auto x = outputStack.top();
			outputStack.pop();
			std::cout << std::to_string((long long)x) << ";";
			tou.push(x);
		}
		while (!tou.empty())
		{
			outputStack.push(tou.top());
			tou.pop();
		}
		std::cout << std::endl;
	}

	if (!popOperators(operatorStack, outputStack, 200, state))
	{
		return MemoryValue(0);
	}

	std::cout << "OP:";
		std::stack<Token> tos;
		while (!operatorStack.empty())
		{
			auto x = operatorStack.top();
			operatorStack.pop();
			std::cout << x.second << (int)x.first << ";";
			tos.push(x);
		}
		while (!tos.empty())
		{
			operatorStack.push(tos.top());
			tos.pop();
		}

		std::cout << ";;; OU:";
		std::stack<MemoryValue> tou;
		while (!outputStack.empty())
		{
			auto x = outputStack.top();
			outputStack.pop();
			std::cout << std::to_string((long long)x) << ";";
			tou.push(x);
		}
		while (!tou.empty())
		{
			outputStack.push(tou.top());
			tou.pop();
		}
		std::cout << std::endl;

	if (outputStack.size() != 1)
	{
		state.errorList.push_back(CompileError("Parsing error.", state.position, CompileErrorSeverity::ERROR));
		return MemoryValue(0);
	}

	return outputStack.top();
}
