/* C++ Assembler Interpreter
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

#ifndef ERAGPSIM_PARSER_FLOAT_PARSER_HPP_
#define ERAGPSIM_PARSER_FLOAT_PARSER_HPP_

#include <stdexcept>
#include <string>
#include "parser/compile-state.hpp"

/**
 * Provides methods for float parsing.
 *
 * \tparam T Number type to parse into
 */
template <typename T>
class FloatParser {
 public:
  /**
   * Parses from a string to a floating point number.
   *
   * Tries to parse the whole string. Adds an error to \p state if the parsing
   * fails.
   *
   * \param str   String to parse
   * \param state The current CopmileState
   * \param start Start position in the string
   *
   * \return The parsed floating point number
   */
  static T
  parse(const std::string &str, CompileState &state, size_t start = 0) {
    size_t count;
    T result{};
    try {
      result = parseInternal(str, start, count);
      if (count < str.length() - start) throw std::invalid_argument("");
    } catch (std::out_of_range &) {
      state.addError("Float out of range", state.position);
    } catch (std::invalid_argument &) {
      state.addError("Float syntax error.", state.position);
    }
    return result;
  }

 private:
  /**
   * Internal parser function to be used if no specialization exists. Currently
   * not implemented.
   */
  static T parseInternal(const std::string &str, size_t start, size_t &count);
};

// Specializations using the standard conversion methods
template <>
float FloatParser<float>::parseInternal(const std::string &str,
                                        size_t start,
                                        size_t &count);

template <>
double FloatParser<double>::parseInternal(const std::string &str,
                                          size_t start,
                                          size_t &count);

template <>
long double FloatParser<long double>::parseInternal(const std::string &str,
                                                    size_t start,
                                                    size_t &count);

#endif// ERAGPSIM_PARSER_FLOAT_PARSER_HPP_
