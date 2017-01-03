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

#include "parser/riscv/riscv-directive-factory.hpp"

#include "parser/independent/intermediate-representator.hpp"

#include "parser/common/compile-error-list.hpp"
#include "parser/independent/constant-directive.hpp"
#include "parser/independent/macro-directive.hpp"
#include "parser/independent/macro-end-directive.hpp"
#include "parser/independent/memory-definition-directive.hpp"
#include "parser/independent/memory-reservation-directive.hpp"
#include "parser/independent/section-directive.hpp"

using DirectivePtr = std::shared_ptr<IntermediateDirective>;

namespace {
template <typename T>
DirectivePtr createInternal(const CodePositionInterval &positionInterval,
                            const PositionedStringVector &labels,
                            const PositionedString &name,
                            const PositionedStringVector &arguments) {
  return std::make_shared<T>(positionInterval, labels, name, arguments);
}

// Function for `createMemoryDefinitionDirective`
template <typename T>
const typename MemoryDefinitionDirective<T>::ProcessValuesFunction
    _processMemoryDefinitionValues =
        [](const PositionedStringVector &values,
           const SymbolReplacer &replacer,
           std::size_t cellSize,
           CompileErrorList &errors,
           const std::function<void(T, std::size_t)> &handler) -> std::size_t {
  std::size_t currentPosition = 0;
  ExpressionCompiler<T> compiler =
      CLikeExpressionCompilers::createCLikeCompiler<T>();

  for (const auto &value : values) {
    if (value.string().empty()) {
      // Empty arguments are not allowed.
      errors.pushError(value.positionInterval(), "Argument is empty.");
    } else if (value.string().at(0) == '\"') {
      // It is a string if it begins with a "
      std::vector<T> temporaryData;
      if (StringParser::parseString(value, errors, temporaryData)) {
        // We add each character of the string to our output.
        for (const auto &j : temporaryData) {
          handler(j, currentPosition);
          currentPosition += cellSize;
        }

        // Final 0 byte/character.
        handler(0, currentPosition);
        currentPosition += cellSize;
      }
    } else {
      // If it is no a string, we regularly parse it.
      T returnData = compiler.compile(value, replacer, errors);
      handler(returnData, currentPosition);
      currentPosition += cellSize;
    }
  }

  // We return our size estimate.
  return currentPosition;
};

template <typename T>
DirectivePtr
createMemoryDefinitionDirective(const CodePositionInterval &positionInterval,
                                const PositionedStringVector &labels,
                                const PositionedString &name,
                                const PositionedStringVector &values) {
  return std::make_shared<MemoryDefinitionDirective<T>>(
      positionInterval,
      labels,
      name,
      values,
      _processMemoryDefinitionValues<T>);
}

// Function for `createMemoryReservationDirective`
MemoryReservationDirective::ArgumentCompileFunction
    _memoryReservationArgumentCompile =
        [](const PositionedString &value,
           const SymbolReplacer &replacer,
           CompileErrorList &errors) -> std::size_t {
  return CLikeExpressionCompilers::CLikeCompilerU64.compile(
      value, replacer, errors);
};


template <std::size_t cellSize>
DirectivePtr
createMemoryReservationDirective(const CodePositionInterval &positionInterval,
                                 const PositionedStringVector &labels,
                                 const PositionedString &name,
                                 const PositionedStringVector &values) {
  return std::make_shared<MemoryReservationDirective>(
      positionInterval,
      labels,
      name,
      values,
      _memoryReservationArgumentCompile,
      cellSize);
}
};

const std::unordered_map<
    std::string,
    std::function<DirectivePtr(const CodePositionInterval &,
                               const PositionedStringVector &,
                               const PositionedString &,
                               const PositionedStringVector &)>>
    RiscVDirectiveFactory::mapping{
        {"section", createInternal<SectionDirective>},
        {"macro", createInternal<MacroDirective>},
        {"endm", createInternal<MacroEndDirective>},
        {"equ", createInternal<ConstantDirective>},
        {"byte", createMemoryDefinitionDirective<uint8_t>},
        {"half", createMemoryDefinitionDirective<uint16_t>},
        {"word", createMemoryDefinitionDirective<uint32_t>},
        {"dword", createMemoryDefinitionDirective<uint64_t>},
        {"resb", createMemoryReservationDirective<1>},
        {"resh", createMemoryReservationDirective<2>},
        {"resw", createMemoryReservationDirective<4>},
        {"resd", createMemoryReservationDirective<8>}};

void RiscVDirectiveFactory::create(const CodePositionInterval &positionInterval,
                                   const PositionedStringVector &labels,
                                   const PositionedString &name,
                                   const PositionedStringVector &arguments,
                                   IntermediateRepresentator &intermediate,
                                   CompileErrorList &errors) {
  DirectivePtr ptr;

  auto element = mapping.find(name.string());

  if (element == mapping.end()) {
    ptr = nullptr;
    errors.pushError(name.positionInterval(), "Unknown directive");
  } else {
    ptr = (element->second)(positionInterval, labels, name, arguments);
    intermediate.insertCommandPointer(ptr, errors);
  }
}
