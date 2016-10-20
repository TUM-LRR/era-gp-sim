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
#include "arch/riscv/architecture-only-instructions.hpp"
#include "arch/riscv/instruction-node-factory.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "arch/riscv/integer-instructions.hpp"
#include "arch/riscv/load-store-instructions.hpp"
#include "arch/riscv/lui-auipc-instructions.hpp"
#include "arch/riscv/mul-div-instructions.hpp"
#include "arch/riscv/word-instruction-wrapper.hpp"

#include "common/utility.hpp"

namespace riscv {

// unnamed namespace for this helper function in order not to pollute riscv
// namespace
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

template <typename UnsignedWord, typename SignedWord>
void _setupLoadInstructions(FactoryMap& _factories) {
  using Type = typename LoadInstructionNode<UnsignedWord, SignedWord>::Type;
  auto facade = _factories.typeFacade<UnsignedWord, SignedWord>();

  // The following load instructions exist in every RISC V architecture
  facade.template add<LoadInstructionNode>("lw", Type::WORD);
  facade.template add<LoadInstructionNode>("lh", Type::HALF_WORD);
  facade.template add<LoadInstructionNode>("lhu", Type::HALF_WORD_UNSIGNED);
  facade.template add<LoadInstructionNode>("lb", Type::BYTE);
  facade.template add<LoadInstructionNode>("lbu", Type::BYTE_UNSIGNED);
}

template <typename UnsignedWord, typename SignedWord>
void _setupStoreInstructions(FactoryMap& _factories) {
  using Type = typename StoreInstructionNode<UnsignedWord, SignedWord>::Type;
  auto facade = _factories.typeFacade<UnsignedWord, SignedWord>();

  // The following store instructions exist in every RISC V architecture
  facade.template add<StoreInstructionNode>("sw", Type::WORD);
  facade.template add<StoreInstructionNode>("sh", Type::HALF_WORD);
  facade.template add<StoreInstructionNode>("sb", Type::BYTE);
}

template <typename UnsignedWord>
void _setupLuiAuipcInstructions(FactoryMap& _factories) {
  auto facade = _factories.typeFacade<UnsignedWord>();

  facade.template add<LuiInstructionNode>("lui");
  facade.template add<AuipcInstructionNode>("auipc");
}

template <typename UnsignedWord, typename SignedWord>
void _setupExtensionM(FactoryMap& _factories) {
  using Part = typename MultiplicationInstruction<
      UnsignedWord>::MultiplicationResultPart;
  using Type = typename MultiplicationInstruction<UnsignedWord>::Type;
  auto Mfacade = _factories.typeFacade<UnsignedWord>();
  auto Dfacade = _factories.typeFacade<UnsignedWord, SignedWord>();

  Mfacade.template add<MultiplicationInstruction>("mul", Part::LOW,
                                                  Type::SIGNED);
  Mfacade.template add<MultiplicationInstruction>("mulh", Part::HIGH,
                                                  Type::SIGNED);
  Mfacade.template add<MultiplicationInstruction>("mul", Part::LOW,
                                                  Type::SIGNED);
  Mfacade.template add<MultiplicationInstruction>("mulhu", Part::HIGH,
                                                  Type::UNSIGNED);
  Mfacade.template add<MultiplicationInstruction>("mulhsu", Part::HIGH,
                                                  Type::SIGNED_UNSIGNED);

  Dfacade.template add<DivisionInstruction>("div", true, false);
  Dfacade.template add<DivisionInstruction>("divu", false, false);
  Dfacade.template add<RemainderInstruction>("rem", true, false);
  Dfacade.template add<RemainderInstruction>("remu", false, false);

  // this is ugly
  if (sizeof(UnsignedWord)*CHAR_BIT == 64) {
    Mfacade.template add<MultiplicationInstruction>("mulw", Part::LOW, Type::SIGNED, true);
    Dfacade.template add<DivisionInstruction>("divw", true, true);
    Dfacade.template add<DivisionInstruction>("divuw", false, true);
    Dfacade.template add<RemainderInstruction>("remw", true, true);
    Dfacade.template add<RemainderInstruction>("remuw", false, true);
  }
}

template <typename UnsignedWord, typename SignedWord>
void _addExtensionMIfPresent(const InstructionSet& instructionSet,
                             FactoryMap& _factories) {
  if (instructionSet.hasInstruction("mul")) {
    _setupExtensionM<UnsignedWord, SignedWord>(_factories);
  }
}
}

