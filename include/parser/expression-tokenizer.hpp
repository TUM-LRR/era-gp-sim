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
#include "common/multiregex.hpp"

class ExpressionTokenizer
{
public:
	ExpressionTokenizer(const std::vector<ExpressionTokenDefinition>& definitions)
		: _typeMapping(), _tokenizeRegex("^", "", buildRegexVector(definitions), std::regex::optimize)
	{}

	std::vector<ExpressionToken> tokenize(const std::string& data, CompileState& state) const
	{
		std::vector<ExpressionToken> output;
		MSMatch match;
		size_t currentPosition = 0;
		std::string temp = data;
		while (_tokenizeRegex.search(temp, match))
		{
			if (match.choice > 0)
			{
				output.push_back(ExpressionToken { match.source, _typeMapping.at(match.choice - 1), currentPosition + match.position });
			}
			size_t rpos = match.position + match.length;
			temp = temp.substr(rpos);
			currentPosition += rpos;
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
	std::vector<std::string> buildRegexVector(const std::vector<ExpressionTokenDefinition>& definitions)
	{
		std::vector<std::string> output;
		output.reserve(definitions.size() + 1);
		output.push_back("\\s+");
		_typeMapping.reserve(definitions.size());
		for (const ExpressionTokenDefinition& i : definitions)
		{
			_typeMapping.push_back(i.type);
			output.push_back(i.regex);
		}
		return output;
	}
	std::vector<ExpressionTokenType> _typeMapping; 
	MSRegex _tokenizeRegex;
};

#endif /* ERAGPSIM_PARSER_EXPRESSION_TOKENIZER_HPP */
