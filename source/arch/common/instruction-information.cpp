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

#include <cassert>
#include <string>

#include "arch/common/instruction-information.hpp"

InstructionInformation::InstructionInformation(
    InformationInterface::Format& data) {
  deserialize(data);
}

InstructionInformation::InstructionInformation(const std::string& mnemonic)
: _mnemonic(mnemonic) {
}

InstructionInformation::InstructionInformation(const std::string& mnemonic,
                                               const InstructionKey& key)
: _mnemonic(mnemonic), _key(key) {
}

bool InstructionInformation::
operator==(const InstructionInformation& other) const noexcept {
  if (this->_mnemonic != other._mnemonic) return false;
  if (this->_key != other._key) return false;

  return true;
}


bool InstructionInformation::
operator!=(const InstructionInformation& other) const noexcept {
  return !(*this == other);
}

InstructionInformation&
InstructionInformation::deserialize(InformationInterface::Format& data) {
  _deserialize(data);
  return *this;
}

InstructionInformation&
InstructionInformation::mnemonic(const std::string& mnemonic) {
  assert(!mnemonic.empty());
  _mnemonic = mnemonic;

  return *this;
}

const std::string& InstructionInformation::getMnemonic() const noexcept {
  assert(hasMnemonic());
  return _mnemonic;
}

bool InstructionInformation::hasMnemonic() const noexcept {
  return !_mnemonic.empty();
}

InstructionInformation& InstructionInformation::key(const InstructionKey& key) {
  _key = key;
  return *this;
}

const InstructionKey& InstructionInformation::getKey() const noexcept {
  assert(hasKey());
  return _key;
}

bool InstructionInformation::hasKey() const noexcept {
  return _key.isValid();
}

bool InstructionInformation::isValid() const noexcept {
  return !_mnemonic.empty() && _key.isValid();
}

void InstructionInformation::_deserialize(InformationInterface::Format& data) {
  assert(data.count("mnemonic"));

  auto iterator = data.find("mnemonic");
  this->mnemonic(Utility::toLower(*iterator));
  data.erase(iterator);

  key(static_cast<InstructionKey>(data));
}
