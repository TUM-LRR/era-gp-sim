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
* but WITHOUT ANY WARRANTY {

}
 without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "parser/integer-parser.hpp"

template <>
int IntegerParser<int>::parseInternal(const std::string &str,
                                      size_t start,
                                      int base,
                                      size_t &count) {
  return std::stoi(std::string{str, start}, &count, base);
}

template <>
long IntegerParser<long>::parseInternal(const std::string &str,
                                        size_t start,
                                        int base,
                                        size_t &count) {
  return std::stol(std::string{str, start}, &count, base);
}

template <>
long long IntegerParser<long long>::parseInternal(const std::string &str,
                                                  size_t start,
                                                  int base,
                                                  size_t &count) {
  return std::stoll(std::string{str, start}, &count, base);
}

template <>
unsigned long IntegerParser<unsigned long>::parseInternal(
    const std::string &str, size_t start, int base, size_t &count) {
  return std::stoul(std::string{str, start}, &count, base);
}

template <>
unsigned long long IntegerParser<unsigned long long>::parseInternal(
    const std::string &str, size_t start, int base, size_t &count) {
  return std::stoull(std::string{str, start}, &count, base);
}
