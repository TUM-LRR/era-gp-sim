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
#include <unordered_map>
#include <vector>
#include "common/assert.hpp"
#include "common/utility.hpp"

/**
 * \brief Helper class for matches of the multiregex.
 * \tparam CharT The character type. Must be integral.
 */
template <typename CharT>
struct MultiregexMatch {
  /**
   * \brief The character type, for internal use.
   */
  using CharType = Utility::TypeBarrier<CharT, std::is_integral>;

  /**
   * \brief Internal string type.
   */
  using String = std::basic_string<CharType>;

  /**
   * \brief The position in the string where the match is located.
   */
  size_t position;

  /**
   * \brief The length of the match.
   */
  size_t length;

  /**
   * \brief The taken choice of the offered ones.
   */
  size_t choice;

  /**
   * \brief The match as string.
   */
  String source;

  /**
   * \brief Creates a new multiregex match with the given arguments.
   * \param source_ The COMPLETE source string (the relevant part will be cut
   * out).
   * \param position_ The positition of the match in the string.
   * \param length_ The length of the match.
   * \param choice_ The taken choice.
   */
  MultiregexMatch(const String& source_,
                  size_t position_,
                  size_t length_,
                  size_t choice_)
  : position(position_)
  , length(length_)
  , choice(choice_)
  , source(source_.substr(position_, length_)) {
  }

  /**
   * \brief Creates an empty multiregex match.
   */
  MultiregexMatch() : position(0), length(0), choice(0), source("") {
  }
};

/**
 * \brief A multiregex match for (normally) 1 byte-wide characters.
 */
using MSMatch = MultiregexMatch<char>;

/**
 * \brief A multiregex match for 2 byte-wide characters.
 */
using MS16Match = MultiregexMatch<char16_t>;

/**
 * \brief A multiregex match for 4 byte-wide characters.
 */
using MS32Match = MultiregexMatch<char32_t>;

/**
 * \brief A wrapper which tries to match multiple regexes at the same time.
 * \tparam CharT The character type used for strings and the regex. Must be
 * integral.
 *
 * Please note that the regexes are not in any order, this is
 * implementation-defined! Yet, it is sometimes useful to put the smaller
 * regexes after the bigger ones.
 */
template <typename CharT>
class Multiregex {
  // Note: if you want to break this class, you will probably manage to do so...
 public:
  /**
   * \brief The internal character type.
   */
  using CharType = Utility::TypeBarrier<CharT, std::is_integral>;

  /**
   * \brief The internal string type.
   */
  using String = std::basic_string<CharType>;

  /**
   * \brief The internal string iterator type.
   */
  using StringIterator = typename String::const_iterator;

  /**
   * \brief The internal regex type.
   */
  using Regex = std::basic_regex<CharType>;

  /**
   * \brief The internal regex iterator type.
   */
  using RegexIterator = std::regex_iterator<StringIterator>;

  /**
   * \brief The internal regex matches.
   */
  using RegexMatch = std::match_results<StringIterator>;

  /**
   * \brief Creates a multiregex with the given prefix and suffix.
   * \param prefix The prefix is mandatory to match and put before the choice.
   * \param suffix The suffix is mandatory to match and put after the choice.
   * \param choice A vector of strings which will be put into the regex. Only
   * one of them has to match. There is no order which regex in the list if
   * preferred, this is rather implementation-defined.
   * \param flags Further regex flags passed down to the regex constructor.
   */
  Multiregex(const String& prefix,
             const String& suffix,
             const std::vector<String>& choice,
             std::regex::flag_type flags = std::regex_constants::ECMAScript)
  : _choice()
  , _baseGroup(countBrackets(prefix) + 1)
  , _regex(buildMultiregex(prefix, suffix, choice, flags), flags) {
  }

  /**
   * \brief Creates a multiregex.
   * \param choice A vector of strings which will be put into the regex. Only
   * one of them has to match. There is no order which regex in the list if
   * preferred, this is rather implementation-defined.
   * \param flags Further regex flags passed down to the regex constructor.
   */
  Multiregex(const std::vector<String>& choice,
             std::regex::flag_type flags = std::regex_constants::ECMAScript)
  : Multiregex("", "", choice, flags) {
  }

