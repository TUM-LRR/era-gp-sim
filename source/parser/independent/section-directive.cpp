/*
* ERASIM Assembler Interpreter
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

#include "parser/independent/section-directive.hpp"

#include <string>

#include "parser/common/compile-error-list.hpp"
#include "parser/independent/memory-allocator.hpp"
#include "parser/independent/section-tracker.hpp"

SectionDirective::SectionDirective(const CodePositionInterval& positionInterval,
                                   const PositionedStringVector& labels,
                                   const PositionedString& name,
                                   const PositionedStringVector& arguments)
: IntermediateDirective(positionInterval, labels, name) {
  _hasName = !arguments.empty();
  if (_hasName) {
    _section = arguments[0];
  }
}

void SectionDirective::allocateMemory(
    const AllocateMemoryImmutableArguments& immutable,
    CompileErrorList& errors,
    MemoryAllocator& allocator,
    SectionTracker& tracker) {
  // First of all, we check for errors: we may miss a section name or the
  // specified section does not exist.
  if (!_hasName) {
    errors.pushError(name().positionInterval(), "Section name missing!");
    return;
  }
  if (!allocator.has(_section.string())) {
    errors.pushError(_section.positionInterval(),
                     "Specified section non-existent!");
    return;
  }
  // Just set the section state to the current section. That's it.
  tracker.section(_section.string());
}

const PositionedString& SectionDirective::section() const noexcept {
  return _section;
}
bool SectionDirective::hasName() const noexcept {
  return _hasName;
}
