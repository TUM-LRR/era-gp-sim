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

#ifndef ERAGPSIM_PARSER_PARSER_HPP
#define ERAGPSIM_PARSER_PARSER_HPP

#include <memory>
#include <string>
#include <vector>
#include "parser/final-representation.hpp"
#include "parser/parser-mode.hpp"

class SyntaxInformation;

class Architecture;

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
   * Retrieves information for syntax highlighting.
   *
   * \return Object containing syntax hightlighting information
   */
  virtual const SyntaxInformation getSyntaxInformation() = 0;


  Parser() = default;
  virtual ~Parser() = default;
};

#endif// ERAGPSIM_PARSER_PARSER_HPP
