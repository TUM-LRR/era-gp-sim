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

#include "parser/memory-allocator.hpp"

RelativeMemoryPosition::RelativeMemoryPosition(const std::string& section,
                                               std::size_t offset)
: section(section), offset(offset) {
}

RelativeMemoryPosition::RelativeMemoryPosition()
: RelativeMemoryPosition("", 0) {
}

MemorySectionDefinition::MemorySectionDefinition(const std::string& name,
                                                 std::size_t sectionAlignment,
                                                 std::size_t dataAlignment)
: name(name), sectionAlignment(sectionAlignment), dataAlignment(dataAlignment) {
  assert::that(sectionAlignment > 0);
  assert::that(dataAlignment > 0);
  assert::that(sectionAlignment % dataAlignment == 0);
}

MemorySectionDefinition::MemorySectionDefinition(const std::string& name,
                                                 std::size_t alignment)
: MemorySectionDefinition(name, alignment, alignment) {
}

MemorySectionDefinition::MemorySectionDefinition(const std::string& name)
: MemorySectionDefinition(name, 1, 1) {
}

void MemoryAllocator::MemorySection::clear() {
  _currentSize = 0;
}

std::size_t MemoryAllocator::MemorySection::allocateRelative(std::size_t size) {
  auto aligned =
      Utility::discreteCeiling(_currentSize, _definition.dataAlignment) *
      _definition.dataAlignment;
  _currentSize = aligned + size;
  return aligned;
}

std::size_t MemoryAllocator::MemorySection::currentSize() const {
  return _currentSize;
}

std::size_t MemoryAllocator::MemorySection::currentPosition() const {
  return _currentPosition;
}

std::size_t
MemoryAllocator::MemorySection::absoluteAddress(std::size_t relative) const {
  return relative + _currentPosition;
}

MemoryAllocator::MemorySection::MemorySection(
    const MemorySectionDefinition& definition)
: _definition(definition), _currentSize(0), _currentPosition(0) {
}

MemoryAllocator::MemoryAllocator(
    const std::vector<MemorySectionDefinition>& sectionDefinitions) {
  _sections.reserve(sectionDefinitions.size());
  for (const auto& i : sectionDefinitions) {
    auto section = MemorySection(i);
    _nameToSection[i.name] = _sections.size();
    _sections.push_back(section);
  }
}

void MemoryAllocator::clear() {
  for (auto& i : _sections) {
    i.clear();
  }
}

std::size_t MemoryAllocator::calculatePositions() {
  std::size_t position = 0;
  for (auto& i : _sections) {
    auto sectionAlign = i._definition.sectionAlignment;
    auto aligned =
        Utility::discreteCeiling(position, sectionAlign) * sectionAlign;
    i._currentPosition = aligned;
    position = aligned + i._currentSize;
  }
  return position;
}

MemoryAllocator::MemorySection& MemoryAllocator::operator[](std::string name) {
  assert::that(_nameToSection.find(name) != _nameToSection.end());
  return _sections[_nameToSection[name]];
}

MemoryAllocator::MemorySection& MemoryAllocator::operator[](std::size_t index) {
  assert::that(index >= 0);
  assert::that(index < _sections.size());
  return _sections[index];
}

std::size_t MemoryAllocator::absolutePosition(
    const RelativeMemoryPosition& relative) const {
  return _sections.at(_nameToSection.at(relative.section))
      .absoluteAddress(relative.offset);
}
