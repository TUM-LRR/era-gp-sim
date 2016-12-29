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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_EXPRESSION_COMPILER_CLIKE_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_EXPRESSION_COMPILER_CLIKE_HPP

#include "common/translateable.hpp"
#include "parser/common/compile-error-list.hpp"
#include "parser/independent/expression-compiler.hpp"
#include "parser/independent/integer-parser.hpp"
#include "parser/independent/string-parser.hpp"

/**
 * \brief Provides some basic expression compilers.
 */
namespace CLikeExpressionCompilers {
/**
 * \brief Creates a default expression compiler, oriented at the C language.
 * \tparam IntType The given integer type.
 * \return A C-like compiler for the given integer type.
 */
template <typename IntType>
ExpressionCompiler<IntType> createCLikeCompiler() {
  auto binaryOperators = std::vector<ExpressionBinaryOperator<IntType>>{
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "||",
                                        120,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out =
                                              f || s ? IntType(1) : IntType(0);
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "&&",
                                        110,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out =
                                              f && s ? IntType(1) : IntType(0);
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "|",
                                        100,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out = f | s;
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "^",
                                        90,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out = f ^ s;
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "&",
                                        80,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out = f & s;
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "==",
                                        70,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out =
                                              f == s ? IntType(1) : IntType(0);
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "!=",
                                        70,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out =
                                              f != s ? IntType(1) : IntType(0);
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "<",
                                        60,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out = f < s ? IntType(1) : IntType(0);
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "<=",
                                        60,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out =
                                              f <= s ? IntType(1) : IntType(0);
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        ">",
                                        60,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out = f > s ? IntType(1) : IntType(0);
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        ">=",
                                        60,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out =
                                              f >= s ? IntType(1) : IntType(0);
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "<<",
                                        50,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out = f << s;
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        ">>",
                                        50,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out = f >> s;
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "+",
                                        40,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out = f + s;
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "-",
                                        40,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out = f - s;
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "*",
                                        30,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out = f * s;
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "/",
                                        30,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out = f / s;
                                          return true;
                                        }},
      ExpressionBinaryOperator<IntType>{ExpressionOperatorAssociativity::LEFT,
                                        "%",
                                        30,
                                        [](const IntType& f,
                                           const IntType& s,
                                           IntType& out,
                                           CompileErrorList& errors) -> bool {
                                          out = f % s;
                                          return true;
                                        }}};
  auto unaryOperators = std::vector<ExpressionUnaryOperator<IntType>>{
      ExpressionUnaryOperator<IntType>{
          "+",
          [](const IntType& v, IntType& out, CompileErrorList& errors) -> bool {
            out = +v;
            return true;
          }},
      ExpressionUnaryOperator<IntType>{
          "-",
          [](const IntType& v, IntType& out, CompileErrorList& errors) -> bool {
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4146)
#endif
            out = -v;
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
            return true;
          }},
      ExpressionUnaryOperator<IntType>{
          "!",
          [](const IntType& v, IntType& out, CompileErrorList& errors) -> bool {
            out = !v ? IntType(1) : IntType(0);
            return true;
          }},
      ExpressionUnaryOperator<IntType>{
          "~",
          [](const IntType& v, IntType& out, CompileErrorList& errors) -> bool {
            out = ~v;
            return true;
          }}};
  auto literalDecoders = std::vector<ExpressionLiteralDecoder<IntType>>{
      ExpressionLiteralDecoder<IntType>{
          "0x[0-9a-fA-F]+",
          [](const PositionedString& number,
             IntType& output,
             CompileErrorList& errors) -> bool {
            output = IntegerParser<IntType>::parse(number, errors, 2, 16);
            return true;
          }},
      ExpressionLiteralDecoder<IntType>{
          "0b[01]+",
          [](const PositionedString& number,
             IntType& output,
             CompileErrorList& errors) -> bool {
            output = IntegerParser<IntType>::parse(number, errors, 2, 2);
            return true;
          }},
      ExpressionLiteralDecoder<IntType>{
          "[0-9]+",
          [](const PositionedString& number,
             IntType& output,
             CompileErrorList& errors) -> bool {
            output = IntegerParser<IntType>::parse(number, errors, 0, 10);
            return true;
          }},
      ExpressionLiteralDecoder<IntType>{
          "'.*?'",
          [](const PositionedString& number,
             IntType& output,
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
  return compilerDefinition;
}

/**
 * \brief A default compiler for 8-bit wide signed integers.
 */
extern ExpressionCompiler<int8_t> CLikeCompilerI8;

/**
 * \brief A default compiler for 8-bit wide unsigned integers.
 */
extern ExpressionCompiler<uint8_t> CLikeCompilerU8;

/**
 * \brief A default compiler for 16-bit wide signed integers.
 */
extern ExpressionCompiler<int16_t> CLikeCompilerI16;

/**
 * \brief A default compiler for 16-bit wide unsigned integers.
 */
extern ExpressionCompiler<uint16_t> CLikeCompilerU16;

/**
 * \brief A default compiler for 32-bit wide signed integers.
 */
extern ExpressionCompiler<int32_t> CLikeCompilerI32;

/**
 * \brief A default compiler for 32-bit wide unsigned integers.
 */
extern ExpressionCompiler<uint32_t> CLikeCompilerU32;

/**
 * \brief A default compiler for 64-bit wide signed integers.
 */
extern ExpressionCompiler<int64_t> CLikeCompilerI64;

/**
 * \brief A default compiler for 64-bit wide unsigned integers.
 */
extern ExpressionCompiler<uint64_t> CLikeCompilerU64;
}

#endif /* ERAGPSIM_PARSER_INDEPENDENT_EXPRESSION_COMPILER_CLIKE_HPP */