  /**
   * \brief Tries to match a string at some position and records the output.
   * \param data The string to match.
   * \param multimatch The multimatch to output the success.
   * \return True if and only if the matching succeeds.
   */
  bool search(const String& data, MultiregexMatch<CharType>& multimatch) const {
    RegexMatch match;

    // First of all, we search using the internal regex.
    bool result = std::regex_search(data, match, _regex);

    if (result) {
      // If we have matched, we look for the first group inside (or after, does
      // not matter, one group will have been matched (hopefully)) which has the
      // full length of the complete group. For this one we know that it must be
      // a group wrapping one of our choice regexes, b/c if not, there would
      // have been another group as big before. So we get always the first
      // matching group.
      size_t targetLength = match.length(_baseGroup);
      for (size_t i = _baseGroup + 1; i < match.size(); ++i) {
        if (match.length(i) == targetLength) {
          // Found one. We set our multimatch and return it.
          assert::that(_choice.find(i) != _choice.end());//(this should actually
                                                         //never fail, only if
                                                         //something in this
                                                         //implementation has
                                                         //gone wrong)
          multimatch = MultiregexMatch<CharType>(
              data, match.position(0), match.length(0), _choice.at(i));
          return true;
        }
      }
    }

    return false;
  }

  /**
   * \brief Tries to match a string at some position.
   * \param data The string to match.
   * \return True if and only if the matching succeeds.
   */
  bool search(const std::basic_string<CharType>& data) const {
    MultiregexMatch<CharType> multimatch;

    // We do also record the data, but simply do not use nor return it.
    return search(data, multimatch);
  }

 private:
  // Counts the number of opening brackets in the string which denote the start
  // of a regex group.
  size_t countBrackets(const String& str) const {
    // A regex did not work here, b/c negative lookbehind is not implemented.
    size_t count = 0;

    // One time iterate over the complete string.
    for (size_t i = 0; i < str.size(); ++i) {
      // clang-format off
            char prev = i     >= 1         ? str[i - 1] : '\0';
            char next = i + 1 < str.size() ? str[i + 1] : '\0';
      // clang-format on

      // Conditions that need to be fulfilled:
      //* The bracket must not be escaped.
      //* The bracket must not denote the start of a not-captured
      //group/lookahead.
      if (prev != '\\' && next != '?' && str[i] == '(') {
        ++count;
      }
    }
    return count;
  }

  // Creates the multiregex regex.
  String buildMultiregex(const String& prefix,
                         const String& suffix,
                         const std::vector<String>& choice,
                         std::regex::flag_type flags) {
    // The choice regex string.
    String regexBuildString = "";

    // We start with our base group.
    size_t id = _baseGroup;
    for (size_t i = 0; i < choice.size(); ++i) {
      // Each regex choice opens a new group.
      ++id;
      _choice[id] = i;
      regexBuildString += "|(" + choice[i] + ")";

      // Test-wise, we build the regex to check its syntax.
      std::regex test(choice[i], flags);

      // Then we need to add all opening brackets.
      id += countBrackets(choice[i]);
    }

    // Then we output the regex.
    return prefix + "(" + regexBuildString.substr(1) + ")" + suffix;
  }

  // Denotes the number of the group which surrounds the choice.
  size_t _baseGroup;

  // A mapping from group to choice number.
  std::unordered_map<size_t, size_t> _choice;

  // The underlying regex.
  Regex _regex;
};

/**
 * \brief A multiregex using (normally) 1 byte-wide characters.
 */
using MSRegex = Multiregex<char>;

/**
 * \brief A multiregex using 2 byte-wide characters.
 */
using MS16Regex = Multiregex<char16_t>;

/**
 * \brief A multiregex using 4 byte-wide characters.
 */
using MS32Regex = Multiregex<char32_t>;

#endif /* ERAGPSIM_COMMON_MULTIREGEX_HPP */
