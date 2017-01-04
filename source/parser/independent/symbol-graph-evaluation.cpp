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

#include "parser/independent/symbol-graph-evaluation.hpp"

#include <cstddef>
#include <vector>

#include "common/assert.hpp"

// Parametrized constructor.

SymbolGraphEvaluation::SymbolGraphEvaluation(
    const std::vector<std::size_t>& invalidNames,
    const std::vector<std::vector<std::size_t>>& duplicates,
    const std::vector<std::size_t>& sampleCycle,
    const std::vector<std::size_t>& topologicOrder,
    const std::vector<Symbol>& symbols)
: _invalidNames(invalidNames)
, _duplicates(duplicates)
, _sampleCycle(sampleCycle)
, _topologicOrder(topologicOrder)
, _symbols(symbols) {
}

bool SymbolGraphEvaluation::valid() const noexcept {
  // For the graph to be valid, there must not be a cyclic reference, duplicates
  // or invalid names.
  return _sampleCycle.empty() && _duplicates.empty() && _invalidNames.empty();
}

// Getters.

const std::vector<std::size_t>& SymbolGraphEvaluation::invalidNames() const
    noexcept {
  return _invalidNames;
}
const std::vector<std::vector<std::size_t>>&
SymbolGraphEvaluation::duplicates() const noexcept {
  return _duplicates;
}
const std::vector<std::size_t>& SymbolGraphEvaluation::sampleCycle() const
    noexcept {
  return _sampleCycle;
}
const std::vector<std::size_t>& SymbolGraphEvaluation::topologicOrder() const
    noexcept {
  return _topologicOrder;
}
const std::vector<Symbol>& SymbolGraphEvaluation::symbols() const noexcept {
  return _symbols;
}
