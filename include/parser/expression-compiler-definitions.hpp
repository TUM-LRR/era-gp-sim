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

#include <functional>
#include <string>
#include <vector>
#include "parser/compile-state.hpp"

// Personal note: it might have been good to program this thing with usage of
// results/optionals.

/**
 * \brief Denotes the type of an expression token in which a text is broken down
 * to.
 */
enum class ExpressionTokenType {
  /**
   * \brief An auxiliary type denoting a wrongly-encoded token (or a non
   * existing one when nothing has been read so far).
   */
  INVALID,

  /**
   * \brief An operator token. We do not differ between unary and binary
   * operator here, yet.
   */
  OPERATOR,

  /**
   * \brief A literal token which is transformed by operators.
   */
  LITERAL,

  /**
   * \brief The left bracket.
   */
  LEFT_BRACKET,

  /**
   * \brief The right bracket.
   */
  RIGHT_BRACKET
};

/**
 * \brief Denotes the associativity of an operator.
 *
 * If an operator is left-associative, we evaluate our expression from left to
 * right.
 * If it is right-associative, we do the exact opposite, but this requires of
 * course knowledge of the right side first.
 */
enum class ExpressionOperatorAssociativity {
  /**
   * \brief The operator is left-associative.
   */
  LEFT,

  /**
   * \brief The operator is right-associative.
   */
  RIGHT
};

/**
 * \brief Represents a binary operator of an arithmetic expression.
 * \tparam T The number type we operate on.
 */
template <typename T>
struct ExpressionBinaryOperator {
  /**
   * \brief The associativity of the operator.
   */
  ExpressionOperatorAssociativity associativity;

  /**
   * \brief The string representation of the operator.
   */
  std::string identifier;

  /**
   * \brief The precedence of the operator.
   *
   * The lower, the earlier it is evaluated.
   */
  unsigned short precedence;

  /**
   * \brief Applies the operator on two numbers and records any possible errors.
   */
  std::function<bool(const T&, const T&, T&, CompileState&)> handler;
};

/**
 * \brief Represents a unary operator of an arithmetic expression.
 * \tparam T The number type we operate on.
 *
 * For this specific algorithm, unary operators always have the same low
 * precedence which is lower than for any binary operator.
 */
template <typename T>
struct ExpressionUnaryOperator {
  /**
   * \brief The string representation of the operator.
   */
  std::string identifier;

  /**
   * \brief Applies the operator on one number and records any possible errors.
   */
  std::function<bool(const T&, T&, CompileState&)> handler;
};

/**
 * \brief Provides information about how to decode a specific literal.
 * \tparam T The number type we operate on.
 */
template <typename T>
struct ExpressionLiteralDecoder {
  /**
   * \brief The regex representation of the literal.
   */
  std::string regex;

  /**
   * \brief Decodes the literal into the number type.
   */
  std::function<bool(std::string, T&, CompileState&)> decoder;
};

/**
 * \brief Provides some help regexes for tokenizing expressions.
 */
struct ExpressionHelpRegexes {
  /**
   * \brief The regex used for parsing opening brackets.
   */
  std::string leftBracket;

  /**
   * \brief The regex used for parsing closing brackets.
   */
  std::string rightBracket;
};

/**
 * \brief Provides information about how to parse specific symbols.
 * \tparam T The number type we operate on.
 */
template <typename T>
struct ExpressionParserDefinition {
  /**
   * \brief The list of supported binary operators.
   */
  std::vector<ExpressionBinaryOperator<T>> binaryOperators;

  /**
   * \brief The list of supported unary operators.
   */
  std::vector<ExpressionUnaryOperator<T>> unaryOperators;

  /**
   * \brief The list of supported literals.
   */
  std::vector<ExpressionLiteralDecoder<T>> literalDecoders;
};


/**
 * \brief Provides information about how to compile specific expressions.
 * \tparam T The number type we operate on.
 */
template <typename T>
struct ExpressionCompilerDefinition {
  /**
   * \brief Provides information about how to parse the expressions.
   */
  ExpressionParserDefinition<T> parserDefinition;

  /**
   * \brief Additional helper regexes for tokenizing.
   */
  ExpressionHelpRegexes helpers;
};

/**
 * \brief The definition of a token for the tokenizer.
 */
struct ExpressionTokenDefinition {
  /**
   * \brief The regex how to recognize the token.
   */
  std::string regex;

  /**
   * \brief The type the token has.
   */
  ExpressionTokenType type;
};

/**
 * \brief A token, taken from a string.
 */
struct ExpressionToken {
  /**
   * \brief The string representation of the token.
   */
  std::string data;

  /**
   * \brief The type of the token.
   */
  ExpressionTokenType type;

  /**
   * \brief The index where the token is placed in the string.
   */
  size_t index;
};

#endif /* ERAGPSIM_PARSER_EXPRESSION_COMPILER_DEFINITIONS_HPP */
