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

#include <string>

#include "arch/common/architecture-brewery.hpp"
#include "arch/common/information-interface.hpp"
#include "common/utility.hpp"

Architecture::Brewery::Brewery(const Architecture::Formula& formula)
: _formula(formula) {
}

Architecture Architecture::Brewery::operator()() {
  return brew();
}

Architecture Architecture::Brewery::brew() {
  Architecture architecture;
  for (auto& extensionName : _formula) {
    if (!_hull.count(extensionName)) {
      TraversalStack stack;
      architecture.extend(_brew(extensionName, stack));
    }
  }

  return architecture.validate();
}

ExtensionInformation Architecture::Brewery::_brew(
    const std::string& extensionName, TraversalStack& traversalStack) {
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

  extension.deserialize(data["information"]);

  _hull[extensionName] = extension;

  assert(extension.isValid());

  // Unwind stack
  traversalStack.erase(extensionName);

  return extension;
}

void Architecture::Brewery::_loadDependencies(
    ExtensionInformation& extension,
    const InformationInterface::Format& data,
    TraversalStack& traversalStack) {
  // clang-format off
  Utility::doIfThere(data, "extends",
  [&extension, this, &traversalStack] (auto& dependencies) {
    for (auto& dependencyName : dependencies) {
      assert(!traversalStack.count(dependencyName));
      if (!traversalStack.count(dependencyName)) {
        extension.merge(_brew(dependencyName, traversalStack));
      }
    }
  });
  // clang-format on
}

void Architecture::Brewery::_handleReset(
    ExtensionInformation& extension, const InformationInterface::Format& data) {
  Utility::doIfThere(data, "reset-instructions", [&extension](auto& reset) {
    if (reset) extension.clearInstructions();
  });

  Utility::doIfThere(data, "reset-units", [&extension](auto& reset) {
    if (reset) extension.clearUnits();
  });
}

InformationInterface::Format Architecture::Brewery::_load(
    const std::string extensionName) {
  auto data = InformationInterface::load(
      Utility::joinToRoot(_formula.getArchitectureName(), extensionName));

  assert(data.count("name"));
  assert(data.count("information"));

  return data;
}
