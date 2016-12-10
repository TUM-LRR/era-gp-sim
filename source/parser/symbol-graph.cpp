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

#include "parser/symbol-graph.hpp"
#include <unordered_map>
#include "common/assert.hpp"
#include "common/utility.hpp"
#include "parser/symbol-graph-evaluation.hpp"

using size_t = std::size_t;

void SymbolGraph::addNode(const Symbol& symbol) {
  auto newNode = SymbolNode(symbol, _nodes.size());
  for (auto& node : _nodes) {
    node.checkAdjacency(newNode);
    newNode.checkAdjacency(node);
  }

  newNode.checkAdjacency(newNode);

  _nodes.push_back(newNode);
}

class PartialEvaluation {
 public:
  PartialEvaluation(const std::vector<size_t>& topologicOrder,
                    const std::vector<size_t>& sampleCycle)
  : _topologicOrder(topologicOrder), _sampleCycle(sampleCycle) {
  }

  const std::vector<size_t>& topologicOrder() const noexcept {
    return _topologicOrder;
  }
  const std::vector<size_t>& sampleCycle() const noexcept {
    return _sampleCycle;
  }

 private:
  std::vector<size_t> _topologicOrder;
  std::vector<size_t> _sampleCycle;
};

struct StackEntry {
  size_t index;
  size_t position;

  StackEntry(size_t index, size_t position) : index(index), position(position) {
  }
};

static std::vector<size_t>
decomposeCycle(size_t startNode, std::stack<StackEntry>& stack) {
  std::vector<size_t> cycle;
  cycle.push_back(startNode);
  assert::that(!stack.empty());
  while (stack.top().index != startNode) {
    cycle.push_back(stack.top().index);
    stack.pop();
    assert::that(!stack.empty());
  }
  cycle.push_back(startNode);
  return cycle;
}

static std::vector<size_t>
connectedDfs(const std::vector<SymbolGraph::SymbolNode>& nodes,
             std::vector<size_t>& topologicOrder,
             std::vector<bool>& visited,
             size_t start) {
  std::stack<size_t> toVisit;
  std::stack<StackEntry> emulatedStack;
  std::vector<bool> fastLookup(nodes.size());

  toVisit.push(start);
  while (!toVisit.empty()) {
    auto node = toVisit.top();
    toVisit.pop();
    if (!visited[node]) {
      visited[node] = true;
      fastLookup[node] = true;
      emulatedStack.push(StackEntry(node, toVisit.size()));

      for (const auto& neighbor : nodes[node].adjacent()) {
        toVisit.push(neighbor);
      }
    } else if (fastLookup[node]) {
      return decomposeCycle(node, emulatedStack);
    }

    while (!emulatedStack.empty() &&
           emulatedStack.top().position == toVisit.size()) {
      auto index = emulatedStack.top().index;
      emulatedStack.pop();
      fastLookup[index] = false;
      topologicOrder.push_back(index);
    }
  }

  return std::vector<size_t>();
}

static PartialEvaluation
disconnectedDfs(const std::vector<SymbolGraph::SymbolNode>& nodes) {
  std::vector<bool> visited(nodes.size());
  std::vector<size_t> topologicOrder;
  if (!nodes.empty()) {
    for (const auto& node : Utility::range<size_t>(0, nodes.size())) {
      if (!visited[node]) {
        auto result = connectedDfs(nodes, topologicOrder, visited, node);
        if (!result.empty()) {
          return PartialEvaluation({}, result);
        }
      }
    }
  }

  return PartialEvaluation(topologicOrder, {});
}

static std::vector<Symbol>
prepareSymbols(const std::vector<SymbolGraph::SymbolNode>& nodes,
               const std::vector<size_t>& topologicOrder) {
  std::vector<std::string> values;
  for (const auto& node : nodes) {
    values.push_back(node.symbol().value());
  }
  for (const auto& nodeIndex : Utility::revertIterable(topologicOrder)) {
    const auto& node = nodes[nodeIndex];
    const auto& nodeValue = values[nodeIndex];
    const auto& nodeRegex = node.symbol().regex();
    for (const auto& neighbor : node.adjacent()) {
      values[neighbor] =
          std::regex_replace(values[neighbor], nodeRegex, nodeValue);
    }
  }
  std::vector<Symbol> symbols;
  for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex) {
    const auto& symbol = nodes[nodeIndex].symbol();
    const auto& nodeValue = values[nodeIndex];
    symbols.push_back(
        Symbol(symbol.name(), nodeValue, symbol.position(), symbol.behavior()));
  }
  return symbols;
}

static std::vector<std::vector<size_t>>
checkDoubleSymbols(const std::vector<SymbolGraph::SymbolNode>& nodes) {
  std::unordered_map<std::string, std::vector<size_t>> nameTest;
  for (const auto& node : nodes) {
    nameTest[node.symbol().name()].push_back(node.index());
  }

  std::vector<std::vector<size_t>> duplicates;
  for (const auto& combined : nameTest) {
    auto& sameIdentifier = combined.second;
    if (sameIdentifier.size() > 1) {
      duplicates.push_back(sameIdentifier);
    }
  }
  return duplicates;
}

static std::vector<size_t>
checkSymbolNames(const std::vector<SymbolGraph::SymbolNode>& nodes) {
  std::vector<size_t> invalidNames;
  for (const auto& node : nodes) {
    if (!node.symbol().nameValid()) {
      invalidNames.push_back(node.index());
    }
  }
  return invalidNames;
}

SymbolGraphEvaluation SymbolGraph::evaluate() const {
  auto doubleSymbols = checkDoubleSymbols(_nodes);
  auto invalidNames = checkSymbolNames(_nodes);
  auto result = disconnectedDfs(_nodes);
  std::vector<Symbol> symbols;
  if (result.sampleCycle().empty() && doubleSymbols.empty() &&
      invalidNames.empty()) {
    symbols = prepareSymbols(_nodes, result.topologicOrder());
  }
  return SymbolGraphEvaluation(invalidNames,
                               doubleSymbols,
                               result.sampleCycle(),
                               result.topologicOrder(),
                               symbols);
}

SymbolGraph::SymbolNode::SymbolNode(const Symbol& symbol, size_t index)
: _symbol(symbol), _index(index), _adjacent() {
}

const Symbol& SymbolGraph::SymbolNode::symbol() const noexcept {
  return _symbol;
}
size_t SymbolGraph::SymbolNode::index() const noexcept {
  return _index;
}
const std::vector<size_t>& SymbolGraph::SymbolNode::adjacent() const noexcept {
  return _adjacent;
}

void SymbolGraph::SymbolNode::checkAdjacency(const SymbolNode& other) {
  if (symbol().behavior() == SymbolBehavior::STATIC &&
      std::regex_search(other.symbol().value(), symbol().regex())) {
    _adjacent.push_back(other.index());
  }
}
