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

#include "parser/expression-compiler-definitions.hpp"
#include "parser/expression-parser.hpp"
#include "parser/expression-tokenizer.hpp"

#include <set>
#include <vector>

/**
 * \brief A compiler for arithmetic expressions.
 * \tparam T The number type for the compiler to operate on.
 *
 * This expression compiler uses the Shunting Yard algorithm as described here:
 * http://wcipeg.com/wiki/Shunting_yard_algorithm
 * Currently this thing supports compiling expressions which have brackets,
 * binary and unary operators, while the binary operators have different
 * precedences (the unary ones do not).
 * The compiler operates in two steps, it first splits the string into tokens
 * (like brackets, literals, operators), then it parses it and evaluates it on
 * the fly (so we're doing syntax and semantic analysis in the same step). It
 * outputs a number, not an RPN or a syntax tree (if we outputted the latter,
 * maybe with some parser clone-classes one could implement maybe full-grown
 * x86, just speculating). How the tokenizer operates is rather simple: it just
 * greedily tries to match the next-best regex for a token type and repeats this
 * procedure until the whole string is tokenized. For the parser, the Shunting
 * Yard algorithm comes in handy. Here, we evaluate the expression using two
 * stacks (one for operators, one for output), so that we can manage different
 * precedences. And that's it. This class takes a definition of a compiler and
 * produces a token definitions list out of it, and also passes the parser
 * definition to the parser itself.
 * Functions and the ternary operators are currently not supported, though
 * possible, because that would lead to some really complicated code with huge
 * switch-cases and that is not what we want (of course, one could implement
 * each token type as a separate class/function struct which is then called,
 * that might make it doable). Also, there is no real practical need for the
 * ternary (we're writing assembler after all, all that could vary are our
 * constants), and functions are probably not needed, though they provide some
 * useful operations sometimes. Probably the best idea to support the two and
 * also stuff like [eax+ebx*8+1234] is to write a full-grown LR parser or
 * something similar, then one may implement this all and must only supply a
 * table with parser data.
 */
template <typename T>
class ExpressionCompiler {
 public:
  /**
   * \brief Creates a new expression compiler based on the definition given.
   * \param definition The given compiler definition.
   */
  ExpressionCompiler(const ExpressionCompilerDefinition<T>& definition)
  : _parser(definition.parserDefinition)
  , _tokenizer(getTokenDefinitions(definition)) {
  }

  /**
   * \brief Creates a new expression compiler with the given tokenizer and
   * parser.
   * \param tokenizer The tokenizer to use.
   * \param parser The parser to use.
   */
  ExpressionCompiler(const ExpressionTokenizer& tokenizer,
                     const ExpressionParser<T>& parser)
  : _tokenizer(tokenizer), _parser(parser) {
  }

  /**
   * \brief Compiles a given string into a number.
   * \param str The given string.
   * \param state The state of the compiler to store any errors.
   * \return The string evaluated into a number or an error value (mostly 0).
   */
  T compile(const std::string& str, CompileState& state) const {
    // Pretty simple: Just pass the string and the tokens in the tokenizer and
    // parser respectively.
    auto tokens = _tokenizer.tokenize(str, state);
    return _parser.parse(tokens, state);
  }

 private:
  // This method generates a token definition list out of a compiler definition.
  std::vector<ExpressionTokenDefinition>
  getTokenDefinitions(const ExpressionCompilerDefinition<T>& definition) {
    std::vector<ExpressionTokenDefinition> tokens;

    // We put unary and binary operators into one token together.
    std::set<std::string> operatorStrings;
    for (const auto& i : definition.parserDefinition.binaryOperators) {
      operatorStrings.insert(i.identifier);
    }
    for (const auto& i : definition.parserDefinition.unaryOperators) {
      operatorStrings.insert(i.identifier);
    }

    // But still, we need to escape the tokens, as they are probably also using
    // characters which are occupied by regex functions.
    std::string operatorRegex;
    for (auto it = operatorStrings.rbegin(); it != operatorStrings.rend();
         ++it) {
      operatorRegex += "|(?:";
      operatorRegex += std::regex_replace(
          *it,
          std::regex(
              "(\\/|\\^|\\$|\\\\|\\.|\\*|\\+|\\?|\\(|\\)|\\[|\\]|\\{|\\}|\\|)"),
          "\\$1");
      operatorRegex += ")";
    }

    // Then, we have the operator token done.
    tokens.push_back(ExpressionTokenDefinition{operatorRegex.substr(1),
                                               ExpressionTokenType::OPERATOR});

    // For the literal decoder tokens, it is simpler, as they are already
    // regexes.
    std::string literalRegex;
    for (const auto& i : definition.parserDefinition.literalDecoders) {
      literalRegex += "|(?:" + i.regex + ")";
    }

    // Just concat them and we are done.
    tokens.push_back(ExpressionTokenDefinition{literalRegex.substr(1),
                                               ExpressionTokenType::LITERAL});

    // The helper regexes we simply add to our token definition list.
    tokens.push_back(ExpressionTokenDefinition{
        definition.helpers.leftBracket, ExpressionTokenType::LEFT_BRACKET});
    tokens.push_back(ExpressionTokenDefinition{
        definition.helpers.rightBracket, ExpressionTokenType::RIGHT_BRACKET});

    // And that's it!
    return tokens;
  }

  // Here, we got our two "bricks" for converting the strings to numbers.
  ExpressionTokenizer _tokenizer;
  ExpressionParser<T> _parser;
};

#endif /* ERAGPSIM_PARSER_EXPRESSION_COMPILER_HPP */
