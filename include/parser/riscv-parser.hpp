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

#ifndef ERAGPSIM_PARSER_RISCV_PARSER_HPP_
#define ERAGPSIM_PARSER_RISCV_PARSER_HPP_

#include <memory>
#include <string>
#include <vector>

#include "arch/common/node-factory-collection.hpp"

#include "parser/compile-state.hpp"
#include "parser/parser.hpp"

/**
 * Risc-V Parser Class
 *
 * This class parses Risc-V instructions into an abstract syntax tree.
 */
class RiscvParser : public Parser {
 public:
  class RiscvRegex;

  RiscvParser(const Architecture &architecture);

  virtual FinalRepresentation
  parse(const std::string &text, ParserMode parserMode);

 protected:
  /**
   * Saves an internal compile state.
   */
  CompileState _compile_state;
  NodeFactoryCollection _factory_collection;
};

#endif// ERAGPSIM_PARSER_RISCV_PARSER_HPP_