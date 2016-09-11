/* C++ Assembler Interpreter
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.*/

#include <string>

#include "arch/common/architecture.hpp"
#include "arch/riscv/instruction-node-factory.hpp"

#include "common/utility.hpp"

namespace riscv {

InstructionNodeFactory::InstructionNodeFactory(
    const InstructionSet& instructions, const Architecture& architecture)
: _instructionSet(instructions) {
  auto wordSize = architecture.getWordSize();
  assert(wordSize == 32 || wordSize == 64);

  if (wordSize == 32) {
    _setupIntegerInstructions<RV32_integral_t>();
  } else if (wordSize == 64) {
    _setupIntegerInstructions<RV64_integral_t>();
  }

  _setupOtherInstructions();
}

void InstructionNodeFactory::_setupOtherInstructions() {
  _setupLoadInstructions();
  _setupStoreInstructions();
}

void InstructionNodeFactory::_setupLoadInstructions() {
  using Type = LoadInstructionNode::Type;

  _factories.add<LoadInstructionNode>("lw", Type::WORD);
  _factories.add<LoadInstructionNode>("lh", Type::HALF_WORD);
  _factories.add<LoadInstructionNode>("lhu", Type::HALF_WORD_UNSIGNED);
  _factories.add<LoadInstructionNode>("lb", Type::BYTE);
  _factories.add<LoadInstructionNode>("lbu", Type::BYTE_UNSIGNED);
}

void InstructionNodeFactory::_setupStoreInstructions() {
  using Type = StoreInstructionNode::Type;

  _factories.add<StoreInstructionNode>("sw", Type::WORD);
  _factories.add<StoreInstructionNode>("sh", Type::HALF_WORD);
  _factories.add<StoreInstructionNode>("sb", Type::BYTE);
}

InstructionNodeFactory::Node InstructionNodeFactory::createInstructionNode(
    const std::string& mnemonic) const {
  auto lower = Utility::toLower(mnemonic);

  assert(_instructionSet.hasInstruction(lower));
  auto information = _instructionSet.getInstruction(lower);

  return _factories.create(lower, information);
}

const InstructionNodeFactory::Factory& InstructionNodeFactory::FactoryMap::get(
    const std::string& instructionName) const {
  auto iterator = _map.find(instructionName);
  assert(iterator != _map.end());

  return iterator->second;
}

const InstructionNodeFactory::Factory& InstructionNodeFactory::FactoryMap::
operator[](const std::string& instructionName) const {
  return this->get(instructionName);
}

InstructionNodeFactory::Node InstructionNodeFactory::FactoryMap::create(
    const std::string& instructionName,
    const InstructionInformation& information) const {
  return get(instructionName)(information);
}

InstructionNodeFactory::Node InstructionNodeFactory::FactoryMap::
operator()(const std::string& instructionName,
           const InstructionInformation& information) const {
  return create(instructionName, information);
}
}
