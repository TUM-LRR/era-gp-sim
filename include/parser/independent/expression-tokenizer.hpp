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

#ifndef ERAGPSIM_PARSER_EXPRESSION_TOKENIZER_HPP
#define ERAGPSIM_PARSER_EXPRESSION_TOKENIZER_HPP

#include "parser/independent/expression-compiler-definitions.hpp"

#include <regex>
#include <unordered_map>
#include "common/multiregex.hpp"
#include "parser/independent/positioned-string.hpp"
class CompileErrorList;

/**
 * \brief Transforms a string into a list of tokens.
 *
 * This system uses a single regex which contains code to match all possible
 * tokens (this way, we may emulate parsing using a finite automaton). If we
 * match, we shorten the string and match again and again until nothing is left.
 * If we do not match any token, the string is seen as invalid.
 */
class ExpressionTokenizer {
 public:
  /**
   * \brief Creates a new expression tokenizer out of a token list.
   * \param definitions The token list.
   */
  ExpressionTokenizer(
      const std::vector<ExpressionTokenDefinition>& definitions);

  /**
     * \brief Tokenizes a given string and records any errors.
   * \param data The string to tokenize.
   * \param state The compile state to note down any errors.
   */
  std::vector<ExpressionToken>
  tokenize(const PositionedString& data, CompileErrorList& errors) const;

 private:
  // Internal method for constructing the token regexes.
  std::vector<std::string>
  buildRegexVector(const std::vector<ExpressionTokenDefinition>& definitions);

  // The regex to token type mapping.
  std::vector<ExpressionTokenType> _typeMapping;

  // The multiregex for tokenizing.
  MSRegex _tokenizeRegex;
};

#endif /* ERAGPSIM_PARSER_EXPRESSION_TOKENIZER_HPP */
