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

#include "parser/symbol.hpp"
#include "common/assert.hpp"

static const std::regex TRIMMED =
    std::regex("(^\\s+|\\s+$)", std::regex_constants::optimize);
static const std::regex VALID_NAME =
    std::regex("^[A-Za-z_][A-Za-z0-9_]*$", std::regex_constants::optimize);

static bool symbolNameValid(const std::string& name) {
  return std::regex_search(name, VALID_NAME);
}

Symbol::Symbol(const std::string& name,
               const std::string& value,
               const CodePositionInterval position,
               SymbolBehavior behavior)
: _name(name)
, _value(value)
, _position(position)
, _behavior(behavior)
, _regex((symbolNameValid(name) ? ("\\b" + name + "\\b") : ("no^")),
         std::regex::optimize) {
  assert::that(!std::regex_search(name, TRIMMED));
}

const std::string& Symbol::name() const noexcept {
  return _name;
}
const std::string& Symbol::value() const noexcept {
  return _value;
}
const CodePositionInterval& Symbol::position() const noexcept {
  return _position;
}
SymbolBehavior Symbol::behavior() const noexcept {
  return _behavior;
}
const std::regex Symbol::regex() const noexcept {
  return _regex;
}

bool Symbol::nameValid() const {
  return symbolNameValid(_name);
}
