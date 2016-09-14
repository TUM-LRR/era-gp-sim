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

#ifndef ERAGPSIM_COMMON_MULTIREGEX_HPP
#define ERAGPSIM_COMMON_MULTIREGEX_HPP

#include <regex>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>
#include "common/utility.hpp"

template<typename CharT>
struct MultiregexMatch {
    using CharType = CharT; //TODO: Include guard!
    using String = std::basic_string<CharType>;

    size_t position;
    size_t length;
    size_t choice;
    String source;

    MultiregexMatch(const String& source_, size_t position_, size_t length_, size_t choice_)
        : position(position_), length(length_), choice(choice_), source(source_.substr(position_, length_)) 
    {

    }

    MultiregexMatch()
        : position(0), length(0), choice(0), source("")
    {

    }
};

using MSMatch = MultiregexMatch<char>;
using MS16Match = MultiregexMatch<char16_t>;
using MS32Match = MultiregexMatch<char32_t>;

template<typename CharT>
class Multiregex {
public:
    using CharType = CharT; //TODO: Include guard!
    using String = std::basic_string<CharType>;
    using StringIterator = typename String::const_iterator;
    using Regex = std::basic_regex<CharType>;
    using RegexIterator = std::regex_iterator<StringIterator>;
    using RegexMatch = std::match_results<StringIterator>;

    Multiregex(const String& prefix, const String& suffix, const std::vector<String>& choice, std::regex::flag_type flags = std::regex_constants::ECMAScript)
        : _choice(), _baseGroup(countBrackets(prefix) + 1), _regex(buildMultiregex(prefix, suffix, choice), flags)
    {

    }

    Multiregex(const std::vector<String>& choice, std::regex::flag_type flags = std::regex_constants::ECMAScript)
        : Multiregex("", "", choice, flags)
    {

    }

    bool search(const String& data, MultiregexMatch<CharType>& multimatch) const
    {
        RegexMatch match;
        bool result = std::regex_search(data, match, _regex);

        if (result)
        {
            size_t targetLength = match.length(_baseGroup);
			for (size_t i = _baseGroup + 1; i < match.size(); ++i)
			{
				if (match.length(i) == targetLength)
				{
                    assert(_choice.find(i) != _choice.end());
                    multimatch = MultiregexMatch<CharType>(data, match.position(0), match.length(0), _choice.at(i));
					return true;
				}
			}
        }
        
        return false;
    }

    bool search(const std::basic_string<CharType>& data) const
    {
        MultiregexMatch<CharType> multimatch;
        return search(data, multimatch);
    }
private:
    size_t countBrackets(const String& str) const
    {
        size_t count = 0;
        for (size_t i = 0; i < str.size(); ++i)
        {
            char prev = i - 1 >= 0 ?         str[i - 1] : '\0';
            char next = i + 1 < str.size() ? str[i + 1] : '\0';
            if (prev != '\\' && next != '?' && str[i] == '(')
            {
                ++count;
            }
        }
        return count;
    }

    String buildMultiregex(const String& prefix, const String& suffix, const std::vector<String>& choice)
    {
        String regexBuildString = "";
        size_t id = _baseGroup;
		for (size_t i = 0; i < choice.size(); ++i)
		{
			++id;
            _choice[id] = i;
			regexBuildString += "|(" + choice[i] + ")";
			id += countBrackets(choice[i]);
		}
		return prefix + "(" + regexBuildString.substr(1) + ")" + suffix;
    }

    size_t _baseGroup;
    std::unordered_map<size_t, size_t> _choice;
    Regex _regex;
};

using MSRegex = Multiregex<char>;
using MS16Regex = Multiregex<char16_t>;
using MS32Regex = Multiregex<char32_t>;

#endif /* ERAGPSIM_COMMON_MULTIREGEX_HPP */
