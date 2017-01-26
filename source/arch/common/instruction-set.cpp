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


#include "common/assert.hpp"
#include <string>

#include "arch/common/instruction-set.hpp"

InstructionSet::InstructionSet(InformationInterface::Format& data) {
  _deserialize(data);
}

InstructionSet::InstructionSet(InitializerList instructions) {
  addInstructions(instructions);
}

InstructionSet& InstructionSet::operator+=(const InstructionSet& other) {
  return addInstructions(other);
}


bool InstructionSet::operator==(const InstructionSet& other) const noexcept {
  return this->_container == other._container;
}

bool InstructionSet::operator!=(const InstructionSet& other) const noexcept {
  return !(*this == other);
}

const InstructionInformation& InstructionSet::
operator[](const std::string& mnemonic) const {
  return getInstruction(mnemonic);
}

const InstructionInformation&
InstructionSet::getInstruction(const std::string& mnemonic) const {
  auto lower = Utility::toLower(mnemonic);
  assert::that(_container.count(lower));
  return _container.find(lower)->second;
}

const InstructionKey&
InstructionSet::getInstructionKey(const std::string& mnemonic) const {
  return getInstruction(mnemonic).getKey();
}


bool InstructionSet::hasInstruction(const std::string& mnemonic) const {
    auto lower = Utility::toLower(mnemonic);
  return _container.count(lower);
}

InstructionSet&
InstructionSet::deserialize(InformationInterface::Format& data) {
  _deserialize(data);
  return *this;
}

InstructionSet& InstructionSet::addInstructions(const InstructionSet& other) {
  for (auto& pair : other) {
    addInstruction(pair.second);
  }

  return *this;
}

InstructionSet&
InstructionSet::addInstruction(const InstructionInformation& instruction) {
  _container.emplace(Utility::toLower(instruction.getMnemonic()), instruction);

  return *this;
}

bool InstructionSet::isValid() const noexcept {
  // clang-format off
  return Utility::allOf(_container, [](auto& pair) {
    return pair.second.isValid();
  });
  // clang-format on
}

void InstructionSet::_deserialize(InformationInterface::Format& data) {
  assert::that(!data.empty());
  for (auto& instruction : data) {
    addInstruction(static_cast<InstructionInformation>(instruction));
  }
}
