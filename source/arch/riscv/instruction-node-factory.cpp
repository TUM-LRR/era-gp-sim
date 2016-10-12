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
namespace {

/**
 * Sets up the integer instructions.
 *
 * \tparam SizeType The word size of the architecture.
 */
template <typename SizeType>
void _setupIntegerInstructions(FactoryMap& _factories) {
  auto facade = _factories.integerInstructionFacade<SizeType>();

  facade.template add<AddInstructionNode>("add");
  facade.template add<SubInstructionNode>("sub", false);
  facade.template add<AndInstructionNode>("and");
  facade.template add<OrInstructionNode>("or");
  facade.template add<XorInstructionNode>("xor");
  facade.template add<ShiftLeftLogicalInstructionNode>("sll");
  facade.template add<ShiftRightLogicalInstructionNode>("srl");
  facade.template add<ShiftRightArithmeticInstructionNode>("sra");
}

/**
 * Sets up branch instructions.
 *
 * Jump instructions include `BEQ`, `BNE`, `BLT[U]` and `BG[U]`.
 *
 * \tparam UnsignedWord An unsigned word type.
 * \tparam SignedWord A signed word type.
 */
template <typename UnsignedWord, typename SignedWord>
void _setupBranchInstructions(FactoryMap& _factories) {
  using OperandTypes =
      typename AbstractBranchInstructionNode<UnsignedWord,
                                             SignedWord>::OperandTypes;

  auto facade = _factories.typeFacade<UnsignedWord, SignedWord>();
  facade.template add<BranchEqualInstructionNode>("beq");
  facade.template add<BranchNotEqualInstructionNode>("bne");

  // clang-format off
  facade.template add<BranchLessThanInstructionNode>(
    "blt",
    OperandTypes::SIGNED
  );

  facade.template add<BranchLessThanInstructionNode>(
    "bltu",
    OperandTypes::UNSIGNED
  );

  facade.template add<BranchGreaterEqualInstructionNode>(
      "bge",
      OperandTypes::SIGNED
  );

  facade.template add<BranchGreaterEqualInstructionNode>(
      "bgeu",
      OperandTypes::UNSIGNED
  );
  // clang-format on
}

/**
 * Sets up the jump instructions in the map.
 *
 * Jump instructions include `JAL`, `JALR` and `J`.
 *
 * \tparam UnsignedWord An unsigned word type.
 * \tparam SignedWord A signed word type.
 */
template <typename UnsignedWord, typename SignedWord>
void _setupJumpInstructions(FactoryMap& _factories) {
  auto facade = _factories.typeFacade<UnsignedWord, SignedWord>();
  facade.template add<JumpAndLinkImmediateInstructionNode>("jal");
  facade.template add<JumpAndLinkRegisterInstructionNode>("jalr");
  facade.template add<JumpInstructionNode>("j");
}
}

InstructionNodeFactory::InstructionNodeFactory(
    const InstructionSet& instructions, const Architecture& architecture)
: _instructionSet(instructions) {
  auto wordSize = architecture.getWordSize();
  assert(wordSize == 32 || wordSize == 64);

  if (wordSize == 32) {
    _setupIntegerInstructions<riscv::unsigned32_t>(_factories);
    _setupBranchInstructions<riscv::unsigned32_t, riscv::signed32_t>(_factories);
    _setupJumpInstructions<riscv::unsigned32_t, riscv::signed32_t>(_factories);
  } else if (wordSize == 64) {
    _setupIntegerInstructions<riscv::unsigned64_t>(_factories);
    _setupBranchInstructions<riscv::unsigned64_t, riscv::signed64_t>(_factories);
    _setupJumpInstructions<riscv::unsigned64_t, riscv::signed64_t>(_factories);
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
}
