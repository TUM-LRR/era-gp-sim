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
#include "parser/compile-error-annotator.hpp"
#include "parser/memory-allocator.hpp"
#include "parser/section-tracker.hpp"

SectionDirective::SectionDirective(
    const LineInterval& lines,
    const std::vector<PositionedString>& labels,
    const PositionedString& name,
    const std::vector<PositionedString>& arguments)
: IntermediateDirective(lines, labels, name) {
  _hasName = !arguments.empty();
  if (_hasName) {
    _section = arguments[0];
  }
}

void SectionDirective::execute(const ExecuteImmutableArguments& immutable,
                               const CompileErrorAnnotator& annotator,
                               FinalRepresentation& finalRepresentator,
                               MemoryAccess& memoryAccess) {
}

void SectionDirective::allocateMemory(
    const PreprocessingImmutableArguments& immutable,
    const CompileErrorAnnotator& annotator,
    MemoryAllocator& allocator,
    SectionTracker& tracker) {
  if (!_hasName) {
    annotator.addError(name().positionInterval(), "Section name missing!");
    return;
  }
  if (!allocator.has(_section.string())) {
    annotator.addError(_section.positionInterval(),
                       "Specified section non-existent!");
    return;
  }
  // Just set the section state to the current section. That's it.
  tracker.section(_section.string());
}
