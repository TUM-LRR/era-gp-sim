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

#ifndef ERAGPSIM_PARSER_PARSER_HPP_
#define ERAGPSIM_PARSER_PARSER_HPP_

#include <memory>
#include <string>
#include <vector>
#include "parser/final-representation.hpp"
#include "parser/parser-mode.hpp"


/**
 * Base Parser class
 */
class Parser {
 public:
  /**
   * Parses text into syntax tree.
   *
   * \param text Text to parse
   * \param parserMode Parser Mode
   */
  virtual FinalRepresentation
  parse(const std::string &text, ParserMode parserMode) = 0;

  /**
   * Creates dialect-specific Regex for syntax highlighting registers.
   *
   * \param name Register name
   * \return Dialect-specific Regex
   */
  virtual std::string getSyntaxRegister(const std::string &name);

  /**
   * Creates dialect-specific Regex for syntax highlighting instructions.
   *
   * \param name Assembler instruction name
   * \return Dialect-specific Regex
   */
  virtual std::string getSyntaxInstruction(const std::string &name);

  /**
   * Creates dialect-specific Regex for syntax highlighting immediates.
   *
   * \return Dialect-specific Regex
   */
  virtual std::string getSyntaxImmediate();


  Parser()          = default;
  virtual ~Parser() = default;
};

#endif// ERAGPSIM_PARSER_PARSER_HPP_
