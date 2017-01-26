/*
* ERASIM Assembler Interpreter
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

#include "parser/independent/symbol-graph.hpp"

#include <cstddef>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/assert.hpp"
#include "common/utility.hpp"
#include "parser/independent/positioned-string.hpp"
#include "parser/independent/symbol-graph-evaluation.hpp"

using size_t = std::size_t;

namespace {
// Some small helper classes.
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

std::vector<size_t>
decomposeCycle(size_t startNode, std::stack<size_t>& stack) {
  assert::that(!stack.empty());

  // Here, we decompose the stack to find a cycle.
  std::vector<size_t> cycle;
  cycle.emplace_back(startNode);

  // We know, (at least: when called in this one place in the function below)
  // that 'startNode' exists again in the stack, so we go down, until we find
  // it.
  while (stack.top() != startNode) {
    cycle.emplace_back(stack.top());
    stack.pop();
    assert::that(!stack.empty());
  }
  return cycle;
}

std::vector<size_t>
connectedDfs(const std::vector<SymbolGraph::SymbolNode>& nodes,
             std::vector<size_t>& topologicOrder,
             std::vector<bool>& visited,
             std::stack<size_t>& currentPath,
             std::vector<bool> fastPathLookup,
             size_t node) {
  // This is a recursively performed DFS on a connected graph. We also check if
  // there are any cycles and record the reverse topologic order (a.k.a. finish
  // numbers of the nodes). In case this leads to a stack overflow, here is also
  // an iterative implementation, e.g.:
  // https://github.com/TUM-LRR/erasim/blob/125a5d17378b7628608e0dc6977e36ee0b28da2a/source/parser/independent/symbol-graph.cpp

  if (!visited[node]) {
    // If we have not visited this node at all, then we do it now.
    visited[node] = true;

    // This we need for tracking down cycles.
    currentPath.push(node);
    fastPathLookup[node] = true;

    // Just for assertions.
    size_t controlLength = currentPath.size();

    // Now we check every neighbor and visit it. In case we got a cycle here, we
    // pass it down.
    for (const auto& neighbor : nodes[node].adjacent()) {
      auto result = connectedDfs(nodes,
                                 topologicOrder,
                                 visited,
                                 currentPath,
                                 fastPathLookup,
                                 neighbor);
      if (!result.empty()) {
        return result;
      }
    }

    // Assertion that the path is tracked properly.
    assert::that(controlLength == currentPath.size());

    // Of course, we have to remove our node now from the path.
    currentPath.pop();
    fastPathLookup[node] = false;

    // Also, we are done with this node now, so we can add it to the finished
    // ones.
    topologicOrder.push_back(node);

  } else if (fastPathLookup[node]) {
    // If we have already visited the node, but it is also in our current path
    // to root, we are now in a cycle.
    return decomposeCycle(node, currentPath);
  }

  // In case we get here, there has nothing been gone wrong.
  return std::vector<size_t>();
}

PartialEvaluation
disconnectedDfs(const std::vector<SymbolGraph::SymbolNode>& nodes) {
  // To check for cycles, we do a DFS over the graph which might not be
  // connected...
  std::vector<bool> visited(nodes.size());
  std::vector<size_t> topologicOrder;
  std::stack<size_t> currentPath;
  std::vector<bool> fastPathLookup(nodes.size());

  // So we have to go over all the nodes until everyone is visited.
  for (const auto& node : Utility::range<size_t>(0, nodes.size())) {
    if (visited[node]) continue;
    // If we found a non-visited node, we have found a new component of the
    // graph.
    auto result = connectedDfs(
        nodes, topologicOrder, visited, currentPath, fastPathLookup, node);

    if (!result.empty()) {
      // 'result' is a sample cycle. If we got one, there is one in the whole
      // graph, i.e. we fail.
      return PartialEvaluation({}, result);
    }
  }

  std::reverse(topologicOrder.begin(), topologicOrder.end());
  return PartialEvaluation(topologicOrder, {});
}

std::vector<Symbol>
prepareSymbols(const std::vector<SymbolGraph::SymbolNode>& nodes,
               const std::vector<size_t>& topologicOrder) {
  // Here, we use the topologic order (if the graph is correct) to replace
  // everything as far as possible.
  // But first of all, we need to get all values separated.
  std::vector<std::string> values;
  for (const auto& node : nodes) {
    values.emplace_back(node.symbol().value().string());
  }

  // Then we traverse the nodes again and replace every adjacent neighbor of the
  // node we just visited (like going down the graph).
  // (we need to keep the array b/c in following iterations, there might be more
  // replaced)
  for (const auto& nodeIndex : topologicOrder) {
    const auto& node = nodes[nodeIndex];
    const auto& nodeValue = values[nodeIndex];
    const auto& nodeRegex = node.symbol().regex();
    for (const auto& neighbor : node.adjacent()) {
      values[neighbor] =
          std::regex_replace(values[neighbor], nodeRegex, nodeValue);
    }
  }

  // Then we re-create our symbols.
  std::vector<Symbol> symbols;
  for (auto nodeIndex : Utility::range<size_t>(0, nodes.size())) {
    const auto& symbol = nodes[nodeIndex].symbol();
    const auto& nodeValue = values[nodeIndex];
    auto positionedValue =
        PositionedString(nodeValue, symbol.value().positionInterval());
    symbols.emplace_back(
        Symbol(symbol.name(), positionedValue, symbol.behavior()));
  }
  return symbols;
}

std::vector<std::vector<size_t>>
checkDoubleSymbols(const std::vector<SymbolGraph::SymbolNode>& nodes) {
  // To record all duplicate symbols, we divide our symbol nodes into
  // equivalence classes by name.
  std::unordered_map<std::string, std::vector<size_t>> nameTest;
  for (const auto& node : nodes) {
    // Using a map for this.
    nameTest[node.symbol().name().string()].emplace_back(node.index());
  }

  // Then we take all equivalence classes which have size 2 or greater.
  std::vector<std::vector<size_t>> duplicates;
  for (const auto& combined : nameTest) {
    auto& sameIdentifier = combined.second;

    // Now we check for it.
    if (sameIdentifier.size() > 1) {
      duplicates.emplace_back(sameIdentifier);
    }
  }
  return duplicates;
}

std::vector<size_t>
checkSymbolNames(const std::vector<SymbolGraph::SymbolNode>& nodes) {
  // For invalid names, we just collect all nodes which do not follow our name
  // guidelines.
  std::vector<size_t> invalidNames;
  for (const auto& node : nodes) {
    if (!node.symbol().nameValid()) {
      invalidNames.emplace_back(node.index());
    }
  }
  return invalidNames;
}
}  // namespace

SymbolGraphEvaluation SymbolGraph::evaluate() const {
  // For evaluation, just check everything (invalid names, duplicates, cycles).
  auto doubleSymbols = checkDoubleSymbols(_nodes);
  auto invalidNames = checkSymbolNames(_nodes);
  auto result = disconnectedDfs(_nodes);
  auto symbols = prepareSymbols(_nodes, result.topologicOrder());
  return SymbolGraphEvaluation(invalidNames,
                               doubleSymbols,
                               result.sampleCycle(),
                               result.topologicOrder(),
                               symbols);
}

// Constructor.

SymbolGraph::SymbolNode::SymbolNode(const Symbol& symbol, size_t index)
: _symbol(symbol), _index(index), _adjacent() {
}

// Getters.

const Symbol& SymbolGraph::SymbolNode::symbol() const noexcept {
  return _symbol;
}
size_t SymbolGraph::SymbolNode::index() const noexcept {
  return _index;
}
const std::vector<size_t>& SymbolGraph::SymbolNode::adjacent() const noexcept {
  return _adjacent;
}

void SymbolGraph::SymbolNode::connectIfContained(const SymbolNode& other) {
  // We only create an edge between two nodes, if: we have static node behavior
  // (so we do not change our value randomly, with this we cannot check for
  // circles), and we are contained in the replacement value of the other
  // symbol.
  if (symbol().behavior() == SymbolBehavior::STATIC &&
      std::regex_search(other.symbol().value().string(), symbol().regex())) {
    _adjacent.emplace_back(other.index());
  }
}

void SymbolGraph::addNode(const Symbol& symbol) {
  // Adding a node means checking all adjencencies to it and from it away – and
  // also to itself.
  auto newNode = SymbolNode(symbol, _nodes.size());

  // Note: we might actually modify the nodes here.
  for (auto& node : _nodes) {
    node.connectIfContained(newNode);
    newNode.connectIfContained(node);
  }

  newNode.connectIfContained(newNode);

  _nodes.emplace_back(newNode);
}
