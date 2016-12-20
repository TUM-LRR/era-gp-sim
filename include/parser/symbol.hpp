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

#ifndef ERAGPSIM_PARSER_SYMBOL_HPP
#define ERAGPSIM_PARSER_SYMBOL_HPP

#include <regex>
#include <string>
#include "parser/code-position-interval.hpp"

enum class SymbolBehavior { STATIC, DYNAMIC };

class Symbol {
 public:
  Symbol(const std::string& name,
         const std::string& value,
         const CodePositionInterval position,
         SymbolBehavior behavior = SymbolBehavior::STATIC);

  const std::string& name() const noexcept;
  const std::string& value() const noexcept;
  const CodePositionInterval& position() const noexcept;
  SymbolBehavior behavior() const noexcept;
  const std::regex regex() const noexcept;
  bool nameValid() const;

 private:
  std::string _name;
  std::string _value;
  CodePositionInterval _position;
  SymbolBehavior _behavior;
  std::regex _regex;
};

#endif /* ERAGPSIM_PARSER_SYMBOL_HPP */
