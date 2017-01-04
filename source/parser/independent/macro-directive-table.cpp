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

#include "parser/independent/macro-directive-table.hpp"

#include <cstddef>
#include <string>
#include <utility>

#include "common/assert.hpp"
#include "parser/independent/macro-directive.hpp"

bool MacroDirectiveTable::insert(const MacroDirective &macro) {
  auto paramCount = macro.getParameterCount();
  // Insert macro for every possible amount of arguments
  for (auto count = paramCount.first; count <= paramCount.second; ++count) {
    MacroInstance instance(macro.macroName().string(), count);
    auto result = _macros.emplace(instance, macro);
    if (!result.second) {
      return false;
    }
  }
  return true;
}

MacroDirectiveTable::MacroWrapper
MacroDirectiveTable::find(const std::string &name, size_t argumentCount) {
  auto iterator = _macros.find({name, argumentCount});
  return {iterator, _macros.end()};
}

MacroDirectiveTable::MacroConstIterator MacroDirectiveTable::begin() {
  return _macros.begin();
}

void MacroDirectiveTable::clear() {
  _macros.clear();
}

MacroDirectiveTable::MacroConstIterator MacroDirectiveTable::end() {
  return _macros.end();
}

MacroDirectiveTable::MacroWrapper::MacroWrapper(MacroMap::iterator iterator,
                                                MacroMap::iterator end)
: _iterator(iterator), _end(end) {
  if (iterator == end) return;
  if (iterator->second.isCompiling()) {
    _cyclic = true;
  } else {
    iterator->second._isCompiling = true;
  }
}

MacroDirectiveTable::MacroWrapper::MacroWrapper(MacroWrapper &&other)
: _iterator{std::move(other._iterator)}
, _end{other._end}
, _cyclic{other._cyclic} {
  other._iterator =
      other._end;  // Invalidate other by setting _iterator to _end.
}

MacroDirectiveTable::MacroWrapper::~MacroWrapper() {
  if (!_cyclic && _iterator != _end) {
    _iterator->second._isCompiling = false;
  }
}

bool MacroDirectiveTable::MacroWrapper::
operator==(MacroConstIterator iterator) const noexcept {
  return _iterator == iterator;
}

bool MacroDirectiveTable::MacroWrapper::
operator!=(MacroConstIterator iterator) const noexcept {
  return !(*this == iterator);
}

const MacroDirectiveTable::MacroMap::value_type &
    MacroDirectiveTable::MacroWrapper::operator*() {
  assert::that(!_cyclic && _iterator != _end);
  return *_iterator;
}

MacroDirectiveTable::MacroConstIterator MacroDirectiveTable::MacroWrapper::
operator->() {
  assert::that(!_cyclic && _iterator != _end);
  return _iterator;
}

bool MacroDirectiveTable::MacroWrapper::isCyclic() const noexcept {
  return _cyclic;
}

bool MacroDirectiveTable::MacroWrapper::found() const noexcept {
  return _iterator != _end;
}

const PositionedString &MacroDirectiveTable::MacroWrapper::name() const {
  assert::that(_iterator != _end);
  return _iterator->second.name();
}
