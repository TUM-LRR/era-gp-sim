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

class SymbolGraph {
 public:
  using size_t = std::size_t;
  SymbolGraph() = default;

  void addNode(const Symbol& symbol);

  SymbolGraphEvaluation evaluate() const;

  class SymbolNode {
   public:
    SymbolNode(const Symbol& symbol, size_t index);

    const Symbol& symbol() const noexcept;
    size_t index() const noexcept;
    const std::vector<size_t>& adjacent() const noexcept;

    void checkAdjacency(const SymbolNode& other);

   private:
    Symbol _symbol;
    size_t _index;
    std::vector<size_t> _adjacent;
  };

 private:
  std::vector<SymbolNode> _nodes;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_SYMBOL_GRAPH_HPP */
