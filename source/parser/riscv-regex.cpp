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

#include "parser/riscv-regex.hpp"

#include "common/assert.hpp"

static const std::regex LINE_REGEX(
    "\\s*"
    "(?:(\\w+)\\s*\\:)?"// Label group
    "\\s*"
    "(([[:alpha:]]+)"// Instruction group
    "\\s*"
    "(\\w+)"// Parameter group
    "\\s*,\\s*"
    "(\\w+)"// Parameter group
    "(?:\\s*,\\s*"
    "(\\w+))?)?"// Parameter group
    "\\s*"
    "(?:;.*)?"// Comment group
    ,
    std::regex_constants::optimize);

RiscvParser::RiscvRegex::RiscvRegex() {
}

void RiscvParser::RiscvRegex::matchLine(const std::string &line) {
  std::regex_match(line, _matches, LINE_REGEX);
}

bool RiscvParser::RiscvRegex::isValid() {
  return !_matches.empty();
}

bool RiscvParser::RiscvRegex::hasLabel() {
  return _matches[1].matched;
}

bool RiscvParser::RiscvRegex::hasInstruction() {
  return _matches[2].matched;
}

std::string RiscvParser::RiscvRegex::getLabel() {
  return _matches[1].str();
}

std::string RiscvParser::RiscvRegex::getInstruction() {
  return _matches[3].str();
}

std::string RiscvParser::RiscvRegex::getParameter(int n) {
  assert::that(n >= 0 && n < getParameterCount());
  assert::that(4 + n < _matches.size());
  return _matches[4 + n];
}

int RiscvParser::RiscvRegex::getParameterCount() {
  return _matches[6].matched ? 3 : 2;
}