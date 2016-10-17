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

#include "parser/constant-directive.hpp"
#include "parser/macro-directive.hpp"
#include "parser/macro-end-directive.hpp"
#include "parser/section-directive.hpp"

using DirectivePtr = std::unique_ptr<IntermediateDirective>;

template <typename T>
static DirectivePtr createInternal(const LineInterval &lines,
                                   const std::vector<std::string> &labels,
                                   const std::string &name,
                                   const std::vector<std::string> &arguments) {
  return DirectivePtr{new T(lines, labels, name, arguments)};
}

// SectionDirective has a different constructor. Will probably change this
// during refactoring
static DirectivePtr
createSectionInternal(const LineInterval &lines,
                      const std::vector<std::string> &labels,
                      const std::string &name,
                      const std::vector<std::string> &arguments) {
  return DirectivePtr{new SectionDirective(lines, labels, name, arguments[0])};
}

const std::unordered_map<std::string,
                         DirectivePtr (*)(const LineInterval &,
                                          const std::vector<std::string> &,
                                          const std::string &,
                                          const std::vector<std::string> &)>
    RiscVDirectiveFactory::mapping{{"section", createSectionInternal},
                                   {"macro", createInternal<MacroDirective>},
                                   {"endm", createInternal<MacroEndDirective>},
                                   {"equ", createInternal<ConstantDirective>}};

void RiscVDirectiveFactory::create(const LineInterval &lines,
                                   const std::vector<std::string> &labels,
                                   const std::string &name,
                                   const std::vector<std::string> &arguments,
                                   IntermediateRepresentator &intermediate,
                                   CompileState &state) {
  DirectivePtr ptr;

  auto element = mapping.find(name);

  if (element == mapping.end()) {
    ptr = nullptr;
    state.addError("Unknown directive", state.position);
  } else {
    ptr = (element->second)(lines, labels, name, arguments);
    intermediate.insertCommandPtr(std::move(ptr), state);
  }
}
