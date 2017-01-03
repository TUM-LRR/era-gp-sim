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

#include <cstddef>
#include <regex>
#include <string>
#include <vector>

#include "common/assert.hpp"
#include "common/utility.hpp"
#include "parser/common/compile-error-list.hpp"
#include "parser/independent/symbol-graph-evaluation.hpp"
#include "parser/independent/symbol-replacer.hpp"

// Helper method to create multiregex out of symbol names (which have been
// checked for valid names before, hopefully... Otherwise, we might have a regex
// injection).
namespace {
MSRegex constructSymbolRegex(const std::vector<Symbol>& symbols) {
  std::vector<std::string> names;
  for (const auto& symbol : symbols) {
    // Only if the names are valid, they are allowed to be inserted.
    assert::that(symbol.nameValid());
    names.emplace_back(symbol.name().string());
  }
  if (names.empty()) {
    // If there is no symbol inserted, we insert an invalid one.
    names.emplace_back("none^");
  }
  return MSRegex("\\b", "\\b", names, std::regex::optimize);
}
}  // namespace

// Some constructors.

SymbolReplacer::SymbolReplacer(const std::vector<Symbol>& symbols,
                               const DynamicReplacer& replacer,
                               std::size_t maximumReplaceCount)
: _symbols(symbols)
, _maximumReplaceCount(maximumReplaceCount)
, _replacer(replacer)
, _matchRegex(constructSymbolRegex(symbols)) {
}

SymbolReplacer::SymbolReplacer(const SymbolGraphEvaluation& evaluation,
                               const DynamicReplacer& replacer,
                               std::size_t maximumReplaceCount)
: SymbolReplacer(evaluation.symbols(), replacer, maximumReplaceCount) {
}

SymbolReplacer::SymbolReplacer(const SymbolReplacer& source,
                               const DynamicReplacer& replacer)
: _symbols(source._symbols)
, _maximumReplaceCount(source._maximumReplaceCount)
, _replacer(replacer)
, _matchRegex(source._matchRegex) {
}

PositionedString SymbolReplacer::replace(const PositionedString& data,
                                         CompileErrorList& errors) const {
  // Main function here. We define our replace function like that we replace all
  // dynamic occurences with our replace function. Everything else stays the
  // same.
  auto result = data.string();
  auto multireplace = [&](std::size_t index) -> std::string {
    const auto& symbol = _symbols[index];
    if (symbol.behavior() == SymbolBehavior::DYNAMIC) {
      return _replacer(symbol);
    } else {
      return symbol.value().string();
    }
  };

  // Then iterate at max 'maximumReplaceCount' round.
  for (const auto& _ : Utility::range<std::size_t>(0, _maximumReplaceCount)) {
    (void)_;  // (to prevent unused variable warnings)

    auto previous = result;

    // We replace another time.
    result = _matchRegex.replace(result, multireplace);
    if (result == previous) {
      // If nothing changes, everything has been replaced, return it!
      return PositionedString(result, data.positionInterval());
    }
  }

  // If we get here, we have exceeded the maximum number of replacement runs.
  // The reason why we need to do multiple runs at all is that we got symbols
  // with dynamic behavior which might change all the time and even produce a
  // cycle spontaneously. It is very unlikely, but in theory that possibility
  // remains.
  errors.pushError(data.positionInterval(),
                   "Exceeded maximum number of replace runs.");

  // The position interval is still the same as for the original string though
  // (even if replacement succeeded).
  return PositionedString(result, data.positionInterval());
}

const SymbolReplacer::DynamicReplacer SymbolReplacer::IDENTITY_REPLACE =
    [](const Symbol& symbol) { return symbol.value().string(); };

// Getters.

const std::vector<Symbol>& SymbolReplacer::symbols() const noexcept {
  return _symbols;
}
size_t SymbolReplacer::maximumReplaceCount() const noexcept {
  return _maximumReplaceCount;
}
