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

#include "arch/riscv/instruction-node-factory.hpp"
#include "arch/riscv/architecture-only-instructions.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "arch/riscv/integer-instructions.hpp"
#include "arch/riscv/load-store-instructions.hpp"
#include "arch/riscv/lui-auipc-instructions.hpp"

#include "common/utility.hpp"

namespace riscv {

// unnamed namespace for this helper function in order not to pollute riscv
// namespace
namespace {

// this function is placed here (and not in instruction-node-factory.h) to have
// all
//"instructionMap filling" in one file
/*!
 * \brief initializeIntegerInstructions fills the given InstructionMap with
 * lambdas creating arithmetic integer instruction nodes (e.g. add/sub/and/or
 * ...)
 * The template parameter indicates on what word size these instructions will
 * operate.
 * \tparam WordSize A integral type that can hold exactly as many bits as the
 * instructions should perform arithmetic operations with
 * \param _instructionMap Map to fill in, use lowercase mnemonics
 */
template <typename WordSize>
void initializeIntegerInstructions(
    InstructionNodeFactory::InstructionMap& _instructionMap) {
  _instructionMap.emplace("add", [](InstructionInformation& info) {
    return std::make_unique<AddInstructionNode<WordSize>>(info, false);
  });
  _instructionMap.emplace("addi", [](InstructionInformation& info) {
    return std::make_unique<AddInstructionNode<WordSize>>(info, true);
  });
  _instructionMap.emplace("sub", [](InstructionInformation& info) {
    return std::make_unique<SubInstructionNode<WordSize>>(info);
  });
  _instructionMap.emplace("and", [](InstructionInformation& info) {
    return std::make_unique<AndInstructionNode<WordSize>>(info, false);
  });
  _instructionMap.emplace("andi", [](InstructionInformation& info) {
    return std::make_unique<AndInstructionNode<WordSize>>(info, true);
  });
  _instructionMap.emplace("or", [](InstructionInformation& info) {
    return std::make_unique<OrInstructionNode<WordSize>>(info, false);
  });
  _instructionMap.emplace("ori", [](InstructionInformation& info) {
    return std::make_unique<OrInstructionNode<WordSize>>(info, true);
  });
  _instructionMap.emplace("xor", [](InstructionInformation& info) {
    return std::make_unique<XorInstructionNode<WordSize>>(info, false);
  });
  _instructionMap.emplace("xori", [](InstructionInformation& info) {
    return std::make_unique<XorInstructionNode<WordSize>>(info, true);
  });
  _instructionMap.emplace("sll", [](InstructionInformation& info) {
    return std::make_unique<ShiftLogicalLeftInstructionNode<WordSize>>(info,
                                                                       false);
  });
  _instructionMap.emplace("slli", [](InstructionInformation& info) {
    return std::make_unique<ShiftLogicalLeftInstructionNode<WordSize>>(info,
                                                                       true);
  });
  _instructionMap.emplace("srl", [](InstructionInformation& info) {
    return std::make_unique<ShiftLogicalRightInstructionNode<WordSize>>(info,
                                                                        false);
  });
  _instructionMap.emplace("srli", [](InstructionInformation& info) {
    return std::make_unique<ShiftLogicalRightInstructionNode<WordSize>>(info,
                                                                        true);
  });
  _instructionMap.emplace("sra", [](InstructionInformation& info) {
    return std::make_unique<ShiftArithmeticRightInstructionNode<WordSize>>(
        info, false);
  });
  _instructionMap.emplace("srai", [](InstructionInformation& info) {
    return std::make_unique<ShiftArithmeticRightInstructionNode<WordSize>>(
        info, true);
  });
  _instructionMap.emplace("lui", [](InstructionInformation& info) {
    return std::make_unique<LuiInstructionNode<WordSize>>(info);
  });
  _instructionMap.emplace("auipc", [](InstructionInformation& info) {
    return std::make_unique<AuipcInstructionNode<WordSize>>(info);
  });
}

template <typename SignedType, typename UnsignedType>
void initializeLoadStoreInstructions(
    Architecture::word_size_t wordSize,
    InstructionNodeFactory::InstructionMap& _instructionMap) {
  if (wordSize == InstructionNodeFactory::RV64) {
    // The following instructions exist only within RV64
    _instructionMap.emplace("ld", [](InstructionInformation& info) {
      return std::make_unique<LoadInstructionNode<SignedType, UnsignedType>>(
          info,
          LoadInstructionNode<SignedType, UnsignedType>::Type::DOUBLE_WORD);
    });
    _instructionMap.emplace("lwu", [](InstructionInformation& info) {
      return std::make_unique<LoadInstructionNode<SignedType, UnsignedType>>(
          info,
          LoadInstructionNode<SignedType, UnsignedType>::Type::WORD_UNSIGNED);
    });
    _instructionMap.emplace("sd", [](InstructionInformation& info) {
      return std::make_unique<StoreInstructionNode<SignedType, UnsignedType>>(
          info,
          StoreInstructionNode<SignedType, UnsignedType>::Type::DOUBLE_WORD);
    });
  }

  // The following load instructions exist in every RISC V architecture
  _instructionMap.emplace("lw", [](InstructionInformation& info) {
    return std::make_unique<LoadInstructionNode<SignedType, UnsignedType>>(
        info, LoadInstructionNode<SignedType, UnsignedType>::Type::WORD);
  });
  _instructionMap.emplace("lh", [](InstructionInformation& info) {
    return std::make_unique<LoadInstructionNode<SignedType, UnsignedType>>(
        info, LoadInstructionNode<SignedType, UnsignedType>::Type::HALF_WORD);
  });
  _instructionMap.emplace("lhu", [](InstructionInformation& info) {
    return std::make_unique<LoadInstructionNode<SignedType, UnsignedType>>(
        info,
        LoadInstructionNode<SignedType,
                            UnsignedType>::Type::HALF_WORD_UNSIGNED);
  });
  _instructionMap.emplace("lb", [](InstructionInformation& info) {
    return std::make_unique<LoadInstructionNode<SignedType, UnsignedType>>(
        info, LoadInstructionNode<SignedType, UnsignedType>::Type::BYTE);
  });
  _instructionMap.emplace("lbu", [](InstructionInformation& info) {
    return std::make_unique<LoadInstructionNode<SignedType, UnsignedType>>(
        info,
        LoadInstructionNode<SignedType, UnsignedType>::Type::BYTE_UNSIGNED);
  });

  _instructionMap.emplace("sw", [](InstructionInformation& info) {
    return std::make_unique<StoreInstructionNode<SignedType, UnsignedType>>(
        info, StoreInstructionNode<SignedType, UnsignedType>::Type::WORD);
  });
  _instructionMap.emplace("sh", [](InstructionInformation& info) {
    return std::make_unique<StoreInstructionNode<SignedType, UnsignedType>>(
        info, StoreInstructionNode<SignedType, UnsignedType>::Type::HALF_WORD);
  });
  _instructionMap.emplace("sb", [](InstructionInformation& info) {
    return std::make_unique<StoreInstructionNode<SignedType, UnsignedType>>(
        info, StoreInstructionNode<SignedType, UnsignedType>::Type::BYTE);
  });
}

void initializeRV64OnlyInstructions(
    InstructionNodeFactory::InstructionMap& _instructionMap) {
  using WordSize      = uint64_t;
  using OperationSize = uint32_t;
  _instructionMap.emplace("addw", [](InstructionInformation& info) {
    return std::make_unique<AddOnlyInstructionNode<WordSize, OperationSize>>(
        info, true);
  });
  _instructionMap.emplace("addiw", [](InstructionInformation& info) {
    return std::make_unique<AddOnlyInstructionNode<WordSize, OperationSize>>(
        info, false);
  });
  _instructionMap.emplace("subw", [](InstructionInformation& info) {
    return std::make_unique<SubOnlyInstructionNode<WordSize, OperationSize>>(
        info);
  });
  _instructionMap.emplace("sllw", [](InstructionInformation& info) {
    return std::make_unique<
        ShiftLogicalLeftOnlyInstructionNode<WordSize, OperationSize>>(info,
                                                                      false);
  });
  _instructionMap.emplace("slliw", [](InstructionInformation& info) {
    return std::make_unique<
        ShiftLogicalLeftOnlyInstructionNode<WordSize, OperationSize>>(info,
                                                                      true);
  });
  _instructionMap.emplace("srlw", [](InstructionInformation& info) {
    return std::make_unique<
        ShiftLogicalRightOnlyInstructionNode<WordSize, OperationSize>>(info,
                                                                       false);
  });
  _instructionMap.emplace("srliw", [](InstructionInformation& info) {
    return std::make_unique<
        ShiftLogicalRightOnlyInstructionNode<WordSize, OperationSize>>(info,
                                                                       true);
  });
  _instructionMap.emplace("sraw", [](InstructionInformation& info) {
    return std::make_unique<
        ShiftArithmeticRightOnlyInstructionNode<WordSize, OperationSize>>(
        info, false);
  });
  _instructionMap.emplace("sraiw", [](InstructionInformation& info) {
    return std::make_unique<
        ShiftArithmeticRightOnlyInstructionNode<WordSize, OperationSize>>(info,
                                                                          true);
  });
}

}// private namespace

void InstructionNodeFactory::initializeInstructionMap(
    const Architecture& architecture) {
  assert(architecture.isValid());

  Architecture::word_size_t wordSize = architecture.getWordSize();
  // create instructions depending on the word size of the architecture
  // (e.g. r32i or r64i)
  if (wordSize == InstructionNodeFactory::RV32) {
    initializeIntegerInstructions<InstructionNodeFactory::RV32_integral_t>(
        _instructionMap);
    initializeLoadStoreInstructions<
        InstructionNodeFactory::RV32_signed_integral_t,
        InstructionNodeFactory::RV32_integral_t>(wordSize, _instructionMap);
  } else if (wordSize == InstructionNodeFactory::RV64) {
    initializeIntegerInstructions<InstructionNodeFactory::RV64_integral_t>(
        _instructionMap);
    initializeLoadStoreInstructions<
        InstructionNodeFactory::RV64_signed_integral_t,
        InstructionNodeFactory::RV64_integral_t>(wordSize, _instructionMap);
    initializeRV64OnlyInstructions(_instructionMap);
  } else {
    // The given architecture does not define a valid word_size to create
    // IntegerInstructions
    assert(false);
  }
}

std::unique_ptr<AbstractSyntaxTreeNode>
InstructionNodeFactory::createInstructionNode(const std::string& token) const {
  using std::begin;
  using std::end;


  // transform token to lowercase
  std::string lower = Utility::toLower(token);

  if (!_instrSet.hasInstruction(lower)) {
    return nullptr;// return nullptr as the lowercase token could not be found
  }

  auto it = _instructionMap.find(lower);// lookup the token
  assert(it != end(_instructionMap));
  auto info = _instrSet.getInstruction(lower);
  return it->second(info);
  // dereference iterator to the key-value pair and call
  // the function providing the correct InstructionInformation for the
  // instruction
}
}
