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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_EXPRESSION_COMPILER_DEFINITIONS_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_EXPRESSION_COMPILER_DEFINITIONS_HPP

#include <functional>
#include <string>
#include <vector>
#include "parser/independent/positioned-string.hpp"
class CompileErrorList;

/**
 * Denotes the type of an expression token in which a text is broken down
 * to.
 */
enum class ExpressionTokenType {
  /**
   * An auxiliary type denoting a wrongly-encoded token (or a non
   * existing one when nothing has been read so far).
   */
  INVALID,

  /**
   * An operator token. We do not differ between unary and binary
   * operator here, yet.
   */
  OPERATOR,

  /**
   * A literal token which is transformed by operators.
   */
  LITERAL,

  /**
   * The left bracket.
   */
  LEFT_BRACKET,

  /**
   * The right bracket.
   */
  RIGHT_BRACKET,

  /**
   * A constant. A constant behaves like a literal except that it is replaced in
   * the token stream and tokenized again.
   */
  CONSTANT
};

/**
 * Denotes the associativity of an operator.
 *
 * If an operator is left-associative, we evaluate our expression from left to
 * right.
 * If it is right-associative, we do the exact opposite, but this requires of
 * course knowledge of the right side first.
 */
enum class ExpressionOperatorAssociativity {
  /**
   * The operator is left-associative.
   */
  LEFT,

  /**
   * The operator is right-associative.
   */
  RIGHT
};

/**
 * Represents a binary operator of an arithmetic expression.
 * \tparam T The number type we operate on.
 */
template <typename T>
struct ExpressionBinaryOperator {
  /**
   * The associativity of the operator.
   */
  ExpressionOperatorAssociativity associativity;

  /**
   * The string representation of the operator.
   */
  std::string identifier;

  /**
   * The precedence of the operator.
   *
   * The lower, the earlier it is evaluated.
   */
  unsigned short precedence;

  /**
   * Applies the operator on two numbers and records any possible errors.
   */
  std::function<bool(const T&, const T&, T&, CompileErrorList&)> handler;
};

/**
 * Represents a unary operator of an arithmetic expression.
 * \tparam T The number type we operate on.
 *
 * For this specific algorithm, unary operators always have the same low
 * precedence which is lower than for any binary operator.
 */
template <typename T>
struct ExpressionUnaryOperator {
  /**
   * The string representation of the operator.
   */
  std::string identifier;

  /**
   * Applies the operator on one number and records any possible errors.
   */
  std::function<bool(const T&, T&, CompileErrorList&)> handler;
};

/**
 * Provides information about how to decode a specific literal.
 * \tparam T The number type we operate on.
 */
template <typename T>
struct ExpressionLiteralDecoder {
  /**
   * The regex representation of the literal.
   */
  std::string regex;

  /**
   * Decodes the literal into the number type.
   */
  std::function<bool(const PositionedString&, T&, CompileErrorList&)> decoder;
};

/**
 * Provides some help regexes for tokenizing expressions.
 */
struct ExpressionHelpRegexes {
  /**
   * The regex used for parsing opening brackets.
   */
  std::string leftBracket;

  /**
   * The regex used for parsing closing brackets.
   */
  std::string rightBracket;

  /**
   * The regex used for parsing constants.
   */
  std::string constant;
};

/**
 * Provides information about how to parse specific symbols.
 * \tparam T The number type we operate on.
 */
template <typename T>
struct ExpressionParserDefinition {
  /**
   * The list of supported binary operators.
   */
  std::vector<ExpressionBinaryOperator<T>> binaryOperators;

  /**
   * The list of supported unary operators.
   */
  std::vector<ExpressionUnaryOperator<T>> unaryOperators;

  /**
   * The list of supported literals.
   */
  std::vector<ExpressionLiteralDecoder<T>> literalDecoders;
};


/**
 * Provides information about how to compile specific expressions.
 * \tparam T The number type we operate on.
 */
template <typename T>
struct ExpressionCompilerDefinition {
  /**
   * Provides information about how to parse the expressions.
   */
  ExpressionParserDefinition<T> parserDefinition;

  /**
   * Additional helper regexes for tokenizing.
   */
  ExpressionHelpRegexes helpers;
};

/**
 * The definition of a token for the tokenizer.
 */
struct ExpressionTokenDefinition {
  /**
   * The regex how to recognize the token.
   */
  std::string regex;

  /**
   * The type the token has.
   */
  ExpressionTokenType type;
};

/**
 * A token, taken from a string.
 */
struct ExpressionToken {
  /**
   * The string representation of the token.
   */
  PositionedString data;

  /**
   * The type of the token.
   */
  ExpressionTokenType type;

  /**
   * The index where the token is placed in the string.
   */
  size_t index;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_EXPRESSION_COMPILER_DEFINITIONS_HPP */
