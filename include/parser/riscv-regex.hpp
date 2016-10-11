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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.*/

#ifndef ERAGPSIM_PARSER_RISCV_REGEX_HPP_
#define ERAGPSIM_PARSER_RISCV_REGEX_HPP_

#include <regex>
#include <string>

#include "parser/riscv-parser.hpp"

/**
 * Wrapper class for the RegExp used for parsing.
 */
class RiscvParser::RiscvRegex {
 public:
  RiscvRegex();

  /**
   * Returns if the line is a valid Risc-V instruction.
   */
  bool isValid();

  /**
   * Returns if the line contains a label.
   */
  bool hasLabel();

  /**
   * Returns if the line contains an instruction.
   */
  bool hasInstruction();

  /**
   * Returns the parsed label.
   */
  std::string getLabel();

  /**
   * Returns the parsed instruction.
   */
  std::string getInstruction();

  /**
   * Returns the n-th parameter of the parsed instruction.
   *
   * \param n Zero-based index of the parameter
   */
  std::string getParameter(int n);

  /**
   * Returns the number of parameters found.
   */
  int getParameterCount();

  /**
   * Parses the next line.
   *
   * \param line The line to parse
   */
  void matchLine(const std::string &line);

 private:
  std::smatch _matches;
};

#endif