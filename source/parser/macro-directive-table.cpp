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

#include "parser/macro-directive-table.hpp"

#include "parser/macro-directive.hpp"

bool MacroDirectiveTable::insert(MacroDirective &macro) {
  auto paramCount = macro.getParameterCount();
  // Insert macro for every possible amount of characters.
  for (int i = paramCount.first; i <= paramCount.second; i++) {
    bool success = _macros.insert({{macro.macroName(), i}, macro}).second;
    if (!success) return false;
  }
  return true;
}

auto MacroDirectiveTable::find(const std::string &name, size_t argCount)
    -> MacroWrapper {
  return {_macros.find({name, argCount}), _macros.end()};
}

auto MacroDirectiveTable::begin() -> macro_map::const_iterator {
  return _macros.begin();
}

auto MacroDirectiveTable::end() -> macro_map::const_iterator {
  return _macros.end();
}

MacroDirectiveTable::MacroWrapper::MacroWrapper(macro_map::iterator i,
                                                const macro_map::iterator &end)
: _i(i), _end(end) {
  if (i == end) return;
  if (i->second.isCompiling())
    _cyclic = true;
  else
    i->second._isCompiling = true;
}

MacroDirectiveTable::MacroWrapper::~MacroWrapper() {
  if (!_cyclic && _i != _end) {
    _i->second._isCompiling = false;
  }
}

auto MacroDirectiveTable::MacroWrapper::operator*()
    -> const macro_map::value_type & {
  assert::that(!_cyclic && _i != _end);
  return *_i;
}

auto MacroDirectiveTable::MacroWrapper::operator-> ()
    -> macro_map::const_iterator {
  assert::that(!_cyclic && _i != _end);
  return _i;
}