InstructionNodeFactory::InstructionNodeFactory(
    const InstructionSet& instructions, const Architecture& architecture)
    : _instructionSet(instructions) {
  auto wordSize = architecture.getWordSize();
  assert(wordSize == 32 || wordSize == 64);

  if (wordSize == 32) {
    _setupIntegerInstructions<riscv::unsigned32_t>(_factories);
    _setupBranchInstructions<riscv::unsigned32_t, riscv::signed32_t>(
        _factories);
    _setupJumpInstructions<riscv::unsigned32_t, riscv::signed32_t>(_factories);
    _setupLoadInstructions<riscv::unsigned32_t, riscv::signed32_t>(_factories);
    _setupStoreInstructions<riscv::unsigned32_t, riscv::signed32_t>(_factories);
    _setupLuiAuipcInstructions<riscv::unsigned32_t>(_factories);
    _addExtensionMIfPresent<riscv::unsigned32_t, riscv::signed32_t>(
        instructions, _factories);

  } else if (wordSize == 64) {
    _setupIntegerInstructions<riscv::unsigned64_t>(_factories);
    _setupBranchInstructions<riscv::unsigned64_t, riscv::signed64_t>(
        _factories);
    _setupJumpInstructions<riscv::unsigned64_t, riscv::signed64_t>(_factories);
    _setupLoadInstructions<riscv::unsigned64_t, riscv::signed64_t>(_factories);
    _setupStoreInstructions<riscv::unsigned64_t, riscv::signed64_t>(_factories);
    _setupLuiAuipcInstructions<riscv::unsigned64_t>(_factories);
    _addExtensionMIfPresent<riscv::unsigned64_t, riscv::signed64_t>(
        instructions, _factories);
    _setup64BitOnlyInstructions();
  }

  _setupOtherInstructions();
}

void InstructionNodeFactory::_setupOtherInstructions() {}

void InstructionNodeFactory::_setup64BitOnlyInstructions() {
  using WordSize = riscv::unsigned64_t;
  using OperationSize = riscv::unsigned32_t;

  auto facade = _factories.typeFacade<WordSize, OperationSize>();
  facade.template add<AddOnlyInstructionNode>("addw", false);
  facade.template add<AddOnlyInstructionNode>("addiw", true);
  facade.template add<SubOnlyInstructionNode>("subw");
  facade.template add<ShiftLogicalLeftOnlyInstructionNode>("sllw", false);
  facade.template add<ShiftLogicalLeftOnlyInstructionNode>("slliw", true);
  facade.template add<ShiftLogicalRightOnlyInstructionNode>("srlw", false);
  facade.template add<ShiftLogicalRightOnlyInstructionNode>("srliw", true);
  facade.template add<ShiftArithmeticRightOnlyInstructionNode>("sraw", false);
  facade.template add<ShiftArithmeticRightOnlyInstructionNode>("sraiw", true);

  //special load/store instructions
  using LoadType = typename LoadInstructionNode<riscv::unsigned64_t, riscv::signed64_t>::Type;
  using StoreType = typename StoreInstructionNode<riscv::unsigned64_t, riscv::signed64_t>::Type;
  auto loadStoreFacade = _factories.typeFacade<riscv::unsigned64_t, riscv::signed64_t>();
  loadStoreFacade.template add<LoadInstructionNode>("ld", LoadType::DOUBLE_WORD);
  loadStoreFacade.template add<LoadInstructionNode>("lwu", LoadType::WORD_UNSIGNED);
  loadStoreFacade.template add<StoreInstructionNode>("sd", StoreType::DOUBLE_WORD);
}

InstructionNodeFactory::Node InstructionNodeFactory::createInstructionNode(
    const std::string& mnemonic) const {
  auto lower = Utility::toLower(mnemonic);

  if(!_instructionSet.hasInstruction(lower)) {
      return nullptr;
  }
  auto information = _instructionSet.getInstruction(lower);

  return _factories.create(lower, information);
}
}
