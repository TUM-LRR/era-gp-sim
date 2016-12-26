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

#include "parser/independent/symbol.hpp"
#include "common/assert.hpp"

// Some testing regexes.
static const std::regex TRIMMED =
    std::regex("(^\\s+|\\s+$)", std::regex_constants::optimize);
static const std::regex VALID_NAME =
    std::regex("^[A-Za-z_][A-Za-z0-9_]*$", std::regex_constants::optimize);

static bool symbolNameValid(const PositionedString& name) {
  return std::regex_search(name.string(), VALID_NAME);
}


Symbol::Symbol(const PositionedString& name,
               const PositionedString& value,
               SymbolBehavior behavior)
: _name(name)
, _value(value)
, _behavior(behavior)
, _regex((symbolNameValid(name) ? ("\\b" + name.string() + "\\b") : ("no^")),
         std::regex::optimize) {
  // We only accept trimmed macros.
  assert::that(!std::regex_search(name.string(), TRIMMED));
}

// Some getters.

const PositionedString& Symbol::name() const noexcept {
  return _name;
}
const PositionedString& Symbol::value() const noexcept {
  return _value;
}
SymbolBehavior Symbol::behavior() const noexcept {
  return _behavior;
}
const std::regex& Symbol::regex() const noexcept {
  return _regex;
}

// Helper methods.

bool Symbol::nameValid() const {
  return symbolNameValid(_name);
}
