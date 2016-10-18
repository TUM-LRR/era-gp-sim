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

#include <string>

#include "arch/common/instruction-information.hpp"
#include "common/assert.hpp"

InstructionInformation::InstructionInformation(
    InformationInterface::Format& data) {
  deserialize(data);
}

InstructionInformation::InstructionInformation(const std::string& mnemonic)
: _mnemonic(mnemonic) {
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
  assert::that(!mnemonic.empty());
  _mnemonic = mnemonic;

  return *this;
}

const std::string& InstructionInformation::getMnemonic() const {
  assert::that(hasMnemonic());
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
  assert::that(hasKey());
  return _key;
}

bool InstructionInformation::hasKey() const noexcept {
  return _key.isValid();
}

InstructionInformation&
InstructionInformation::format(const std::string& format) {
  assert::that(!format.empty());
  _format = format;
  return *this;
}

const std::string& InstructionInformation::getFormat() const {
  assert::that(hasFormat());
  return _format;
}

bool InstructionInformation::hasFormat() const noexcept {
  return !_format.empty();
}

InstructionInformation& InstructionInformation::length(length_t length) {
  assert::that(length > 0);
  _length = length;
  return *this;
}

InstructionInformation::length_t InstructionInformation::getLength() const {
  assert::that(hasLength());
  return *_length;
}

bool InstructionInformation::hasLength() const noexcept {
  return static_cast<bool>(_length);
}

bool InstructionInformation::isValid() const noexcept {
  if (!hasMnemonic()) return false;
  if (!hasKey()) return false;
  if (!hasFormat()) return false;
  if (!hasLength()) return false;

  return true;
}

void InstructionInformation::_deserialize(InformationInterface::Format& data) {
  assert::that(data.count("mnemonic"));
  assert::that(data.count("format"));
  assert::that(data.count("key"));
  assert::that(data.count("length"));

  mnemonic(Utility::toLower(data["mnemonic"]));
  format(data["format"]);
  length(data["length"]);

  key(static_cast<InstructionKey>(data["key"]));
}
