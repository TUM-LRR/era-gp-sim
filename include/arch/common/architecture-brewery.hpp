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

#ifndef ERAGPSIM_ARCH_COMMON_ARCHITECTURE_BREWERY_HPP
#define ERAGPSIM_ARCH_COMMON_ARCHITECTURE_BREWERY_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "arch/common/node-factory-collection.hpp"
#include "arch/common/architecture-brewery.hpp"
#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/extension-information.hpp"

/**
 * Class to brew (load) an architecture.
 *
 * This class is responsible for loading architecture definitions from disk and
 * deserializes an `Architecture` instance from it. It takes as input an
 * architecture formula, which contains information about the architecture's
 * name and requested extensions. It then performs a variant of
 * depth-first-search to traverse the extension graph. For each extension in
 * the formula, its dependencies are resolved and the corresponding extension
 * files are loaded from disk. If the extensions have dependencies themselves
 * the graph is traversed further. While the algorithm is running, the
 * transitive hull of all nodes (extensions) loaded so far are kept track of to
 * avoid multiple loads from disk and re-evaluation of dependencies. I.e., once
 * an extension's dependencies have been processed and the extension is complete
 * it is marked, such that dependents of it must not re-load it. Furthermore,
 * to avoid cycles in the graph, a "traversal stack" of nodes in the current
 * "dependency queue" is maintained. Whenever a vertex is visited that is not
 * yet complete, it is put into the traversal stack. When traversing a vertex's
 * dependencies (i.e. adjacent edges), nodes in the traversal stack are not
 * revisited. Basically, backward edges are not followed. This ensures that even
 * if the actual dependency graph is not a DAG, the *effective* graph will be a
 * DAG.
 *
 * The class has a functor-like interface.
 */
class ArchitectureBrewery {
 public:
  /**
   * Constructs a new Brewery ready to brew an architecture from the given
   * formula.
   *
   * \param formula The architecture formula to brew from.
   */
  explicit ArchitectureBrewery(const ArchitectureFormula& formula);

  /**
   * Runs the graph algorithm to produce the Architecture object.
   *
   * Calls `brew()`.
   *
   * \return The finished (brewed) Architecture.
   *
   * \see brew()
   */
  Architecture operator()();

  /**
   * Runs the graph algorithm to produce the Architecture object.
   *
   * \return The finished (brewed) Architecture.
   */
  Architecture brew();

 private:
  using TransitiveHull = std::unordered_map<std::string, ExtensionInformation>;
  using TraversalStack = std::unordered_set<std::string>;

  /**
   * The main recursive implementation of the graph traversal.
   *
   * If the extension has already been created, it is returned. Otherwise
   * a new extension is instantiated. Then the extension's data is loaded.
   * Then its dependencies are resolved and merged into the empty extension.
   * Finally the extension is deserialized from the rest of the data.
   *
   * \param extensionName The name of the extension currenlty under inspection.
   * \param traversalStack The stack of extensions currently under construction.
   *
   * \return The finished extension.
   */
  ExtensionInformation
  _brew(const std::string& extensionName, TraversalStack& traversalStack);

  /**
   * Refactors the loading of dependencies (traversing adjacent edges).
   *
   * \param extension The current extension.
   * \param data The data to load from.
   * \param traversalStack The stack of extensions currently under construction.
   */
  void _loadDependencies(ExtensionInformation& extension,
                         InformationInterface::Format& data,
                         TraversalStack& traversalStack);

  /**
   * Refactors the resetting of units and instructions after loading an
   * extension.
   *
   * \param extension The current extension.
   * \param data The data to load from.
   */
  void _handleReset(ExtensionInformation& extension,
                    InformationInterface::Format& data);

  /**
   * Loads the configuration data of the extension with the given name from
   * disk.
   *
   * \param extensionName The name of the extension to load.
   *
   * \return Data in the information interface's format.
   */
  InformationInterface::Format _load(const std::string extensionName);

  /** Extensions that have already been visited. */
  TransitiveHull _hull;

  /** The formula to brew. */
  ArchitectureFormula _formula;
};

#endif /* ERAGPSIM_ARCH_COMMON_ARCHITECTURE_BREWERY_HPP */
