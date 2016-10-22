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

#include "parser/macro-directive.hpp"

void MacroDirective::execute(FinalRepresentation& finalRepresentator,
                             const SymbolTable& table,
                             const SyntaxTreeGenerator& generator,
                             CompileState& state,
                             MemoryAccess& memoryAccess) {
  _macroParameters.validate(state);
  state.registerMacro(*this);
}

MacroDirective::MacroParameters::MacroParameters(
    std::vector<std::string>::const_iterator begin,
    std::vector<std::string>::const_iterator end)
: _minParams(0) {
  for (auto i = begin; i != end; ++i) {
    Optional<std::string> defaultVal;
    std::string name;

    // Search for default argument by finding the '=' character.
    auto equalPos = i->find('=');

    if (equalPos != std::string::npos) {
      defaultVal = i->substr(equalPos + 1);
      name = i->substr(0, equalPos);
    } else {
      name = *i;
      _minParams++;
    }

    _params.emplace_back(std::move(name), std::move(defaultVal));
  }
}

void MacroDirective::MacroParameters::validate(CompileState& state) {
  bool containedDefault{false};
  for (auto param : _params) {
    // Check for empty names or default values
    if (param.first.size() == 0 ||
        (param.second && param.second->size() == 0)) {
      state.addError("Malformed macro argument list!");
      return;
    }

    // Check for missing default values after a default value.
    if (param.second) containedDefault = true;
    if (containedDefault && !param.second) {
      state.addError("Default macro argument values have to be placed last!");
      return;
    }
  }
}

void MacroDirective::MacroParameters::insertParameters(
    IntermediateOperationPointer& operation,
    const std::vector<std::string>& values) {
  // Since macros are identified by name and argument count, this function
  // should always be called with a valid size of `values`.
  assert::that(values.size() >= _minParams && values.size() <= _params.size());

  for (int i = 0; i < _params.size(); i++) {
    const std::string& name{_params[i].first};
    const std::string& value{i >= values.size() ? *_params[i].second
                                                : values[i]};
    operation->insertIntoArguments(name, value);
  }
  // return std::move(operation);
}
