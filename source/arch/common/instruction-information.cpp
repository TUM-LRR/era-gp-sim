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

InstructionInformation::InstructionInformation(const std::string& mnemonic,
                                               const InstructionKey& key,
                                               const std::string& format)
: _mnemonic(mnemonic), _key(key), _format(format) {
}

bool InstructionInformation::
operator==(const InstructionInformation& other) const noexcept {
  if (this->_mnemonic != other._mnemonic) return false;
  if (this->_key != other._key) return false;
  if (this->_format != other._format) return false;

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

const std::string& InstructionInformation::getMnemonic() const {
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

const InstructionKey& InstructionInformation::getKey() const {
  assert(hasKey());
  return _key;
}

bool InstructionInformation::hasKey() const noexcept {
  return _key.isValid();
}

InstructionInformation&
InstructionInformation::format(const std::string& format) {
  assert(!format.empty());
  _format = format;
  return *this;
}

const std::string& InstructionInformation::getFormat() const noexcept {
  assert(hasFormat());
  return _format;
}

bool InstructionInformation::hasFormat() const noexcept {
  return !_format.empty();
}

bool InstructionInformation::isValid() const noexcept {
  if (!hasMnemonic()) return false;
  if (!hasKey()) return false;
  if (!hasFormat()) return false;

  return true;
}

void InstructionInformation::_deserialize(InformationInterface::Format& data) {
  assert(data.count("mnemonic"));
  assert(data.count("format"));
  assert(data.count("key"));

  mnemonic(Utility::toLower(data["mnemonic"]));
  format(data["format"]);

  key(static_cast<InstructionKey>(data["key"]));
}
