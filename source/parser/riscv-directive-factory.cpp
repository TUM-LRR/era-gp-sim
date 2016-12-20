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

#include "parser/riscv-directive-factory.hpp"

#include "parser/intermediate-representator.hpp"

#include "parser/compile-error-annotator.hpp"
#include "parser/constant-directive.hpp"
#include "parser/macro-directive.hpp"
#include "parser/macro-end-directive.hpp"
#include "parser/memory-definition-directive.hpp"
#include "parser/memory-reservation-directive.hpp"
#include "parser/section-directive.hpp"

using DirectivePtr = std::unique_ptr<IntermediateDirective>;

template <typename T>
static DirectivePtr createInternal(const LineInterval &lines,
                                   const std::vector<std::string> &labels,
                                   const std::string &name,
                                   const std::vector<std::string> &arguments) {
  return std::make_unique<T>(lines, labels, name, arguments);
}

// Function for `createMemoryDefinitionDirective`
template <typename T>
static const typename MemoryDefinitionDirective<T>::ProcessValuesFunction
    _processMemoryDefinitionValues =
        [](const std::vector<std::string> &values,
           std::size_t cellSize,
           const CompileErrorAnnotator &annotator,
           const std::function<void(T, std::size_t)> &handler) -> std::size_t {
  std::size_t currentPosition = 0;
  ExpressionCompiler<T> compiler =
      CLikeExpressionCompilers::createCLikeCompiler<T>();

  for (const auto &i : values) {
    if (i.empty()) {
      // Empty arguments are not allowed.
      annotator.addErrorHere("Argument is empty.");
    } else if (i.at(0) == '\"') {
      // It is a string if it begins with a "
      std::vector<T> temporaryData;
      if (StringParser::parseString(i, annotator, temporaryData)) {
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
      T returnData = compiler.compile(i, annotator);
      handler(returnData, currentPosition);
      currentPosition += cellSize;
    }
  }

  // We return our size estimate.
  return currentPosition;
};

template <typename T>
static DirectivePtr
createMemoryDefinitionDirective(const LineInterval &lines,
                                const std::vector<std::string> &labels,
                                const std::string &name,
                                const std::vector<std::string> &values) {
  return std::make_unique<MemoryDefinitionDirective<T>>(
      lines, labels, name, values, _processMemoryDefinitionValues<T>);
}


// Function for `createMemoryReservationDirective`
MemoryReservationDirective::ArgumentCompileFunction
    _memoryReservationArgumentCompile =
        [](const std::string &value,
           const CompileErrorAnnotator &annotator) -> std::size_t {
  return CLikeExpressionCompilers::CLikeCompilerU64.compile(value, annotator);
};

template <std::size_t cellSize>
static DirectivePtr
createMemoryReservationDirective(const LineInterval &lines,
                                 const std::vector<std::string> &labels,
                                 const std::string &name,
                                 const std::vector<std::string> &values) {
  return std::make_unique<MemoryReservationDirective>(
      lines, labels, name, cellSize, values, _memoryReservationArgumentCompile);
}


const std::unordered_map<
    std::string,
    std::function<DirectivePtr(const LineInterval &,
                               const std::vector<std::string> &,
                               const std::string &,
                               const std::vector<std::string> &)>>
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

void RiscVDirectiveFactory::create(const LineInterval &lines,
                                   const std::vector<std::string> &labels,
                                   const std::string &name,
                                   const std::vector<std::string> &arguments,
                                   IntermediateRepresentator &intermediate,
                                   const CompileErrorAnnotator &annotator) {
  DirectivePtr ptr;

  auto element = mapping.find(name);

  if (element == mapping.end()) {
    ptr = nullptr;
    annotator.addErrorHere("Unknown directive");
  } else {
    ptr = (element->second)(lines, labels, name, arguments);
    intermediate.insertCommandPtr(std::move(ptr), annotator);
  }
}
