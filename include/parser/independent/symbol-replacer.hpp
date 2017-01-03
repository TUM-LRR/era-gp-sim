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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_SYMBOL_REPLACER_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_SYMBOL_REPLACER_HPP

#include <functional>
#include <vector>
#include "common/multiregex.hpp"
#include "parser/independent/positioned-string.hpp"
#include "parser/independent/symbol.hpp"
class SymbolGraphEvaluation;
class CompileErrorList;

/**
 * A class for replacing recursive symbols in text.
 */
class SymbolReplacer {
 public:
  using size_t = std::size_t;

  /**
   * A function to determine which schema should be used to replace symbols
   * marked as dynamic.
   */
  using DynamicReplacer = std::function<std::string(const Symbol&)>;

  /**
   * The default dynamic replacer, replaces with the specified value in the
   * symbol.
   */
  static const DynamicReplacer IDENTITY_REPLACE;

  /**
   * Creates a new `SymbolReplacer`.
   *
   * \param evaluation The symbol graph evaluation to base the replacer of.
   * \param replacer The replacer for dynamic symbols.
   * \param maximumReplaceCount The maximum number of rounds before aborting.
   */
  explicit SymbolReplacer(const SymbolGraphEvaluation& evaluation,
                          const DynamicReplacer& replacer = IDENTITY_REPLACE,
                          size_t maximumReplaceCount = 64);
  /**
   * Creates a new `SymbolReplacer`.
   *
   * \param symbols The symbol list to base the replacer of.
   * \param replacer The replacer for dynamic symbols.
   * \param maximumReplaceCount The maximum number of rounds before aborting.
   */
  explicit SymbolReplacer(const std::vector<Symbol>& symbols = {},
                          const DynamicReplacer& replacer = IDENTITY_REPLACE,
                          size_t maximumReplaceCount = 64);

  /**
   * Creates a new `SymbolReplacer`.
   *
   * \param source The `SymbolReplacer` to copy from.
   * \param replacer The replacer for dynamic symbols.
   */
  SymbolReplacer(const SymbolReplacer& source, const DynamicReplacer& replacer);

  /**
   * Replaces all symbols the given string.
   *
   * \param data The string to replace all symbols in.
   * \param errors The `CompileErrorList` to denote any occuring errors.
   * \return The replaced string, but seen on the same position interval as the
   * original string.
   */
  PositionedString
  replace(const PositionedString& data, CompileErrorList& errors) const;

  /**
   * \return All symbols stored in this replacer.
   */
  const std::vector<Symbol>& symbols() const noexcept;

  /**
   * \return The maximum count of replacement rounds before aborting.
   */
  size_t maximumReplaceCount() const noexcept;

 private:
  /**
   * All symbols stored in this replacer.
   */
  std::vector<Symbol> _symbols;

  /**
   * The maximum count of replacement rounds before aborting.
   */
  size_t _maximumReplaceCount;

  /**
   * The dynamic replacement function for this symbol replacer.
   */
  DynamicReplacer _replacer;

  /**
   * The internal regex to match all symbols.
   */
  MSRegex _matchRegex;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_SYMBOL_REPLACER_HPP */
