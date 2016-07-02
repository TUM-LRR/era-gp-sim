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

#include "arch/common/architecture-brewery.hpp"
#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/extension-information.hpp"

Architecture Architecture::Brew(const ArchitectureFormula& formula) {
  return ArchitectureBrewery(formula).brew();
}

Architecture::Architecture(const std::string& name) : _name(name) {
}

Architecture::Architecture(const std::string& name,
                           const ExtensionInformation& base)
: _base(std::make_unique<ExtensionInformation>(base)), _validated(false) {
  assert(base.isComplete());
  // For constraints
  this->name(name);
}

Architecture::Architecture(const Architecture& other)
: _name(other._name)
, _base(std::make_unique<ExtensionInformation>(*other._base))
, _validated(other._validated) {
}

Architecture::Architecture(Architecture&& other) noexcept : Architecture() {
  swap(other);
}

Architecture& Architecture::operator=(Architecture other) {
  swap(other);
  return *this;
}

Architecture::~Architecture() = default;

void Architecture::swap(Architecture& other) noexcept {
  using std::swap;

  swap(_name, other._name);
  swap(_base, other._base);
  swap(_validated, other._validated);
}

void swap(Architecture& first, Architecture& second) noexcept {
  first.swap(second);
}

Architecture& Architecture::operator+=(const ExtensionInformation& extension) {
  return extendBy(extension);
}

Architecture Architecture::operator+(
    const ExtensionInformation& extension) const {
  auto temp = *this;
  temp += extension;

  return temp;
}

Architecture& Architecture::extendBy(const ExtensionInformation& extension) {
  if (_base == nullptr) {
    _base = std::make_unique<ExtensionInformation>(extension);
  } else {
    _base->merge(extension);
  }

  _validated = false;

  return *this;
}

Architecture& Architecture::name(const std::string& name) {
  assert(!name.empty());
  _name = name;

  return *this;
}

const std::string& Architecture::getName() const noexcept {
  return _name;
}

Architecture::Endianness Architecture::getEndianness() const noexcept {
  assert(isValidated());
  return _base->getEndianness();
}

Architecture::AlignmentBehavior Architecture::getAlignmentBehavior() const
    noexcept {
  assert(isValidated());
  return _base->getAlignmentBehavior();
}

/**
 * Returns the word size of the extension (in bits), if any.
 */
Architecture::word_size_t Architecture::getWordSize() const noexcept {
  assert(isValidated());
  return _base->getWordSize();
}

const Architecture::UnitContainer& Architecture::getUnits() const {
  assert(isValidated());
  return _base->getUnits();
}

const InstructionSet& Architecture::getInstructions() const {
  assert(isValidated());
  return _base->getInstructions();
}

Architecture& Architecture::validate() {
  if (!_validated) {
    assert(isValid());
    _validated = true;
  }

  return *this;
}

bool Architecture::isValidated() const noexcept {
  return _validated;
}

bool Architecture::isValid() const noexcept {
  if (_name.empty()) return false;
  if (_base == nullptr) return false;
  if (!_base->isComplete()) return false;

  return true;
}
