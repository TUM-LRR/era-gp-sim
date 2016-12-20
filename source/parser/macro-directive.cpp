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

#include "parser/compile-state.hpp"
#include "parser/intermediate-instruction.hpp"
#include "parser/macro-directive.hpp"

MacroDirective::MacroDirective(const LineInterval& lines,
                               const std::vector<std::string>& labels,
                               const std::string& name,
                               const std::vector<std::string>& arguments)
: IntermediateDirective(lines, labels, name)
, _macroName(arguments.size() > 0 ? arguments[0] : "")
, _macroParameters(arguments.size() > 0 ? arguments.begin() + 1
                                        : arguments.end(),
                   arguments.end())
, _operations() {
}

MacroDirective::MacroDirective(const LineInterval& lines,
                               const std::vector<std::string>& labels,
                               const std::string& name,
                               const std::string& macroName,
                               const std::vector<std::string>& macroParameters)
: IntermediateDirective(lines, labels, name)
, _macroName(macroName)
, _macroParameters(macroParameters)
, _operations() {
}

void MacroDirective::insert(IntermediateOperationPointer pointer) {
  // Remember index of the first instruction so we can use its address for
  // labels.
  if (_firstInstruction < 0 &&
      pointer->getType() == IntermediateOperation::Type::INSTRUCTION) {
    _firstInstruction = _operations.size();
  }
  _operations.push_back(std::move(pointer));
}

void MacroDirective::execute(FinalRepresentation& finalRepresentator,
                             const SymbolTable& table,
                             const SyntaxTreeGenerator& generator,
                             CompileState& state,
                             MemoryAccess& memoryAccess) {
  if (macroName().length() == 0) {
    state.addError("Missing macro name.");
  }
  _macroParameters.validate(state);
  state.registerMacro(*this);
}

TargetSelector MacroDirective::newTarget() const {
  return TargetSelector::THIS;
}

IntermediateExecutionTime MacroDirective::executionTime() const {
  return IntermediateExecutionTime::BEFORE_ALLOCATION;
}

const std::string& MacroDirective::macroName() const {
  return _macroName;
}

/**
 * Returns number of operations.
 */
size_t MacroDirective::getOperationCount() const {
  return _operations.size();
}

std::pair<size_t, size_t> MacroDirective::getParameterCount() const {
  return _macroParameters.getParameterCount();
}

/**
 * Returns if an instance of the macro is currently compiling. Used to detect
 * cyclic macro calls.
 */
bool MacroDirective::isCompiling() {
  return _isCompiling;
}

IntermediateOperationPointer
MacroDirective::getOperation(size_t index,
                             const std::vector<std::string>& arguments) const {
  IntermediateOperationPointer ptr = _operations[index]->clone();

  if (ptr == nullptr) return ptr;

  _macroParameters.insertParameters(ptr, arguments);
  return std::move(ptr);
}

int MacroDirective::firstInstructionIndex() const {
  return _firstInstruction;
}

const std::string& MacroDirective::getOperationName(size_t index) const {
  return _operations[index]->name();
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
      // If default value is supplied, split argument into name and value.
      defaultVal = i->substr(equalPos + 1);
      name = i->substr(0, equalPos);
    } else {
      // Otherwise use whole string as value and increase the minimum amount of
      // parameters.
      name = *i;
      _minParams++;
    }

    _params.emplace_back(std::move(name), std::move(defaultVal));
  }
}

void MacroDirective::MacroParameters::validate(CompileState& state) const {
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
    const std::vector<std::string>& values) const {
  // Since macros are identified by name and argument count, this function
  // should always be called with a valid size of `values`.
  assert::that(values.size() >= _minParams && values.size() <= _params.size());

  for (int i = 0; i < _params.size(); i++) {
    const std::string& name{_params[i].first};
    const std::string& value{i >= values.size() ? *_params[i].second
                                                : values[i]};
    operation->insertIntoArguments(name, value);
  }
}
