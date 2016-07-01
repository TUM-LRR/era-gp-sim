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

#include "arch/common/instruction-set.hpp"

InstructionSet::InstructionSet(const InformationInterface::Format& data) {
  _deserialize(data);
}

InstructionSet::InstructionSet(InitializerList instructions)
: super(instructions) {
}

InstructionSet& InstructionSet::deserialize(
    const InformationInterface::Format& data) {
  _deserialize(data);
  return *this;
}

InstructionSet& InstructionSet::addInstructions(InitializerList instructions) {
  assert(instructions.size() > 0);
  return addInstructions<InitializerList>(instructions);
}

InstructionSet& InstructionSet::addInstruction(
    const InstructionInformation& instruction) {
  _container.emplace_back(instruction);

  return *this;
}

bool InstructionSet::isValid() const noexcept {
  // clang-format off
  return Utility::allOf(_container, [](auto& instruction) {
    return instruction.isValid();
  });
  // clang-format on
}

void InstructionSet::_deserialize(const InformationInterface::Format& data) {
  for (auto& instruction : data) {
    addInstruction(InstructionInformation{data});
  }
}
