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

#include "parser/expression-compiler-definitions.hpp"

#include <regex>
#include <unordered_map>
#include "common/multiregex.hpp"

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
  ExpressionTokenizer(const std::vector<ExpressionTokenDefinition>& definitions)
  : _typeMapping()
  , _tokenizeRegex(
        "^", "", buildRegexVector(definitions), std::regex::optimize) {
  }

  /**
     * \brief Tokenizes a given string and records any errors.
   * \param data The string to tokenize.
   * \param state The compile state to note down any errors.
   */
  std::vector<ExpressionToken>
  tokenize(const std::string& data, CompileState& state) const {
    std::vector<ExpressionToken> output;
    MSMatch match;
    size_t currentPosition = 0;
    std::string temp = data;

    // We loop until we cannot match anything any more.
    while (_tokenizeRegex.search(temp, match)) {
      // if match.choice==0 we got some whitespace which we simply ignore.
      if (match.choice > 0) {
        // If not, we got another token.
        output.push_back(ExpressionToken{match.source,
                                         _typeMapping.at(match.choice - 1),
                                         currentPosition + match.position});
      }

      // Now we update our position and the rest string and continue.
      size_t rpos = match.position + match.length;
      temp = temp.substr(rpos);
      currentPosition += rpos;
    }

    if (temp.empty()) {
      // We are done, and the string has been parsed completely.
      return output;
    } else {
      // We are done, but there is an unrecognized token. We return as if the
      // string was empty.
      state.addError("Unrecognized token at: " + temp.substr(0, 20),
                     state.position >> currentPosition);
      return std::vector<ExpressionToken>();
    }
  }

 private:
  // Internal method for constructing the token regexes.
  std::vector<std::string>
  buildRegexVector(const std::vector<ExpressionTokenDefinition>& definitions) {
    std::vector<std::string> output;
    output.reserve(definitions.size() + 1);

    // We copy the regexes from our definitions and also include a whitespace
    // finder.
    output.push_back("\\s+");
    _typeMapping.reserve(definitions.size());
    for (const ExpressionTokenDefinition& i : definitions) {
      // And while doing that, we are setting the regex to token type mapping.
      _typeMapping.push_back(i.type);
      output.push_back(i.regex);
    }
    return output;
  }

  // The regex to token type mapping.
  std::vector<ExpressionTokenType> _typeMapping;

  // The multiregex for tokenizing.
  MSRegex _tokenizeRegex;
};

#endif /* ERAGPSIM_PARSER_EXPRESSION_TOKENIZER_HPP */
