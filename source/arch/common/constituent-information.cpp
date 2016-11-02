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

#include "arch/common/constituent-information.hpp"
#include "common/assert.hpp"

ConstituentInformation::ConstituentInformation() noexcept = default;

ConstituentInformation::ConstituentInformation(super::Format& data) {
  _deserialize(data);
}

ConstituentInformation::ConstituentInformation(
    id_t id, bit_offset_t enclosingOffset) noexcept
: _id(id), _enclosingOffset(enclosingOffset) {
}

bool ConstituentInformation::
operator==(const ConstituentInformation& other) const noexcept {
  if (this->_id != other._id) return false;
  if (this->_enclosingOffset != other._enclosingOffset) return false;

  return true;
}

bool ConstituentInformation::
operator!=(const ConstituentInformation& other) const noexcept {
  return !(*this == other);
}

ConstituentInformation&
ConstituentInformation::deserialize(InformationInterface::Format& data) {
  _deserialize(data);
  return *this;
}

ConstituentInformation& ConstituentInformation::id(id_t id) noexcept {
  _id = id;
  return *this;
}

bool ConstituentInformation::hasID() const noexcept {
  return static_cast<bool>(_id);
}

ConstituentInformation::id_t ConstituentInformation::getID() const {
  assert::that(hasID());
  return *_id;
}

ConstituentInformation&
ConstituentInformation::enclosingOffset(bit_offset_t index) noexcept {
  _enclosingOffset = index;
  return *this;
}

bool ConstituentInformation::hasEnclosingOffset() const noexcept {
  return static_cast<bool>(_enclosingOffset);
}

ConstituentInformation::bit_offset_t
ConstituentInformation::getEnclosingOffset() const {
  assert::that(hasEnclosingOffset());
  return *_enclosingOffset;
}

bool ConstituentInformation::isValid() const noexcept {
  return hasID() && hasEnclosingOffset();
}

void ConstituentInformation::_deserialize(Format& data) {
  assert::that(data.count("id"));
  assert::that(data.count("enclosing-index"));

  _id = static_cast<id_t>(data["id"]);
  _enclosingOffset = static_cast<bit_offset_t>(data["enclosing-index"]);
}
