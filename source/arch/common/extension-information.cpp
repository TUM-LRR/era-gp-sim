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


#include <algorithm>
#include <cassert>
#include <string>

#include "arch/common/extension-information.hpp"
#include "arch/common/instruction-set.hpp"
#include "common/utility.hpp"

ExtensionInformation::ExtensionInformation() noexcept = default;

ExtensionInformation::ExtensionInformation(const Information::Format& data) {
  _deserialize(data);
}

ExtensionInformation::ExtensionInformation(const std::string& name)
: _instructions(std::make_unique<InstructionSet>()) {
  // For constraint checking
  this->name(name);
}

ExtensionInformation::ExtensionInformation(const ExtensionInformation& other)
: _name(other._name)
, _endianness(other._endianness)
, _alignmentBehavior(other._alignmentBehavior)
, _wordSize(other._wordSize)
, _instructions(std::make_unique<InstructionSet>(*other._instructions))
, _units(other._units) {
}

ExtensionInformation::ExtensionInformation(
    ExtensionInformation&& other) noexcept
: ExtensionInformation() {
  swap(other);
}

ExtensionInformation& ExtensionInformation::
operator=(ExtensionInformation other) {
  swap(other);

  return *this;
}

ExtensionInformation::~ExtensionInformation() = default;

void ExtensionInformation::swap(ExtensionInformation& other) noexcept {
  using std::swap;

  swap(_name, other._name);
  swap(_endianness, other._endianness);
  swap(_alignmentBehavior, other._alignmentBehavior);
  swap(_wordSize, other._wordSize);
  swap(_instructions, other._instructions);
  swap(_units, other._units);
}

void swap(ExtensionInformation& first, ExtensionInformation& second) noexcept {
  first.swap(second);
}

ExtensionInformation& ExtensionInformation::
operator+=(const ExtensionInformation& other) {
  return merge(other);
}

ExtensionInformation ExtensionInformation::
operator+(const ExtensionInformation& other) const {
  auto temp = *this;
  temp += other;

  return temp;
}

ExtensionInformation&
ExtensionInformation::deserialize(const Information::Format& data) {
  _deserialize(data);
  return *this;
}

ExtensionInformation& ExtensionInformation::name(const std::string& name) {
  assert(!name.empty());
  _name = name;

  return *this;
}

const std::string& ExtensionInformation::getName() const noexcept {
  return _name;
}

ExtensionInformation& ExtensionInformation::endianness(Endianness endianness) {
  _endianness = endianness;
  return *this;
}

ExtensionInformation::Endianness ExtensionInformation::getEndianness() const
    noexcept {
  assert(hasEndianness());
  return *_endianness;
}

bool ExtensionInformation::hasEndianness() const noexcept {
  return static_cast<bool>(_endianness);
}

ExtensionInformation&
ExtensionInformation::alignmentBehavior(AlignmentBehavior alignmentBehavior) {
  _alignmentBehavior = alignmentBehavior;
  return *this;
}

ExtensionInformation::AlignmentBehavior
ExtensionInformation::getAlignmentBehavior() const noexcept {
  assert(hasAlignmentBehavior());
  return *_alignmentBehavior;
}

bool ExtensionInformation::hasAlignmentBehavior() const noexcept {
  return static_cast<bool>(_alignmentBehavior);
}


ExtensionInformation& ExtensionInformation::wordSize(size_t wordSize) {
  _wordSize = wordSize;
  return *this;
}


ExtensionInformation::size_t ExtensionInformation::getWordSize() const
    noexcept {
  assert(hasWordSize());
  return *_wordSize;
}

bool ExtensionInformation::hasWordSize() const noexcept {
  return static_cast<bool>(_wordSize);
}

ExtensionInformation&
ExtensionInformation::addInstructions(const InstructionSet& instructions) {
  assert(_instructions != nullptr);
  *_instructions += instructions;

  return *this;
}

ExtensionInformation&
ExtensionInformation::setInstructions(const InstructionSet& instructions) {
  assert(_instructions != nullptr);
  *_instructions += instructions;

  return *this;
}

const InstructionSet& ExtensionInformation::getInstructions() const noexcept {
  assert(_instructions != nullptr);
  return *_instructions;
}

ExtensionInformation& ExtensionInformation::addUnits(UnitList units) {
  assert(units.size() > 0);
  return addUnits<UnitList>(units);
}

ExtensionInformation&
ExtensionInformation::addUnit(const UnitInformation& unit) {
  _units.emplace_back(unit);

  return *this;
}

const ExtensionInformation::UnitContainer&
ExtensionInformation::getUnits() const noexcept {
  return _units;
}

ExtensionInformation& ExtensionInformation::merge(ExtensionList list) {
  assert(list.size() > 0);
  return merge<ExtensionList>(list);
}

ExtensionInformation&
ExtensionInformation::merge(const ExtensionInformation& otherExtension) {
  if (otherExtension._endianness) {
    _endianness = otherExtension._endianness;
  }

  if (otherExtension._alignmentBehavior) {
    _alignmentBehavior = otherExtension._alignmentBehavior;
  }

  if (otherExtension._wordSize) {
    _wordSize = otherExtension._wordSize;
  }

  *_instructions += otherExtension.getInstructions();
  Utility::concatenate(_units, otherExtension.getUnits());

  return *this;
}

bool ExtensionInformation::isValid() const noexcept {
  if (_name.empty()) return false;
  if (!_instructions->isValid()) return false;
  if (!Utility::allOf(_units, [](auto& unit) { return unit.isValid(); })) {
    return false;
  }

  return true;
}

bool ExtensionInformation::isValidBase() const noexcept {
  if (!hasEndianness()) return false;
  if (!hasAlignmentBehavior()) return false;
  if (!hasWordSize()) return false;

  // Check the basic stuff
  return isValid();
}

void ExtensionInformation::_deserialize(const Information::Format& data) {
  assert(data.count("name"));

  name(data["name"]);
  _parseEndianness(data);
  _parseAlignmentBehavior(data);

  auto wordSize = data.find("word-size");
  if (wordSize != data.end()) {
    _wordSize = static_cast<size_t>(*wordSize);
  }

  auto units = data.find("units");
  if (units != data.end()) {
    for (auto& unit : *units) {
      addUnit(static_cast<UnitInformation>(unit));
    }
  }

  auto instructions = data.find("instructions");
  if (instructions != data.end()) {
    addInstructions(static_cast<InstructionSet>(*instructions));
  }
}

void ExtensionInformation::_parseEndianness(const Information::Format& data) {
  auto endianness = data.find("endianness");
  if (endianness == data.end()) return;

  if (*endianness == "little") {
    _endianness = ArchitectureProperties::Endianness::LITTLE;
  } else if (*endianness == "big") {
    _endianness = ArchitectureProperties::Endianness::BIG;
  } else if (*endianness == "mixed") {
    _endianness = ArchitectureProperties::Endianness::MIXED;
  } else if (*endianness == "bi") {
    _endianness = ArchitectureProperties::Endianness::BI;
  } else {
    assert(false);
  }
}

void ExtensionInformation::_parseAlignmentBehavior(
    const Information::Format& data) {
  auto alignment = data.find("alignment-behavior");
  if (alignment == data.end()) return;

  if (*alignment == "strict") {
    _alignmentBehavior = ArchitectureProperties::AlignmentBehavior::STRICT;
  } else if (*alignment == "relaxed") {
    _alignmentBehavior = ArchitectureProperties::AlignmentBehavior::RELAXED;
  } else {
    assert(false);
  }
}
