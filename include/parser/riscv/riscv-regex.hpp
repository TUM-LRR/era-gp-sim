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

#ifndef ERAGPSIM_PARSER_RISCV_RISCV_REGEX_HPP
#define ERAGPSIM_PARSER_RISCV_RISCV_REGEX_HPP

#include <cstddef>
#include <regex>
#include <string>

#include "parser/riscv/riscv-parser.hpp"

class CompileErrorList;
#include "parser/independent/positioned-string.hpp"

/**
 * Wrapper class for the RegExp used for parsing.
 */
class RiscvParser::RiscvRegex {
 public:
  RiscvRegex();

  /**
   * Returns if the line is a valid Risc-V instruction.
   */
  bool isValid() const noexcept;

  /**
   * Returns if the line contains a label.
   */
  bool hasLabel() const noexcept;

  /**
   * Returns if the line contains an instruction.
   */
  bool hasInstruction() const noexcept;

  /**
   * Returns if the instruction is a parser directive.
   */
  bool isDirective() const;

  /**
   * Returns the parsed label.
   */
  const PositionedString &getLabel() const noexcept;

  /**
   * Returns the parsed instruction.
   */
  PositionedString getInstruction() const;

  /**
   * Returns the n-th parameter of the parsed instruction.
   *
   * \param n Zero-based index of the parameter
   */
  const PositionedString &getParameter(int n) const;

  /**
   * Returns the number of parameters found.
   */
  int getParameterCount() const noexcept;

  /**
   * Parses the next line.
   *
   * \param line  The line to parse.
   * \param lineCoordinate
   * \param errors CompileErrorList to record errors.
   * \return The last read position in the string (if the line does not match,
   * this is the character that caused the error)
   */
  size_t matchLine(const std::string &line,
                   CodeCoordinate lineCoordinate,
                   CompileErrorList &errors);

 private:
  /**
   * Reads an instruction or a label from `line` beginning at `pos`.
   *
   * \param line The given line to parse.
   * \param lineCoordinate The position of the line in code.
   * \param errors The CompileErrorList to record errors.
   * \param pos The position where to start recording errors.
   * \return True, if successful, else false.
   */
  bool _readInstructionOrLabel(const std::string &line,
                               CodeCoordinate lineCoordinate,
                               CompileErrorList &errors,
                               size_t &pos);

  /**
     * Reads a parameter from `line` beginning at `pos`.
     *
     * \param line The given line to parse.
     * \param lineCoordinate The position of the line in code.
     * \param errors The CompileErrorList to record errors.
     * \param pos The position where to start recording errors.
     * \return True, if successful, else false.
     */
  bool _readParameter(const std::string &line,
                      CodeCoordinate lineCoordinate,
                      CompileErrorList &errors,
                      size_t &pos);

  /**
   * Resets this RISC-V regex so it may be used again later.
   */
  void _resetResults();

  /**
   * Creates an interval which covers only this very character.
   *
   * \param lineCoordinate The y-coordinate of the character.
   * \param erros The CompileErrorList to record errors.
   * \param pos The x-coordinate of the character.
   */
  CodePositionInterval _getCharacterPosition(CodeCoordinate lineCoordinate,
                                             CompileErrorList &errors,
                                             size_t pos) const;

  /**
   * Creates an interval which covers an interval of characters.
   *
   * \param lineCoordinate The y-coordinate of the interval.
   * \param erros The CompileErrorList to record errors.
   * \param start The start x-coordinate of the interval.
   * \param end The end x-coordinate of the interval.
   */
  CodePositionInterval _getCharacterInterval(CodeCoordinate lineCoordinate,
                                             CompileErrorList &errors,
                                             size_t start,
                                             size_t end) const;

  /**
   * Creates a positioned string out of the given interval of characters.
   * \param line The string to take the position of.
   * \param lineCoordinate The y-coordinate of the interval.
   * \param erros The CompileErrorList to record errors.
   * \param start The start x-coordinate of the interval.
   * \param end The end x-coordinate of the interval.
   */
  PositionedString _getPositionedString(const std::string &line,
                                        CodeCoordinate lineCoordinate,
                                        CompileErrorList &errors,
                                        size_t start,
                                        size_t end) const;

  bool _isValid;
  PositionedString _label;
  PositionedString _instruction;
  PositionedStringVector _parameters;
};

#endif
