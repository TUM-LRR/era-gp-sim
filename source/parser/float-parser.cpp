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

#include "parser/float-parser.hpp"

template <>
float FloatParser<float>::parseInternal(const std::string &str,
                                        size_t start,
                                        size_t &count) {
  return std::stof(std::string{str, start}, &count);
}

template <>
double FloatParser<double>::parseInternal(const std::string &str,
                                          size_t start,
                                          size_t &count) {
  return std::stod(std::string{str, start}, &count);
}

template <>
long double FloatParser<long double>::parseInternal(const std::string &str,
                                                    size_t start,
                                                    size_t &count) {
  return std::stold(std::string{str, start}, &count);
}
