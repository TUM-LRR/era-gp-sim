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

#include <functional>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
#include "common/assert.hpp"
#include "common/utility.hpp"

/**
 * Helper class for matches of the multiregex.
 * \tparam CharT The character type. Must be integral.
 */
template <typename CharT>
struct MultiregexMatch {
  /**
   * The character type, for internal use.
   */
  using CharType = Utility::TypeBarrier<CharT, std::is_integral>;

  /**
   * Internal string type.
   */
  using String = std::basic_string<CharType>;

  /**
   * Creates a new multiregex match with the given arguments.
   * \param source The COMPLETE source string (the relevant part will be cut
   * out).
   * \param position The positition of the match in the string.
   * \param length The length of the match.
   * \param choice The taken choice.
   */
  MultiregexMatch(const String& source = String(),
                  size_t position = 0,
                  size_t length = 0,
                  size_t choice = 0)
  : position(position)
  , length(length)
  , choice(choice)
  , source(source.substr(position, length)) {
  }

  /**
   * The position in the string where the match is located.
   */
  size_t position;

  /**
   * The length of the match.
   */
  size_t length;

  /**
   * The taken choice of the offered ones.
   */
  size_t choice;

  /**
   * The match as string.
   */
  String source;
};

/**
 * A multiregex match for (normally) 1 byte-wide characters.
 */
using MSMatch = MultiregexMatch<char>;

/**
 * A multiregex match for 2 byte-wide characters.
 */
using MS16Match = MultiregexMatch<char16_t>;

/**
 * A multiregex match for 4 byte-wide characters.
 */
using MS32Match = MultiregexMatch<char32_t>;

/**
 * A wrapper which tries to match multiple regexes at the same time.
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
   * The internal character type.
   */
  using CharType = Utility::TypeBarrier<CharT, std::is_integral>;

  /**
   * The internal string type.
   */
  using String = std::basic_string<CharType>;

  /**
   * The internal string iterator type.
   */
  using StringIterator = typename String::const_iterator;

  /**
   * The internal regex type.
   */
  using Regex = std::basic_regex<CharType>;

  /**
   * The internal regex iterator type.
   */
  using RegexIterator = std::regex_iterator<StringIterator>;

  /**
   * The internal regex matches.
   */
  using RegexMatch = std::match_results<StringIterator>;

  using MultiReplaceFunction = std::function<String(std::size_t)>;

  /**
   * Creates a multiregex with the given prefix and suffix.
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
  : _baseGroup(countBrackets(prefix) + 1)
  , _choice()
  , _regex(buildMultiregex(prefix, suffix, choice, flags), flags) {
  }

  /**
   * Creates a multiregex.
   * \param choice A vector of strings which will be put into the regex. Only
   * one of them has to match. There is no order which regex in the list if
   * preferred, this is rather implementation-defined.
   * \param flags Further regex flags passed down to the regex constructor.
   */
  Multiregex(const std::vector<String>& choice,
             std::regex::flag_type flags = std::regex_constants::ECMAScript)
  : Multiregex(String(), String(), choice, flags) {
  }

  /**
   * Tries to match a string at some position and records the output.
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
      for (auto i : Utility::range<size_t>(_baseGroup + 1, match.size())) {
        if ((size_t)match.length(i) == targetLength) {
          // Found one. We set our multimatch and return it.
          // (this should actually never fail, only if something in this
          // implementation has gone wrong)
          assert::that(_choice.find(i) != _choice.end());
          multimatch = MultiregexMatch<CharType>(
              data, match.position(0), match.length(0), _choice.at(i));
          return true;
        }
      }
    }

    return false;
  }

  /**
   * Tries to match a string at some position.
   * \param data The string to match.
   * \return True if and only if the matching succeeds.
   */
  bool search(const String& data) const {
    MultiregexMatch<CharType> multimatch;

    // We do also record the data, but simply do not use nor return it.
    return search(data, multimatch);
  }

  /**
   * Replaced any match in the multi-regex by something, using a replace
   * function for the choice.
   * \param data The string which should be treated.
   * \param replacement The selector for the replacement.
   * \return The string with everything replaced.
   */
  String
  replace(const String& data, const MultiReplaceFunction& replacement) const {
    // This implementation here might be rather inefficient... But it works!
    MultiregexMatch<CharType> multimatch;
    String substring = data;
    String result;

    // We create substrings and search for the next occurence until there is no
    // more.
    while (search(substring, multimatch)) {
      // Then replace.
      auto end = multimatch.position + multimatch.length;
      result += substring.substr(0, multimatch.position);
      result += replacement(multimatch.choice);

      // Continue with the next substring.
      substring = substring.substr(end);
    }

    // Adding up the rest.
    result += substring;
    return result;
  }

 private:
  // Counts the number of opening brackets in the string which denote the start
  // of a regex group.
  size_t countBrackets(const String& string) const {
    // A regex did not work here, b/c negative lookbehind is not implemented.
    size_t count = 0;

    // One time iterate over the complete string.
    for (auto i : Utility::range<size_t>(0, string.size())) {
      // clang-format off
            char prev = i     >= 1             ? string[i - 1] : '\0';
            char next = i + 1 <  string.size() ? string[i + 1] : '\0';
      // clang-format on

      // Conditions that need to be fulfilled:
      //* The bracket must not be escaped.
      //* The bracket must not denote the start of a not-captured
      // group/lookahead.
      if (prev != '\\' && next != '?' && string[i] == '(') {
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
    String regexBuildString = String();

    // We start with our base group.
    size_t id = _baseGroup;
    for (auto i : Utility::range<size_t>(0, choice.size())) {
      // Each regex choice opens a new group.
      ++id;
      _choice[id] = i;
      regexBuildString += String("|(") + choice[i] + String(")");

      // Test-wise, we build the regex to check its syntax.
      std::regex test(choice[i], flags);

      // Then we need to add all opening brackets.
      id += countBrackets(choice[i]);
    }

    // Then we output the regex.
    return prefix + String("(") + regexBuildString.substr(1) + String(")") +
           suffix;
  }

  // Denotes the number of the group which surrounds the choice.
  size_t _baseGroup;

  // A mapping from group to choice number.
  std::unordered_map<size_t, size_t> _choice;

  // The underlying regex.
  Regex _regex;
};

/**
 * A multiregex using (normally) 1 byte-wide characters.
 */
using MSRegex = Multiregex<char>;

/**
 * A multiregex using 2 byte-wide characters.
 */
using MS16Regex = Multiregex<char16_t>;

/**
 * A multiregex using 4 byte-wide characters.
 */
using MS32Regex = Multiregex<char32_t>;

#endif /* ERAGPSIM_COMMON_MULTIREGEX_HPP */
