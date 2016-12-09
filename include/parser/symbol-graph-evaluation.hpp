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

#ifndef ERAGPSIM_PARSER_SYMBOL_GRAPH_EVALUATION_HPP
#define ERAGPSIM_PARSER_SYMBOL_GRAPH_EVALUATION_HPP

#include <vector>
#include "parser/symbol.hpp"

class SymbolGraphEvaluation {
 public:
  SymbolGraphEvaluation(const std::vector<std::size_t>& invalidNames,
                        const std::vector<std::vector<std::size_t>>& duplicates,
                        const std::vector<std::size_t>& sampleCycle,
                        const std::vector<std::size_t>& topologicOrder,
                        const std::vector<Symbol>& symbols);

  bool valid() const noexcept;
  const std::vector<std::size_t>& invalidNames() const noexcept;
  const std::vector<std::vector<std::size_t>>& duplicates() const noexcept;
  const std::vector<std::size_t>& sampleCycle() const noexcept;
  const std::vector<std::size_t>& topologicOrder() const noexcept;
  const std::vector<Symbol>& symbols() const noexcept;

 private:
  std::vector<std::size_t> _invalidNames;
  std::vector<std::vector<std::size_t>> _duplicates;
  std::vector<std::size_t> _sampleCycle;
  std::vector<std::size_t> _topologicOrder;
  std::vector<Symbol> _symbols;
};

#endif /* ERAGPSIM_PARSER_SYMBOL_GRAPH_EVALUATION_HPP */
