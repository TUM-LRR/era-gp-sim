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

#ifndef ERAGPSIM_ARCH_RISCV_BRANCH_INSTRUCTIONS_HPP
#define ERAGPSIM_ARCH_RISCV_BRANCH_INSTRUCTIONS_HPP

#include "arch/common/instruction-information.hpp"
#include "arch/riscv/abstract-branch-instruction-node.hpp"

namespace riscv {

/**
 * The `BEQ` equality branch instruction.
 */
template <typename UnsignedWord, typename SignedWord>
struct BranchEqualInstructionNode
    : public AbstractBranchInstructionNode<UnsignedWord, SignedWord> {
  using super = AbstractBranchInstructionNode<UnsignedWord, SignedWord>;


  /**
   * Constructs the branch instruction.
   *
   * \param information The information object for the instruction.
   */
  explicit BranchEqualInstructionNode(const InstructionInformation& information)
  : super(information, [](const auto& first, const auto& second) {
    // Note the necessity of the `template` before the template function
    // This is because `super` is template type and the template function
    // dependent on it
    // For more information: http://bit.ly/2coIPmu
    return riscv::convert<UnsignedWord>(first) ==
           riscv::convert<UnsignedWord>(second);
  }) {
  }
};

/**
 * The `BNE` inequality branch instruction.
 */
template <typename UnsignedWord, typename SignedWord>
struct BranchNotEqualInstructionNode
    : public AbstractBranchInstructionNode<UnsignedWord, SignedWord> {
  using super = AbstractBranchInstructionNode<UnsignedWord, SignedWord>;


  /**
   * Constructs the branch instruction.
   *
   * \param information The information object for the instruction.
   */
  explicit BranchNotEqualInstructionNode(
      const InstructionInformation& information)
  : super(information, [this](const auto& first, const auto& second) {
    return riscv::convert<UnsignedWord>(first) !=
           riscv::convert<UnsignedWord>(second);
  }) {
  }
};

/**
 * The `BLT[U]` "less than" branch instruction.
 *
 * This instruction handles both signed and unsigned comparison.
 */
template <typename UnsignedWord, typename SignedWord>
struct BranchLessThanInstructionNode
    : public AbstractBranchInstructionNode<UnsignedWord, SignedWord> {
  using super = AbstractBranchInstructionNode<UnsignedWord, SignedWord>;

  using typename super::OperandTypes;

  /**
   * Constructs the branch instruction.
   *
   * \param information The information object for the instruction.
   * \param operandTypes An enum member describing the types of operands.
   */
  explicit BranchLessThanInstructionNode(
      const InstructionInformation& information, OperandTypes operandTypes)
  : super(information, [operandTypes](const auto& first, const auto& second) {
    if (operandTypes == OperandTypes::SIGNED) {
      return riscv::convert<SignedWord>(first) <
             riscv::convert<SignedWord>(second);
    } else {
      return riscv::convert<UnsignedWord>(first) <
             riscv::convert<UnsignedWord>(second);
    }
  }) {
  }
};

/**
 * The `BGE[U]` "greater than or equal" branch This.
 *
 * instruction instruction handles both signed and unsigned comparison.
 */
template <typename UnsignedWord, typename SignedWord>
struct BranchGreaterEqualInstructionNode
    : public AbstractBranchInstructionNode<UnsignedWord, SignedWord> {
  using super = AbstractBranchInstructionNode<UnsignedWord, SignedWord>;

  using typename super::OperandTypes;

  /**
   * Constructs the branch instruction.
   *
   * \param information The information object for the instruction.
   * \param operandTypes An enum member describing the types of operands.
   */
  explicit BranchGreaterEqualInstructionNode(
      const InstructionInformation& information, OperandTypes operandTypes)
  : super(information, [operandTypes](const auto& first, const auto& second) {
    if (operandTypes == OperandTypes::SIGNED) {
      return riscv::convert<SignedWord>(first) >=
             riscv::convert<SignedWord>(second);
    } else {
      return riscv::convert<UnsignedWord>(first) >=
             riscv::convert<UnsignedWord>(second);
    }
  }) {
  }
};
}


#endif /* ERAGPSIM_ARCH_RISCV_BRANCH_INSTRUCTIONS_HPP */
