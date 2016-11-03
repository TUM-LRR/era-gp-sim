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

#ifndef ERAGPSIM_PARSER_INT_PARSER_HPP
#define ERAGPSIM_PARSER_INT_PARSER_HPP

#include <cctype>
#include <limits>
#include <stdexcept>
#include <string>
#include "common/assert.hpp"
#include "parser/compile-state.hpp"

/**
 * Provides methods for integer parsing.
 *
 * \tparam T Number type to parse into
 */
template <typename T>
class IntegerParser {
 public:
  /**
   * Parses from a string to an integer.
   *
   * Tries to parse the whole string. Adds an error to \p state if the parsing
   * fails.
   *
   * \param str   String to parse
   * \param state The current CompileState
   * \param start Start position in the string
   * \param base  Integer based used for parsing. Autodetects base 8, 16 and 10
   *              based on prefix if set to 0
   *
   * \return The parsed integer. Undefined if parsing fails.
   */
  static T parse(const std::string &str,
                 CompileState &state,
                 size_t start = 0,
                 int base = 10) {
    size_t count;
    T result{};
    try {
      result = parseInternal(str, start, base, count);
      if (count < str.length() - start) throw std::invalid_argument("");
    } catch (std::out_of_range &) {
      state.errorList.emplace_back(
          "Integer out of range.", state.position, CompileErrorSeverity::ERROR);
    } catch (std::invalid_argument &) {
      state.errorList.emplace_back(
          "Integer syntax error.", state.position, CompileErrorSeverity::ERROR);
    }
    return result;
  }

 private:
  /**
   * Internal parser function to be used if no specialization exists. Custom
   * number types that want to use this function need to implement a
   * std::numeric_limits specialization Designed to mimic the specification of
   * std::stoi, std::stoul, etc.
   */
  static T
  parseInternal(const std::string &str, size_t start, int base, size_t &count) {
    assert::that((base == 0 || base >= 2) && base <= 36);

    auto begin = str.begin() + start;
    auto position = begin;
    bool negative = false;
    T value{};

    // Skip all spaces
    while (std::isspace(*position)) {
      ++position;
    }

    // Check sign
    if (*position == '-') {
      negative = true;
      ++position;
    } else if (*position == '+') {
      ++position;
    }

    // Check prefixes
    if (*position == '0') {
      ++position;

      if (*position == 'x' || *position == 'X') {
        if (base == 0 || base == 16) {
          base = 16;
          ++position;// Skip the 'x'
        }
      } else if (base == 0) {
        base = 8;
      }
    }

    if (base == 0) base = 10;

    // `max_value` is the maximum value `value` can have without overflowing
    // when the next digit is added. If `value == max_value` there can still be
    // an overflow if the next digit is greater than max_last_digit.
    // Example: For a signed 8 bit integer - which has a maximum of 127 -
    // max_value would be 12 and max_last_digit would be 7
    T max_value{negative && std::numeric_limits<T>::is_signed
                    ? std::numeric_limits<T>::min()
                    : std::numeric_limits<T>::max()};
    int max_last_digit{static_cast<int>(max_value % base)};
    max_value /= base;
    if (max_value < 0) max_value = -max_value;
    if (max_last_digit < 0) max_last_digit = -max_last_digit;

    for (; position != str.end(); ++position) {
      T oldValue = value;

      char c = *position;
      int digit = -1;
      if (c >= '0' && c <= '9') {
        digit = c - '0';
      } else if (c >= 'a' && c <= 'z') {
        digit = 10 + c - 'a';
      } else if (c >= 'A' && c <= 'Z') {
        digit = 10 + c - 'A';
      }

      // Check if digit is inside range
      if (digit < 0 || digit >= base) {
        break;
      }

      // Check for overflow
      if (value > max_value || (value == max_value && digit > max_last_digit)) {
        throw std::out_of_range{"Integer out of range"};
      }

      // Shift value one to the left and add new digit.
      value *= static_cast<T>(base);
      value += static_cast<T>(digit);
    }

    // If no characters were processed, throw exception
    if (position == begin)
      throw std::invalid_argument{"No conversion performed"};

    // Return amount of processed characters
    count = (position - begin);

    if (negative) value = -value;

    return value;
  }
};

// Specializations using the standard conversion methods
template <>
int IntegerParser<int>::parseInternal(const std::string &str,
                                      size_t start,
                                      int base,
                                      size_t &count);

template <>
long IntegerParser<long>::parseInternal(const std::string &str,
                                        size_t start,
                                        int base,
                                        size_t &count);

template <>
long long IntegerParser<long long>::parseInternal(const std::string &str,
                                                  size_t start,
                                                  int base,
                                                  size_t &count);

template <>
unsigned long IntegerParser<unsigned long>::parseInternal(
    const std::string &str, size_t start, int base, size_t &count);

template <>
unsigned long long IntegerParser<unsigned long long>::parseInternal(
    const std::string &str, size_t start, int base, size_t &count);

#endif /* ERAGPSIM_PARSER_INT_PARSER_HPP */
