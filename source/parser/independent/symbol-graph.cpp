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

#include "parser/independent/symbol-graph.hpp"
#include <unordered_map>
#include "common/assert.hpp"
#include "common/utility.hpp"
#include "parser/independent/positioned-string.hpp"
#include "parser/independent/symbol-graph-evaluation.hpp"

using size_t = std::size_t;

void SymbolGraph::addNode(const Symbol& symbol) {
  // Adding a node means checking all adjencencies to it and from it away – and
  // also to itself.
  auto newNode = SymbolNode(symbol, _nodes.size());
  for (auto& node : _nodes) {
    node.checkAdjacency(newNode);
    newNode.checkAdjacency(node);
  }

  newNode.checkAdjacency(newNode);

  _nodes.push_back(newNode);
}

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

struct StackEntry {
  size_t index;
  size_t position;

  StackEntry(size_t index, size_t position) : index(index), position(position) {
  }
};

static std::vector<size_t>
decomposeCycle(size_t startNode, std::stack<StackEntry>& stack) {
  // Here, we decompose the stack to find a cycle.
  std::vector<size_t> cycle;
  cycle.push_back(startNode);

  // We know, (at least: when called in this one place in the function below)
  // that 'startNode' exists again in the stack, so we go down, until we find
  // it.
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
  // Now here comes the fun part (almost as fun as implementing the Hierholzer
  // algorithm for Euler cycles iteratively, it's horrible), we do iterative DFS
  // but with checking for a DAG and generating of finishing numbers for each
  // node (we need it for the topologic sort). Sounds fun? So it is. Much fun.
  // (why iterative? Well, the main reason was that if some day some maniac came
  // and did some crazy constant stuff, the whole system would not crash because
  // of out of stack... But this might also be thought a bit too far... But hey,
  // now we've already got it coded!)

  // The stack for iterative DFS (works like described on Wikipedia).
  std::stack<size_t> toVisit;

  // Some means to emulate the call stack.
  std::stack<StackEntry> emulatedStack;
  std::vector<bool> fastLookup(nodes.size());

  // We start as usual with one vertex pushed.
  toVisit.push(start);
  while (!toVisit.empty()) {
    // We take the next vertex on the stack.
    auto node = toVisit.top();
    toVisit.pop();
    if (!visited[node]) {
      // If we have not visited this node yet (there can be one node more than
      // once on the toVisit stack, but otherwise we would not visit them like a
      // DFS, always the oldest node would be considered, but we need the latest
      // occurence found), we visit it now.
      visited[node] = true;

      // Here comes the special part: we emulate the callstack and the nodes in
      // the current path to the root.
      fastLookup[node] = true;
      emulatedStack.push(StackEntry(node, toVisit.size()));

      for (const auto& neighbor : nodes[node].adjacent()) {
        // Then, we push each neighbor, regardless if already visited or not
        // (for reason why, see above; the algorithm is still bound by the
        // number of edges in the graph b/c a node might be as much put on the
        // stack as there are edges to it).
        toVisit.push(neighbor);
      }
    } else if (fastLookup[node]) {
      // If we have already visited the node, but it is also in our current path
      // to root, we are now in a cycle.
      return decomposeCycle(node, emulatedStack);
    }

    // In the end, we need to emulate the recursive backtracking on our virtual
    // stack. For this, we save the number of items on our toVisit stack when we
    // put our node on the stack. If we are back down, it means that we are back
    // at the old junction again and our node is not visited any more.
    while (!emulatedStack.empty() &&
           emulatedStack.top().position == toVisit.size()) {
      // If so, we have finished a node, so we put it to the (inverse) topologic
      // order.
      auto index = emulatedStack.top().index;
      emulatedStack.pop();
      fastLookup[index] = false;
      topologicOrder.push_back(index);
    }

    // And all of that, we do over and over again.
  }

  // If we have finished without problems, there has not been a cycle.
  return std::vector<size_t>();
}

static PartialEvaluation
disconnectedDfs(const std::vector<SymbolGraph::SymbolNode>& nodes) {
  // To check for cycles, we to a DFS over the graph which might not be
  // connected...
  std::vector<bool> visited(nodes.size());
  std::vector<size_t> topologicOrder;

  // So we have to go over all the nodes until everyone is visited.
  for (const auto& node : Utility::range<size_t>(0, nodes.size())) {
    if (!visited[node]) {
      // If we found a non-visited node, we have found a new component of the
      // graph.
      auto result = connectedDfs(nodes, topologicOrder, visited, node);

      if (!result.empty()) {
        // 'result' is a sample cycle. If we got one, there is one in the whole
        // graph, i.e. we fail.
        return PartialEvaluation({}, result);
      }
    }
  }

  std::reverse(topologicOrder.begin(), topologicOrder.end());
  return PartialEvaluation(topologicOrder, {});
}

static std::vector<Symbol>
prepareSymbols(const std::vector<SymbolGraph::SymbolNode>& nodes,
               const std::vector<size_t>& topologicOrder) {
  // Here, we use the topologic order (if the graph is correct) to replace
  // everything as far as possible.
  // But first of all, we need to get all values separated.
  std::vector<std::string> values;
  for (const auto& node : nodes) {
    values.push_back(node.symbol().value().string());
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
    symbols.push_back(
        Symbol(symbol.name(), positionedValue, symbol.behavior()));
  }
  return symbols;
}

static std::vector<std::vector<size_t>>
checkDoubleSymbols(const std::vector<SymbolGraph::SymbolNode>& nodes) {
  // To record all duplicate symbols, we divide our symbol nodes into
  // equivalence classes by name.
  std::unordered_map<std::string, std::vector<size_t>> nameTest;
  for (const auto& node : nodes) {
    // Using a map for this.
    nameTest[node.symbol().name().string()].push_back(node.index());
  }

  // Then we take all equivalence classes which have size 2 or greater.
  std::vector<std::vector<size_t>> duplicates;
  for (const auto& combined : nameTest) {
    auto& sameIdentifier = combined.second;

    // Now we check for it.
    if (sameIdentifier.size() > 1) {
      duplicates.push_back(sameIdentifier);
    }
  }
  return duplicates;
}

static std::vector<size_t>
checkSymbolNames(const std::vector<SymbolGraph::SymbolNode>& nodes) {
  // For invalid names, we just collect all nodes which do not follow our name
  // guidelines.
  std::vector<size_t> invalidNames;
  for (const auto& node : nodes) {
    if (!node.symbol().nameValid()) {
      invalidNames.push_back(node.index());
    }
  }
  return invalidNames;
}

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

void SymbolGraph::SymbolNode::checkAdjacency(const SymbolNode& other) {
  // We only create an edge between two nodes, if: we have static node behavior
  // (so we do not change our value randomly, with this we cannot check for
  // circles), and we are contained in the replacement value of the other
  // symbol.
  if (symbol().behavior() == SymbolBehavior::STATIC &&
      std::regex_search(other.symbol().value().string(), symbol().regex())) {
    _adjacent.push_back(other.index());
  }
}
