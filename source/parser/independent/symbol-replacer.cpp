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

#include "parser/independent/symbol-replacer.hpp"
#include "common/assert.hpp"
#include "common/utility.hpp"
#include "parser/common/compile-error-list.hpp"
#include "parser/independent/symbol-graph-evaluation.hpp"

static MSRegex constructSymbolRegex(std::vector<Symbol> symbols) {
  std::vector<std::string> names;
  for (const auto& symbol : symbols) {
    assert::that(symbol.nameValid());
    names.push_back(symbol.name().string());
  }
  if (names.empty()) {
    names.push_back("none^");
  }
  return MSRegex("\\b", "\\b", names, std::regex::optimize);
}

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
  auto result = data.string();
  auto multireplace = [&](std::size_t index) -> std::string {
    const auto& symbol = _symbols[index];
    if (symbol.behavior() == SymbolBehavior::DYNAMIC) {
      return _replacer(symbol);
    } else {
      return symbol.value().string();
    }
  };
  for (const auto& round :
       Utility::range<std::size_t>(0, _maximumReplaceCount)) {
    (void)round;// (to prevent unused variable warnings)
    auto previous = result;
    result = _matchRegex.replace(result, multireplace);
    if (result == previous) {
      return PositionedString(result, data.positionInterval());
    }
  }

  errors.pushError(data.positionInterval(),
                   "Exceeded maximum number of replace runs.");

  return PositionedString(result, data.positionInterval());
}

const SymbolReplacer::DynamicReplacer SymbolReplacer::IDENTITY_REPLACE =
    [](const Symbol& symbol) { return symbol.value().string(); };

const std::vector<Symbol>& SymbolReplacer::symbols() const noexcept {
  return _symbols;
}
size_t SymbolReplacer::maximumReplaceCount() const noexcept {
  return _maximumReplaceCount;
}
