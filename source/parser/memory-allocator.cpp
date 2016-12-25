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

void MemoryAllocator::MemorySection::clear() {
  // Really, just setting it to zero.
  _currentSize = 0;
}

RelativeMemoryPosition
MemoryAllocator::MemorySection::allocateRelative(std::size_t size) {
  // We got to round up and so on...
  auto aligned =
      Utility::discreteCeiling(_currentSize, _definition.dataAlignment);

  // Now that we got the beginning of our memory section, we can add the size to
  // it.
  _currentSize = aligned + size;
  return RelativeMemoryPosition(_definition.name, aligned);
}

// Getters...
std::size_t MemoryAllocator::MemorySection::currentSize() const noexcept {
  return _currentSize;
}

std::size_t MemoryAllocator::MemorySection::currentPosition() const noexcept {
  return _currentPosition;
}

// Transforming functions...
std::size_t MemoryAllocator::MemorySection::absoluteAddress(
    const RelativeMemoryPosition& relative) const {
  return absoluteAddress(relative.offset);
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

std::size_t MemoryAllocator::calculateSize(bool finalize) {
  std::size_t position = 0;

  // As we have befriended the MemorySection, we may do this. Again, we use
  // rounding up to align.
  for (auto& i : _sections) {
    auto sectionAlign = i._definition.sectionAlignment;
    auto aligned = Utility::discreteCeiling(position, sectionAlign);
    if (finalize) {
      i._currentPosition = aligned;
    }
    position = aligned + i._currentSize;
  }
  return position;
}

std::size_t MemoryAllocator::calculatePositions() {
  return calculateSize(true);
}

std::size_t MemoryAllocator::estimateSize() {
  return calculateSize(false);
}

// Accessor functions...
MemoryAllocator::MemorySection& MemoryAllocator::
operator[](const std::string& name) {
  assert::that(has(name));
  return _sections[_nameToSection[name]];
}

MemoryAllocator::MemorySection& MemoryAllocator::operator[](std::size_t index) {
  assert::that(index >= 0);
  assert::that(index < _sections.size());
  return _sections[index];
}

const MemoryAllocator::MemorySection&
MemoryAllocator::at(const std::string& name) const {
  assert::that(has(name));
  return _sections.at(_nameToSection.at(name));
}

const MemoryAllocator::MemorySection&
MemoryAllocator::at(std::size_t index) const {
  assert::that(index >= 0);
  assert::that(index < _sections.size());
  return _sections.at(index);
}

std::size_t MemoryAllocator::absolutePosition(
    const RelativeMemoryPosition& relative) const {
  assert::that(has(relative.section));
  return _sections.at(_nameToSection.at(relative.section))
      .absoluteAddress(relative.offset);
}

bool MemoryAllocator::has(const std::string& name) const noexcept {
  return _nameToSection.find(name) != _nameToSection.end();
}
