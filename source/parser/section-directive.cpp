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

#include "parser/section-directive.hpp"
#include "parser/compile-state.hpp"
#include "parser/memory-allocator.hpp"

SectionDirective::SectionDirective(const LineInterval& lines,
                                   const std::vector<std::string>& labels,
                                   const std::string& name,
                                   const std::vector<std::string>& arguments)
: IntermediateDirective(lines, labels, name) {
  _hasName = arguments.size() > 0;
  if (_hasName) _section = arguments[0];
}

void SectionDirective::execute(FinalRepresentation& finalRepresentator,
                               const SymbolTable& table,
                               const SyntaxTreeGenerator& generator,
                               CompileState& state,
                               MemoryAccess& memoryAccess) {
}

void SectionDirective::allocateMemory(const Architecture& architecture,
                                      MemoryAllocator& allocator,
                                      CompileState& state) {
  if (!_hasName) {
    state.addError(P_TR("Section name missing!"));
    return;
  }
  if (!allocator.has(_section)) {
    state.addError("Specified section non-existent!");
    return;
  }
  // Just set the section state to the current section. That's it.
  state.section = _section;
}
