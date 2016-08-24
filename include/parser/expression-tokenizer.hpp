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

#include "expression-compiler-definitions.hpp"

#include <regex>

class ExpressionTokenizer
{
public:
	ExpressionTokenizer(std::vector<ExpressionTokenDefinition> definitions)
	{
		//TODO: Maybe combine to one giant regex! (w/ groups) If this improves performance...
		_definitions.reserve(definitions.size());
		for (const auto& i : definitions)
		{
			_definitions.push_back(ITokenDefinition{ std::regex("^(" + i.regex + ")"), i.type });
		}
	}

	std::vector<ExpressionToken> tokenize(const std::string& data, CompileState& state) const
	{
		std::vector<ExpressionToken> output;
		size_t index = 0;
		while (index < data.size())
		{
			auto subSpace = data.substr(index);
			std::smatch spaceMatch;
			if (std::regex_search(subSpace, spaceMatch, std::regex("^\\s+")))
			{
				index += spaceMatch.length(0);
			}

			std::smatch match;
			auto sub = data.substr(index);
			bool matched = false;
			for (const auto& i : _definitions)
			{
				if (std::regex_search(sub, match, i.regex))
				{
					output.push_back(ExpressionToken{ match[0].str(), i.type, index });
					index += match.length(0);
					matched = true;
					break;
				}
			}
			if (!matched)
			{
                auto pos = state.position;
                pos.second += index;
				state.errorList.push_back(CompileError("Unrecognized token at: " + sub.substr(0, 20), pos, CompileErrorSeverity::ERROR));
				return output;
			}
		}
		return output;
	}

private:
	struct ITokenDefinition
	{
		std::regex regex;
		ExpressionTokenType type;
	};

	std::vector<ITokenDefinition> _definitions;
};

#endif /* ERAGPSIM_PARSER_EXPRESSION_TOKENIZER_HPP */
