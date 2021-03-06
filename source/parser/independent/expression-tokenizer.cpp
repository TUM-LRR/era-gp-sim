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

#include "parser/independent/expression-tokenizer.hpp"

#include <cstddef>
#include <string>
#include <vector>

#include "common/multiregex.hpp"
#include "parser/common/compile-error-list.hpp"
#include "parser/independent/symbol-replacer.hpp"

ExpressionTokenizer::ExpressionTokenizer(
    const std::vector<ExpressionTokenDefinition>& definitions)
: _typeMapping()
, _tokenizeRegex("^", "", buildRegexVector(definitions), std::regex::optimize) {
}

std::vector<ExpressionToken>
ExpressionTokenizer::tokenize(const PositionedString& data,
                              CompileErrorList& errors) const {
  std::vector<ExpressionToken> output;
  MSMatch match;
  size_t currentPosition = 0;
  std::string temp = data.string();

  // We loop until we cannot match anything any more.
  while (_tokenizeRegex.search(temp, match)) {
    // if match.choice==0 we got some whitespace which we simply ignore.
    if (match.choice > 0) {
      // If not, we got another token.
      auto positionedSource =
          data.slice(currentPosition + match.position, match.length);
      output.emplace_back(ExpressionToken{positionedSource,
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
    errors.pushError(
        data.slice(currentPosition, data.string().size() - currentPosition)
            .positionInterval(),
        "Unrecognized token at '%1'",
        temp.substr(0, 20));
    return std::vector<ExpressionToken>();
  }
}

std::vector<std::string> ExpressionTokenizer::buildRegexVector(
    const std::vector<ExpressionTokenDefinition>& definitions) {
  std::vector<std::string> output;
  output.reserve(definitions.size() + 1);

  // We copy the regexes from our definitions and also include a whitespace
  // finder.
  output.emplace_back("\\s+");
  _typeMapping.reserve(definitions.size());
  for (const auto& tokenDefinition : definitions) {
    // And while doing that, we are setting the regex to token type mapping.
    _typeMapping.emplace_back(tokenDefinition.type);
    output.emplace_back(tokenDefinition.regex);
  }
  return output;
}
