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

#include <iostream>
#include <string>

#include "arch/common/abstract-factory-types.hpp"
#include "arch/common/architecture-brewery.hpp"
#include "arch/common/information-interface.hpp"
#include "arch/common/node-factory-collection-maker.hpp"
#include "common/utility.hpp"

ArchitectureBrewery::ArchitectureBrewery(const ArchitectureFormula& formula)
: _formula(formula) {
}

Architecture ArchitectureBrewery::operator()() {
  return brew();
}

Architecture ArchitectureBrewery::brew() {
  Architecture architecture(_formula.getArchitectureName());
  for (const auto& extensionName : _formula) {
    if (!_hull.count(extensionName)) {
      TraversalStack stack;
      architecture.extendBy(_brew(extensionName, stack));
    }
  }

  architecture.validate();

  auto factories = NodeFactoryCollectionMaker::CreateFor(architecture);
  architecture.nodeFactories(factories);

  return architecture;
}

ExtensionInformation
ArchitectureBrewery::_brew(const std::string& extensionName,
                           TraversalStack& traversalStack) {
  auto iterator = _hull.find(extensionName);
  if (iterator != _hull.end()) {
    return iterator->second;
  }

  ExtensionInformation extension;

  // Put the extension in the stack to avoid backward edges
  // and thereby cycles in the graph. This avoids infinite recursion
  // in cyclc graphs where extensions depend on each other.
  traversalStack.emplace(extensionName);

  auto data = _load(extensionName);

  _loadDependencies(extension, data, traversalStack);
  _handleReset(extension, data);

  extension.deserialize(data);

  _hull[extensionName] = extension;

  assert(extension.isValid());

  // Unwind stack
  traversalStack.erase(extensionName);

  return extension;
}

void ArchitectureBrewery::_loadDependencies(ExtensionInformation& extension,
                                            InformationInterface::Format& data,
                                            TraversalStack& traversalStack) {
  // clang-format off
  Utility::doIfThere(data, "extends",
  [&extension, this, &traversalStack, &data] (auto& dependencies) {
    for (const auto& dependencyName : dependencies) {
      if (!traversalStack.count(dependencyName)) {
        extension.merge(this->_brew(dependencyName, traversalStack));
      } else {
        std::clog << "Detected a cyclic edge from "
                  << data["name"]
                  << " to "
                  << dependencyName
                  << std::endl;
      }
    }
  });
  // clang-format on
}

void ArchitectureBrewery::_handleReset(ExtensionInformation& extension,
                                       InformationInterface::Format& data) {
  Utility::doIfThere(data, "reset-instructions", [&extension](auto& reset) {
    if (reset) extension.clearInstructions();
  });

  Utility::doIfThere(data, "reset-units", [&extension](auto& reset) {
    if (reset) extension.clearUnits();
  });
}

InformationInterface::Format
ArchitectureBrewery::_load(const std::string extensionName) {
  // clang-format off
  auto data = InformationInterface::load(Utility::joinPaths(
      _formula.getPath(),
      extensionName,
      "config.json"
  ));
  // clang-format on

  assert(data.count("name"));

  return data;
}
