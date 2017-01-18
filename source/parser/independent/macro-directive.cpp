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

#include "parser/independent/macro-directive.hpp"

#include <cstddef>
#include <string>
#include <utility>

#include "common/utility.hpp"
#include "parser/common/compile-error-list.hpp"
#include "parser/independent/intermediate-instruction.hpp"
#include "parser/independent/macro-directive-table.hpp"

MacroDirective::MacroDirective(const CodePositionInterval& positionInterval,
                               const PositionedStringVector& labels,
                               const PositionedString& name,
                               const PositionedStringVector& arguments)
: IntermediateDirective(positionInterval, labels, name)
, _macroName(arguments.empty() ? PositionedString() : arguments[0])
, _macroParameters(arguments.size() > 0 ? arguments.begin() + 1
                                        : arguments.end(),
                   arguments.end())
, _operations() {
}

MacroDirective::MacroDirective(const CodePositionInterval& positionInterval,
                               const PositionedStringVector& labels,
                               const PositionedString& name,
                               const PositionedString& macroName,
                               const PositionedStringVector& macroParameters)
: IntermediateDirective(positionInterval, labels, name)
, _macroName(macroName)
, _macroParameters(macroParameters)
, _operations() {
}

void MacroDirective::insert(const IntermediateOperationPointer& pointer) {
  // Remember index of the first instruction so we can use its address for
  // labels.
  if (_firstInstruction < 0 &&
      Utility::isInstance<IntermediateInstruction>(pointer)) {
    _firstInstruction = _operations.size();
  }
  _operations.emplace_back(pointer);
}

void MacroDirective::precompile(const PrecompileImmutableArguments& immutable,
                                CompileErrorList& errors,
                                MacroDirectiveTable& macroTable) {
  if (macroName().string().empty()) {
    errors.pushError(name().positionInterval(), "Missing macro name.");
    return;
  }
  _macroParameters.validate(errors);
  auto success = macroTable.insert(*this);
  if (!success) {
    errors.pushError(macroName().positionInterval(),
                     "Macro '%1' already exists.",
                     macroName().string());
  }
}

TargetSelector MacroDirective::newTarget() const {
  return TargetSelector::THIS;
}

const PositionedString& MacroDirective::macroName() const noexcept {
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
                             const PositionedStringVector& arguments) const {
  IntermediateOperationPointer ptr = _operations[index]->clone();

  if (!ptr) {
    return ptr;
  }

  _macroParameters.insertParameters(ptr, arguments);
  return ptr;
}

int MacroDirective::firstInstructionIndex() const {
  return _firstInstruction;
}

const PositionedString& MacroDirective::getOperationName(size_t index) const {
  return _operations[index]->name();
}

MacroDirective::MacroParameters::MacroParameters(
    PositionedStringVector::const_iterator begin,
    PositionedStringVector::const_iterator end)
: _minParams(0) {
  for (auto i = begin; i != end; ++i) {
    Optional<PositionedString> defaultVal;
    PositionedString name;

    const auto& parameterString = i->string();

    // Search for default argument by finding the '=' character.
    auto equalPos = parameterString.find('=');

    if (equalPos != std::string::npos) {
      // If default value is supplied, split argument into name and value.
      defaultVal = PositionedString(parameterString.substr(equalPos + 1),
                                    i->positionInterval());
      name = PositionedString(parameterString.substr(0, equalPos),
                              i->positionInterval());
    } else {
      // Otherwise use whole string as value and increase the minimum amount of
      // parameters.
      name = PositionedString(parameterString, i->positionInterval());
      _minParams++;
    }

    _params.emplace_back(name, defaultVal);
  }
}

MacroDirective::MacroParameters::MacroParameters(
    const PositionedStringVector& arguments)
: MacroParameters(arguments.begin(), arguments.end()) {
}

void MacroDirective::MacroParameters::validate(CompileErrorList& errors) const {
  bool containedDefault = false;
  MacroParameter last;
  for (auto param : _params) {
    // Check for empty names or default values
    if (param.first.string().empty() ||
        (param.second && param.second->string().empty())) {
      errors.pushError(param.first.positionInterval(),
                       "Malformed macro argument list.");
      return;
    }

    // Check for missing default values after a default value.
    if (param.second) {
      containedDefault = true;
    }
    if (containedDefault && !param.second) {
      errors.pushError(last.second->positionInterval(),
                       "Default macro argument values have to be placed last.");
      return;
    }

    last = param;
  }
}

void MacroDirective::MacroParameters::insertParameters(
    IntermediateOperationPointer& operation,
    const PositionedStringVector& values) const {
  // Since macros are identified by name and argument count, this function
  // should always be called with a valid size of `values`.
  assert::that(values.size() >= _minParams && values.size() <= _params.size());

  for (auto i : Utility::range<size_t>(0, _params.size())) {
    const auto& name = _params[i].first;
    const auto& value = i >= values.size() ? *_params[i].second : values[i];
    operation->insertIntoArguments(name, value);
  }
}
