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

ExtensionInformation::ExtensionInformation(InformationInterface::Format& data) {
  _deserialize(data);
}

ExtensionInformation::ExtensionInformation(const std::string& name)
: _name(name), _wordSize(0), _byteSize(0) {
}

bool ExtensionInformation::operator==(const ExtensionInformation& other) const
    noexcept {
  if (this->_name != other._name) return false;
  if (this->_endianness != other._endianness) return false;
  if (this->_alignmentBehavior != other._alignmentBehavior) return false;
  if (this->_wordSize != other._wordSize) return false;
  if (this->_byteSize != other._byteSize) return false;
  if (this->_instructions != other._instructions) return false;
  if (this->_units != other._units) return false;

  return true;
}

bool ExtensionInformation::operator!=(const ExtensionInformation& other) const
    noexcept {
  return !(*this == other);
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
ExtensionInformation::deserialize(InformationInterface::Format& data) {
  _deserialize(data);
  return *this;
}

ExtensionInformation& ExtensionInformation::name(const std::string& name) {
  assert(!name.empty());
  _name = name;

  return *this;
}

const std::string& ExtensionInformation::getName() const {
  assert(hasName());
  return _name;
}

bool ExtensionInformation::hasName() const noexcept {
  return !_name.empty();
}

ExtensionInformation& ExtensionInformation::endianness(Endianness endianness) {
  _endianness = endianness;
  return *this;
}

ExtensionInformation::Endianness ExtensionInformation::getEndianness() const {
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
ExtensionInformation::getAlignmentBehavior() const {
  assert(hasAlignmentBehavior());
  return *_alignmentBehavior;
}

bool ExtensionInformation::hasAlignmentBehavior() const noexcept {
  return static_cast<bool>(_alignmentBehavior);
}


ExtensionInformation& ExtensionInformation::wordSize(word_size_t wordSize) {
  _wordSize = wordSize;
  return *this;
}


ExtensionInformation::word_size_t ExtensionInformation::getWordSize() const {
  assert(hasWordSize());
  return _wordSize;
}

bool ExtensionInformation::hasWordSize() const noexcept {
  return _wordSize > 0;
}

ExtensionInformation& ExtensionInformation::byteSize(byte_size_t byteSize) {
  _byteSize = byteSize;
  return *this;
}


ExtensionInformation::byte_size_t ExtensionInformation::getByteSize() const
    noexcept {
  assert(hasByteSize());
  return _byteSize;
}

bool ExtensionInformation::hasByteSize() const noexcept {
  return _byteSize > 0;
}

ExtensionInformation&
ExtensionInformation::addInstructions(const InstructionSet& instructions) {
  _instructions += instructions;

  return *this;
}

ExtensionInformation&
ExtensionInformation::setInstructions(const InstructionSet& instructions) {
  _instructions = instructions;

  return *this;
}

ExtensionInformation& ExtensionInformation::clearInstructions() {
  _instructions.clear();

  return *this;
}

const InstructionSet& ExtensionInformation::getInstructions() const noexcept {
  return _instructions;
}

bool ExtensionInformation::hasInstructions() const noexcept {
  return !_instructions.isEmpty();
}

ExtensionInformation& ExtensionInformation::addUnits(UnitList units) {
  assert(units.size() > 0);
  return addUnits<UnitList>(units);
}

ExtensionInformation&
ExtensionInformation::addUnit(const UnitInformation& unit) {
  // If the unit is already present, we merge the units
  // (i.e. form the union of the registers), else just insert the unit
  auto iterator = _units.find(unit);
  if (iterator != _units.end()) {
    auto combination = *iterator + unit;
    _units.erase(iterator);
    _units.emplace(combination);
  } else {
    _units.emplace(unit);
  }

  return *this;
}

ExtensionInformation& ExtensionInformation::clearUnits() noexcept {
  _units.clear();
  return *this;
}

const UnitContainer& ExtensionInformation::getUnits() const noexcept {
  return _units;
}

bool ExtensionInformation::hasUnits() const noexcept {
  return !_units.empty();
}

ExtensionInformation& ExtensionInformation::merge(ExtensionList list) {
  assert(list.size() > 0);
  return merge<ExtensionList>(list);
}

ExtensionInformation&
ExtensionInformation::merge(const ExtensionInformation& other) {
  if (other.hasEndianness()) {
    _endianness = other._endianness;
  }

  if (other.hasAlignmentBehavior()) {
    _alignmentBehavior = other._alignmentBehavior;
  }

  if (other.hasWordSize()) {
    _wordSize = other._wordSize;
  }

  if (other.hasByteSize()) {
    _byteSize = other._byteSize;
  }

  addInstructions(other.getInstructions());
  addUnits(other.getUnits());

  return *this;
}

bool ExtensionInformation::isValid() const noexcept {
  if (!_instructions.isValid()) return false;
  if (!Utility::allOf(_units, [](auto& unit) { return unit.isValid(); })) {
    return false;
  }

  return true;
}

bool ExtensionInformation::isComplete() const noexcept {
  if (!hasEndianness()) return false;
  if (!hasAlignmentBehavior()) return false;
  if (!hasWordSize()) return false;
  if (!hasByteSize()) return false;
  if (_instructions.isEmpty()) return false;
  if (_units.empty()) return false;

  // Check the basic stuff
  return isValid();
}

void ExtensionInformation::_deserialize(InformationInterface::Format& data) {
  assert(data.count("name"));

  name(data["name"]);
  _parseEndianness(data);
  _parseAlignmentBehavior(data);

  Utility::doIfThere(data, "word-size", [this](auto& wordSize) {
    this->wordSize(static_cast<word_size_t>(wordSize));
  });

  Utility::doIfThere(data, "byte-size", [this](auto& byteSize) {
    this->byteSize(static_cast<byte_size_t>(byteSize));
  });

  Utility::doIfThere(data, "units", [this](auto& units) {
    for (auto& unit : units) {
      this->addUnit(static_cast<UnitInformation>(unit));
    }
  });

  Utility::doIfThere(data, "instructions", [this](auto& instructions) {
    this->addInstructions(static_cast<InstructionSet>(instructions));
  });
}

void ExtensionInformation::_parseEndianness(
    InformationInterface::Format& data) {
  Utility::doIfThere(data, "endianness", [this](auto& endianness) {
    if (endianness == "little") {
      _endianness = ArchitectureProperties::Endianness::LITTLE;
    } else if (endianness == "big") {
      _endianness = ArchitectureProperties::Endianness::BIG;
    } else if (endianness == "mixed") {
      _endianness = ArchitectureProperties::Endianness::MIXED;
    } else if (endianness == "bi") {
      _endianness = ArchitectureProperties::Endianness::BI;
    } else {
      assert(false);
    }
  });
}

void ExtensionInformation::_parseAlignmentBehavior(
    InformationInterface::Format& data) {
  Utility::doIfThere(data, "alignment-behavior", [this](auto& behavior) {
    if (behavior == "strict") {
      _alignmentBehavior =
          ArchitectureProperties::AlignmentBehavior::ALIGN_STRICT;
    } else if (behavior == "relaxed") {
      _alignmentBehavior =
          ArchitectureProperties::AlignmentBehavior::ALIGN_RELAXED;
    } else {
      assert(false);
    }
  });
}
