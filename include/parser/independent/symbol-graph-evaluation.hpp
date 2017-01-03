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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_SYMBOL_GRAPH_EVALUATION_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_SYMBOL_GRAPH_EVALUATION_HPP

#include <vector>
#include "parser/independent/symbol.hpp"

/**
 * A helper class for storing the result of a symbol graph evaluation.
 *
 * It contains all possible errors (invalid names, duplicate names, cycles in
 * dependencies) and maybe a topologic order for efficient replacement, if the
 * graph is correct. Also, it contains all symbols, maybe also replaced as far
 * as possible.
 */
class SymbolGraphEvaluation {
 public:
  using size_t = std::size_t;

  /**
   * Creates a new symbol graph evaluation with all the given parameters
   *
   * \param invalidNames A list of all invalid names in the graph.
   * \param duplicates A list of all duplicate symbol names.
   * \param sampleCycle A sample cycle in the symbol graph.
   * \param topologicOrder The topologic order of all the elements (only, if
   * everything is valid).
   * \param symbols The list of all symbols.
   */
  SymbolGraphEvaluation(const std::vector<size_t>& invalidNames,
                        const std::vector<std::vector<size_t>>& duplicates,
                        const std::vector<size_t>& sampleCycle,
                        const std::vector<size_t>& topologicOrder,
                        const std::vector<Symbol>& symbols);

  /**
   * \return If the graph is valid, i.e. all names are valid and unique, and
   * there exist no dependency cycles.
   */
  bool valid() const noexcept;

  /**
   * \return A list of all invalid names in the graph.
   */
  const std::vector<size_t>& invalidNames() const noexcept;

  /**
   * \return A list of all duplicate symbol names.
   */
  const std::vector<std::vector<size_t>>& duplicates() const noexcept;

  /**
   * \return A sample cycle in the symbol graph.
   */
  const std::vector<size_t>& sampleCycle() const noexcept;

  /**
   * \return The topologic order of all the elements (only, if everything is
   * valid).
   */
  const std::vector<size_t>& topologicOrder() const noexcept;

  /**
   * \return The list of all symbols.
   */
  const std::vector<Symbol>& symbols() const noexcept;

 private:
  /**
    * A list of all invalid names in the graph.
    */
  std::vector<size_t> _invalidNames;

  /**
   * A list of all duplicate symbol names.
   */
  std::vector<std::vector<size_t>> _duplicates;

  /**
   * A sample cycle in the symbol graph.
   */
  std::vector<size_t> _sampleCycle;

  /**
   * The topologic order of all the elements (only, if everything is valid).
   */
  std::vector<size_t> _topologicOrder;

  /**
   * The list of all symbols.
   */
  std::vector<Symbol> _symbols;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_SYMBOL_GRAPH_EVALUATION_HPP */
