/*
* ERASIM Assembler Interpreter
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

#ifndef ERASIM_PARSER_INDEPENDENT_EXPRESSION_COMPILER_CLIKE_HPP
#define ERASIM_PARSER_INDEPENDENT_EXPRESSION_COMPILER_CLIKE_HPP

#include <vector>

#include "common/translateable.hpp"
#include "parser/common/compile-error-list.hpp"
#include "parser/independent/expression-compiler.hpp"
#include "parser/independent/integer-parser.hpp"
#include "parser/independent/string-parser.hpp"

#define CLIKE_RECORD_ERROR(errorList, position, msg, ...)          \
  errorList.pushErrorInternal(                                     \
      position, QT_TRANSLATE_NOOP("Expression Parser Error", msg), \
      ##__VA_ARGS__)

#define FIX_SOME_SYNTAX_HIGHLIGHTING  // Yes, that's the only reason this macro
                                      // exists.

/**
 * Provides some basic expression compilers.
 */
namespace CLikeExpressionCompilers {

static constexpr ExpressionOperatorAssociativity LEFT =
    ExpressionOperatorAssociativity::LEFT;
static constexpr ExpressionOperatorAssociativity RIGHT =
    ExpressionOperatorAssociativity::RIGHT;

// unnamed namespace to hide helper functions
namespace {
template <typename IntType>
using BinOp = ExpressionBinaryOperator<IntType>;
template <typename IntType>
using BinOpImpl = std::function<IntType(const IntType&, const IntType&)>;
template <typename IntType>
using UnOp = ExpressionUnaryOperator<IntType>;
template <typename IntType>
using UnOpImpl = std::function<IntType(const IntType&)>;

/**
*Creates a new binary operator using the given associativity, precedence, symbol
* and behavioural implementation
*/
template <typename IntType>
BinOp<IntType> binaryOperator(ExpressionOperatorAssociativity assoc,
                              const std::string& op, std::uint16_t precedence,
                              const BinOpImpl<IntType>& impl) {
  return BinOp<IntType>{
      assoc, op, precedence,
      [impl](const IntType& first, const IntType& second, IntType& out,
             CompileErrorList& errors, const CodePositionInterval& pos) {
        out = impl(first, second);
        return true;
      }};
}

/**
 * Creates a new unary operator using the given symbol and behavioural
 * implementation
 */
template <typename IntType>
UnOp<IntType> unaryOperator(const std::string& op,
                            const UnOpImpl<IntType>& impl) {
  return UnOp<IntType>{
      op, [impl](const IntType& value, IntType& out, CompileErrorList& errors,
                 const CodePositionInterval& pos) {
        out = impl(value);
        return true;
      }};
}
}
/**
 * Creates a default expression compiler, oriented at the C language.
 *
 * \tparam IntType The given integer type.
 * \return A C-like compiler for the given integer type.
 */
template <typename IntType>
ExpressionCompiler<IntType> createCLikeCompiler() {
  std::vector<BinOp<IntType>> binaryOperators = std::vector<BinOp<IntType>>{
      binaryOperator<IntType>(LEFT, "||", 120,
                              [](auto f, auto s) { return f || s ? 1 : 0; }),
      binaryOperator<IntType>(LEFT, "&&", 110,
                              [](auto f, auto s) { return f && s ? 1 : 0; }),
      binaryOperator<IntType>(LEFT, "|", 100,
                              [](auto f, auto s) { return f | s; }),
      binaryOperator<IntType>(LEFT, "^", 90,
                              [](auto f, auto s) { return f ^ s; }),
      binaryOperator<IntType>(LEFT, "&", 80,
                              [](auto f, auto s) { return f & s; }),
      binaryOperator<IntType>(LEFT, "==", 70,
                              [](auto f, auto s) { return f == s ? 1 : 0; }),
      binaryOperator<IntType>(LEFT, "!=", 70,
                              [](auto f, auto s) { return f != s ? 1 : 0; }),
      binaryOperator<IntType>(LEFT, "<", 60,
                              [](auto f, auto s) { return f < s ? 1 : 0; }),
      binaryOperator<IntType>(LEFT, "<=", 60,
                              [](auto f, auto s) { return f <= s ? 1 : 0; }),
      binaryOperator<IntType>(LEFT, ">", 60,
                              [](auto f, auto s) { return f > s ? 1 : 0; }),
      binaryOperator<IntType>(LEFT, ">=", 60,
                              [](auto f, auto s) { return f >= s ? 1 : 0; }),
      binaryOperator<IntType>(LEFT, "<<", 50,
                              [](auto f, auto s) { return f << s; }),
      binaryOperator<IntType>(LEFT, ">>", 50,
                              [](auto f, auto s) { return f >> s; }),
      binaryOperator<IntType>(LEFT, "+", 40,
                              [](auto f, auto s) { return f + s; }),
      binaryOperator<IntType>(LEFT, "-", 40,
                              [](auto f, auto s) { return f - s; }),
      binaryOperator<IntType>(LEFT, "*", 30,
                              [](auto f, auto s) { return f * s; }),
      ExpressionBinaryOperator<IntType>{
          ExpressionOperatorAssociativity::LEFT, "/", 30,
          [](const IntType& f, const IntType& s, IntType& out,
             CompileErrorList& errors, const CodePositionInterval& pos) {
            if (s == 0) {
              CLIKE_RECORD_ERROR(errors, pos, "Division by zero!");
              return false;
            }
            out = f / s;
            return true;
          }},
      ExpressionBinaryOperator<IntType>{
          ExpressionOperatorAssociativity::LEFT, "%", 30,
          [](const IntType& f, const IntType& s, IntType& out,
             CompileErrorList& errors, const CodePositionInterval& pos) {
            if (s == 0) {
              CLIKE_RECORD_ERROR(errors, pos, "Division by zero!");
              return false;
            }
            out = f % s;
            return true;
          }}};
  std::vector<UnOp<IntType>> unaryOperators = std::vector<UnOp<IntType>>{
      unaryOperator<IntType>("+", [](auto v) { return +v; }),
      unaryOperator<IntType>("-",
                             [](auto v) {
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4146)
#endif
                               return -v;
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
                             }),
      unaryOperator<IntType>("!", [](auto v) { return !v ? 1 : 0; }),
      unaryOperator<IntType>("~", [](auto v) { return ~v; })};
  auto literalDecoders = std::vector<ExpressionLiteralDecoder<IntType>>{
      ExpressionLiteralDecoder<IntType>{
          "0x[0-9a-fA-F]+",
          [](const PositionedString& number, IntType& output,
             CompileErrorList& errors) -> bool {
            output = IntegerParser<IntType>::parse(number, errors, 2, 16);
            return true;
          }},
      ExpressionLiteralDecoder<IntType>{
          "0b[01]+",
          [](const PositionedString& number, IntType& output,
             CompileErrorList& errors) -> bool {
            output = IntegerParser<IntType>::parse(number, errors, 2, 2);
            return true;
          }},
      ExpressionLiteralDecoder<IntType>{
          "[0-9]+",
          [](const PositionedString& number, IntType& output,
             CompileErrorList& errors) -> bool {
            output = IntegerParser<IntType>::parse(number, errors, 0, 10);
            return true;
          }},
      ExpressionLiteralDecoder<IntType>{
          "'.*?'", [](const PositionedString& number, IntType& output,
                      CompileErrorList& errors) -> bool {
            std::vector<uint32_t> intermediate;
            if (!StringParser::parseCharacter(number, errors, intermediate)) {
              return false;
            }
            output = (IntType)intermediate[0];
            return true;
          }}};
  auto parserDefinition = ExpressionParserDefinition<IntType>{
      binaryOperators, unaryOperators, literalDecoders};
  auto helpRegexes =
      ExpressionHelpRegexes{"\\(", "\\)", "[_A-Za-z][_A-Za-z0-9]*"};
  auto compilerDefinition =
      ExpressionCompilerDefinition<IntType>{parserDefinition, helpRegexes};
  return ExpressionCompiler<IntType>(compilerDefinition);
}

