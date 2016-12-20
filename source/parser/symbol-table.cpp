/* C++ Assembler Interpreter
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
#include "parser/symbol-replacer.hpp"

#include <stack>
#include <unordered_set>
#include "common/assert.hpp"
#include "common/utility.hpp"
/*
// This speeds things up a bit.
static const std::regex TRIMMED =
    std::regex("(^\\s+|\\s+$)", std::regex_constants::optimize);
static const std::regex VALID_NAME =
    std::regex("^[A-Za-z_][A-Za-z0-9_]*$", std::regex_constants::optimize);

SymbolTable::TableEntry::TableEntry(const std::string& name,
                                    const std::string& specifiedReplacement,
                                    CodePositionInterval position,
                                    SymbolBehavior behavior,
                                    SymbolType type)
: _name(name)
, _specifiedReplacement(specifiedReplacement)
, _finalReplacement(specifiedReplacement)
, _position(position)
, _type(type)
, _behavior(behavior)
, _searchRegex("\\b" + name + "\\b", std::regex_constants::optimize) {
}
void SymbolTable::TableEntry::checkDependency(const TableEntry& entry) {
  if (std::regex_search(_specifiedReplacement, entry.searchRegex())) {
    _dependencies.insert(entry.name());
  }
}
void SymbolTable::TableEntry::finalReplacement(
    const std::string& finalReplacement) {
  _finalReplacement = finalReplacement;
}
const std::string& SymbolTable::TableEntry::name() const noexcept {
  return _name;
}
const std::string& SymbolTable::TableEntry::specifiedReplacement() const
    noexcept {
  return _specifiedReplacement;
}
const std::string& SymbolTable::TableEntry::finalReplacement() const noexcept {
  return _finalReplacement;
}
SymbolTable::SymbolType SymbolTable::TableEntry::type() const noexcept {
  return _type;
}
SymbolTable::SymbolBehavior SymbolTable::TableEntry::behavior() const noexcept {
  return _behavior;
}
const CodePositionInterval& SymbolTable::TableEntry::position() const noexcept {
  return _position;
}
const std::regex& SymbolTable::TableEntry::searchRegex() const noexcept {
  return _searchRegex;
}
const std::set<std::string>& SymbolTable::TableEntry::dependencies() const
    noexcept {
  return _dependencies;
}

const SymbolTable::Table& SymbolTable::table() const noexcept {
  return _table;
}

void SymbolTable::clearTable() {
  // Simply clear everything.
  _table.clear();
}

void SymbolTable::insertEntry(const std::string& name,
                              const std::string& replacement,
<<<<<<< HEAD
                              const CodePositionInterval& position,
                              CompileState& state,
                              SymbolBehavior behavior,
                              SymbolType type) {
=======
                              CompileState& state,
                              SymbolType type) {
  // Note: this method seems to be really slow. I feel like it is the regexes...
  // We could check them by hand, if it matters.
>>>>>>> master
  // Expects a trimmed string.
  // To explain the regex: We just look if there are spaces at the beginning OR
  // at the end of the string.

  assert::that(!std::regex_search(name, TRIMMED));

  // First of all, we check for errors with our new symbol.

  if (!std::regex_search(name, VALID_NAME)) {
    // Basically, everything with a leading number is not accepted.
<<<<<<< HEAD
    state.addError(
        "Symbol '" + name +
        "' does not have a qualified name.");// TODO add position parameter here
=======
      state.addErrorHereT("Symbol '%1' does not have a qualified name.", name);
>>>>>>> master
    return;
  }

  if (_table.find(name) != _table.end()) {
    // We also fail, if we define the symbol twice in a commit (which would
    // count as double definition in a file).
<<<<<<< HEAD
    state.addError("Symbol '" + name +
                   "' defined twice.");// TODO add position parameter here
=======
    state.addErrorHereT("Symbol '%1' defined twice.", name);
>>>>>>> master
    return;
  }

  // All conditions checked, insert!
  auto newEntry = TableEntry(name, replacement, position, behavior, type);

  // Now, we got to update our dependencies:
  // * All existing entries got to be updated with the new symbol
  // * The symbol itself needs to be updated with all others
  for (auto& i : _table) {
    i.second.checkDependency(newEntry);
    newEntry.checkDependency(i.second);
  }

  // You never know...
  newEntry.checkDependency(newEntry);

  _table.emplace(name, newEntry);
}
#include <iostream>
bool SymbolTable::doDfs(std::vector<std::string>& order,
                        std::unordered_set<std::string>& visited,
                        const std::string& startNode) {
  // This is a standard iterative DFS with included recording of the finish time
  // of the nodes and cycle detection. Was as fun as implementing Euler cycles
  // iteratively.
  // Why iterative? I do not trust the stack...
  // Note: This might still be optimizable in terms of additional data
  // structures to use.

  // I don't like pairs. But in this case, they were just too handy to resist
  // using them.
  using NodePosition = std::pair<std::string, std::size_t>;

  // The standard stack used for DFS.
  std::stack<std::string> toVisit;

<<<<<<< HEAD
  // The current path, i.e. the virtual and yet not so realistic callstack.
  // (used for finish order determination) But it ensures, that nodes are
  // finished as soon as we drop to their definition level in the stack (that's
  // what we need the number for).
  std::stack<NodePosition> currentPath;

  std::unordered_set<std::string> currentPathSet;

  // Then, we push the start node and the fun begins.
  toVisit.push(startNode);
  visited.insert(startNode);
  while (!toVisit.empty()) {
    auto curr = toVisit.top();
    toVisit.pop();
    // We have not visited the node in any run yet.

    // Let's add it to the stack.
    currentPath.push(NodePosition(curr, toVisit.size()));
    currentPathSet.insert(curr);

    for (const auto& j : _table.at(curr).dependencies()) {
      if (visited.find(j) == visited.end()) {
        // The nodes are added for further consideration. Pushing nodes
        // for more than one time on this is actually allowed.
        toVisit.push(j);
        // Then mark it as visited.
        visited.insert(j);
      }
      if (currentPathSet.find(j) != currentPathSet.end()) {
        return false;
      }
    }
    // It is like "descending in a callstack".
    while (!currentPath.empty() && currentPath.top().second == toVisit.size()) {
      // Node finished.
      auto finished = currentPath.top();
      currentPath.pop();
      currentPathSet.erase(currentPathSet.find(finished.first));
      order.push_back(finished.first);
    }
  }
  return true;
}

bool SymbolTable::checkCyclic(std::vector<std::string>& order) {
  // We need to check for cycles in the whole dependency graph of the symbols.
  std::unordered_set<std::string> visited;

  // We need this loop because the graph is not guaranteed to be connected. And
  // because it is a directed graph.
  for (const auto& i : _table) {
    if (visited.find(i.first) == visited.end()) {
      // At each unvisited node, we start our DFS.
      if (!doDfs(order, visited, i.first)) {
        // Found a cycle, it is not a DAG.
        return false;
=======
  // Just make a copy and iterate over all defined symbols. If they match,
  // replace them.
  for (int i = 0; i < _maximumRecursionDepth; ++i) {
    bool cont = false;
    // Actually, we could store all last results and check, if
    // one has already occured. Or just for one round... We
    // just have do differ from infinite recursion where one
    // pass gets us the same string again...
    for (const auto& entry : _table) {
      // Replace all symbol occurrences.
      // call the replacer function with replacement string & type
      const auto& replacement =
          //    the replacement string, the symbol type
          replacer(entry.second.first, entry.second.second);
      std::string newResult =
          std::regex_replace(result, makeRegex(entry.first), replacement);
      if (!cont && newResult != result) {
        cont = true;
>>>>>>> master
      }
    }
  }
  return true;
}

void SymbolTable::prepareSymbols(const std::vector<std::string>& order) {
  // We take the reverse order of the dfs (which is the nodes but topologically
  // sorted, as we got a DAG).
  for (auto& i : Utility::revertIterable(order)) {
    auto& entry = _table.at(i);
    std::string result = entry.specifiedReplacement();
    if (entry.behavior() == SymbolBehavior::STATIC) {
      for (const auto& j : entry.dependencies()) {
        result = std::regex_replace(
            result, entry.searchRegex(), entry.finalReplacement());
      }
    }
    entry.finalReplacement(result);
  }
}

bool SymbolTable::finalizeEntries() {
  // Simple top level description: We perform a DFS on the possibly disconnected
  // dependency graph, determine if it is a DAG and if yes, we use the finish
  // number ordered nodes to topologically traverse the dependency graph and
  // replace the symbols by their value.
  // TODO: revert changes, we now only check for cycles! (b/c line positions are
  // relative...) Or maybe, do a hybrid approach: replace as far as possible,
  // except for lines.
  std::vector<std::string> order;
  bool result = checkCyclic(order);
  if (result) {
    prepareSymbols(order);
  }
  _replacementOrder = order;
  std::reverse(_replacementOrder.begin(), _replacementOrder.end());
  return result;
}

std::string
SymbolTable::replaceSymbols(const std::string& source,
                            CompileState& state,
                            const ReplacementFunction& replacer) const {
  std::string result = source;

<<<<<<< HEAD
  // Just make a copy and iterate over all defined symbols. If they match,
  // replace them.
  for (const auto& name : _replacementOrder) {
    // Replace all symbol occurrences.
    // call the replacer function with replacement string & type
    const auto& entry = _table.at(name);
    auto replacement = entry.finalReplacement();
    if (entry.behavior() == SymbolBehavior::DYNAMIC) {
      replacement = replacer(entry.finalReplacement(), entry.type());
    }
    result = std::regex_replace(result, entry.searchRegex(), replacement);
  }
=======
  // If we come here, we have replaced too often and abort, suspecting an
  // infinite loop.
  state.addErrorHereT("Exceeded recursion replacement depth.");
>>>>>>> master

  return result;
}

void SymbolTable::replaceSymbols(std::vector<std::string>& source,
                                 CompileState& state,
                                 const ReplacementFunction& replacer) const {
  // We just replace the symbols for each of the vector elements.

  for (int i = 0; i < source.size(); i++) {
    source[i] = replaceSymbols(source[i], state, replacer);
  }
}

const SymbolTable::ReplacementFunction SymbolTable::SIMPLE_REPLACE = [](
    const std::string& repl, SymbolType type) -> std::string { return repl; };
*/