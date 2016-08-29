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
#include <unordered_map>

class ExpressionTokenizer
{
public:
	ExpressionTokenizer(const std::vector<ExpressionTokenDefinition>& definitions)
		: _typeMapping(), _tokenizeRegex(buildRegexString(definitions), std::regex::optimize)
	{}

	std::vector<ExpressionToken> tokenize(const std::string& data, CompileState& state) const
	{
		std::vector<ExpressionToken> output;
		std::smatch match;
		std::string temp = data;
		while (std::regex_search(temp, match, _tokenizeRegex))
		{
			size_t targetLength = match.length(0);
			for (size_t i = 2; i < match.size(); ++i)
			{
				if (match.length(i) == targetLength)
				{
					if (i != 2)
					{
						output.push_back(ExpressionToken { match.str() , _typeMapping.at(i) });
					}
					break;
				}
			}
			temp = match.suffix().str();
		}
		if (temp.empty())
		{
			return output;
		}
		else
		{
			//TODO: Make a bit more beautiful
			CodePosition position = state.position;
			position.second += data.length() - temp.length();
			state.errorList.push_back(CompileError("Unrecognized token at: " + temp.substr(0, 20), position, CompileErrorSeverity::ERROR));
			return std::vector<ExpressionToken>();
		}
	}

private:
	std::string buildRegexString(const std::vector<ExpressionTokenDefinition>& definitions)
	{
		std::string regexBuildString = "(\\s+)";
		int id = 2;
		for (const auto& i : definitions)
		{
			++id;
			_typeMapping[id] = i.type;
			regexBuildString += "|(" + i.regex + ")";
			std::regex groupCount("(^|[^\\\\])\\(([^\\?]|$)");
			id += std::distance(std::sregex_iterator(i.regex.begin(), i.regex.end(), groupCount), std::sregex_iterator());
		}
		return "^(" + regexBuildString + ")";
	}

	std::unordered_map<int, ExpressionTokenType> _typeMapping; 
	std::regex _tokenizeRegex;
};

#endif /* ERAGPSIM_PARSER_EXPRESSION_TOKENIZER_HPP */
