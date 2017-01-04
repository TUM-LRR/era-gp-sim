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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_SYMBOL_GRAPH_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_SYMBOL_GRAPH_HPP

#include <string>
#include <vector>
#include "parser/independent/symbol.hpp"
class SymbolGraphEvaluation;

/**
 * A graph made out of symbols (as nodes) and the dependencies between them (as
 * edges).
 */
class SymbolGraph {
 public:
  using size_t = std::size_t;

  /**
   * A node containing a symbol and an adjacency list.
   */
  class SymbolNode {
   public:
    /**
     * Creates a new symbol node.
     *
     * \param symbol The symbol to add.
     * \param index The index of the symbol in the graph for easier referencing.
     */
    SymbolNode(const Symbol& symbol, size_t index);

    /**
     * \return The symbol.
     */
    const Symbol& symbol() const noexcept;

    /**
     * \return The index of the symbol in the graph.
     */
    size_t index() const noexcept;

    /**
     * \return The adjacent indices to this node.
     */
    const std::vector<size_t>& adjacent() const noexcept;

    /**
     * Connects two symbol nodes, if this one is contained in the other symbol.
     *
     * Node: we do not connect to any other node, if this symbol is dynamic.
     *
     * \param other The other symbol node to connect with maybe.
     */
    void connectIfContained(const SymbolNode& other);

   private:
    /**
      * The symbol.
      */
    Symbol _symbol;

    /**
     * The index of the symbol in the graph.
     */
    size_t _index;

    /**
     * The adjacent indices to this node.
     */
    std::vector<size_t> _adjacent;
  };

  /**
   * Adds a new symbol to the graph.
   *
   * \param symbol The symbol to add.
   */
  void addNode(const Symbol& symbol);

  /**
   * Evaluates the symbol graph, looking for cycles and invalid symbols. If
   * succeeding, we also replace the symbols as far as possible.
   *
   * \return The graph evaluation.
   */
  SymbolGraphEvaluation evaluate() const;

 private:
  /**
   * The internal list of symbol nodes.
   */
  std::vector<SymbolNode> _nodes;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_SYMBOL_GRAPH_HPP */
