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

#ifndef ERAGPSIM_PARSER_SYMBOL_REPLACER_HPP
#define ERAGPSIM_PARSER_SYMBOL_REPLACER_HPP

#include <functional>
#include <vector>
#include "common/multiregex.hpp"
#include "parser/independent/positioned-string.hpp"
#include "parser/independent/symbol.hpp"
class SymbolGraphEvaluation;
class CompileErrorList;

class SymbolReplacer {
 public:
  using size_t = std::size_t;
  using DynamicReplacer = std::function<std::string(const Symbol&)>;
  static const DynamicReplacer IDENTITY_REPLACE;

  explicit SymbolReplacer(const SymbolGraphEvaluation& evaluation,
                          const DynamicReplacer& replacer = IDENTITY_REPLACE,
                          size_t maximumReplaceCount = 64);
  explicit SymbolReplacer(const std::vector<Symbol>& symbols = {},
                          const DynamicReplacer& replacer = IDENTITY_REPLACE,
                          size_t maximumReplaceCount = 64);
  SymbolReplacer(const SymbolReplacer& source, const DynamicReplacer& replacer);

  PositionedString
  replace(const PositionedString& data, CompileErrorList& errors) const;

  const std::vector<Symbol>& symbols() const noexcept;
  size_t maximumReplaceCount() const noexcept;

 private:
  std::vector<Symbol> _symbols;
  MSRegex _matchRegex;
  DynamicReplacer _replacer;
  size_t _maximumReplaceCount;
};

#endif /* ERAGPSIM_PARSER_SYMBOL_REPLACER_HPP */