/**
 * A default compiler for 8-bit wide signed integers.
 */
extern ExpressionCompiler<int8_t> CLikeCompilerI8;

/**
 * A default compiler for 8-bit wide unsigned integers.
 */
extern ExpressionCompiler<uint8_t> CLikeCompilerU8;

/**
 * A default compiler for 16-bit wide signed integers.
 */
extern ExpressionCompiler<int16_t> CLikeCompilerI16;

/**
 * A default compiler for 16-bit wide unsigned integers.
 */
extern ExpressionCompiler<uint16_t> CLikeCompilerU16;

/**
 * A default compiler for 32-bit wide signed integers.
 */
extern ExpressionCompiler<int32_t> CLikeCompilerI32;

/**
 * A default compiler for 32-bit wide unsigned integers.
 */
extern ExpressionCompiler<uint32_t> CLikeCompilerU32;

/**
 * A default compiler for 64-bit wide signed integers.
 */
extern ExpressionCompiler<int64_t> CLikeCompilerI64;

/**
 * A default compiler for 64-bit wide unsigned integers.
 */
extern ExpressionCompiler<uint64_t> CLikeCompilerU64;

}  // namespace CLikeExpressionCompilers

#endif /* ERASIM_PARSER_INDEPENDENT_EXPRESSION_COMPILER_CLIKE_HPP */
