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

#ifndef ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTIONS_HPP
#define ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTIONS_HPP

#include <QtGlobal>
#include <cassert>
#include <string>
#include <type_traits>

#include "arch/common/instruction-information.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "arch/riscv/integer-instruction-node.hpp"
#include "common/utility.hpp"

namespace riscv {

/**
 * Represents a RISC-V "add/addi" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
struct AddInstructionNode : public AbstractIntegerInstructionNode<SizeType> {
  using super = AbstractIntegerInstructionNode<SizeType>;
  using typename super::Operands;
  explicit AddInstructionNode(const InstructionInformation& information,
                              Operands operands)
  : super(information, operands, [](const auto& first, const auto& second) {
    return first + second;
  }) {
  }
};

/**
 * Represents a RISC-V "sub" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate on
 */
template <typename SizeType>
struct SubInstructionNode : public AbstractIntegerInstructionNode<SizeType> {
  using super = AbstractIntegerInstructionNode<SizeType>;
  using typename super::Operands;
  explicit SubInstructionNode(const InstructionInformation& information,
                              Operands)
  : super(
        information,
        super::Operands::REGISTERS,
        [](const auto& first, const auto& second) { return first - second; }) {
  }
};

/**
 * Represents a RISC-V "and/andi" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate on
 */
template <typename SizeType>
struct AndInstructionNode : public AbstractIntegerInstructionNode<SizeType> {
  using super = AbstractIntegerInstructionNode<SizeType>;
  using typename super::Operands;
  explicit AndInstructionNode(const InstructionInformation& information,
                              Operands operands)
  : super(information, operands, [](const auto& first, const auto& second) {
    return first & second;
  }) {
  }
};

/**
 * Represents a RISC-V "or/ori" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate on
 */
template <typename SizeType>
struct OrInstructionNode : public AbstractIntegerInstructionNode<SizeType> {
  using super = AbstractIntegerInstructionNode<SizeType>;
  using typename super::Operands;
  explicit OrInstructionNode(const InstructionInformation& information,
                             Operands operands)
  : super(information, operands, [](const auto& first, const auto& second) {
    return first | second;
  }) {
  }
};

/**
 * Represents a RISC-V "xor/xori" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate on
 */
template <typename SizeType>
struct XorInstructionNode : public AbstractIntegerInstructionNode<SizeType> {
  using super = AbstractIntegerInstructionNode<SizeType>;
  using typename super::Operands;
  explicit XorInstructionNode(const InstructionInformation& information,
                              Operands operands)
  : super(information, operands, [](const auto& first, const auto& second) {
    return first ^ second;
  }) {
  }
};

/**
 * Represents a RISC-V "sll/slli" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate on
 */
template <typename SizeType>
struct ShiftLeftLogicalInstructionNode
    : public AbstractIntegerInstructionNode<SizeType> {
  using super = AbstractIntegerInstructionNode<SizeType>;
  using typename super::Operands;
  explicit ShiftLeftLogicalInstructionNode(
      const InstructionInformation& information, Operands operands)
  : super(information, operands, [this](const auto& first, const auto& second) {
    // For logical right shift, SizeType must be a unsigned integral type. Due
    // to the fact that signed right shift is implementation/compiler specific
    // and can be either a logical shift or a arithmetical shift
    static_assert(std::is_unsigned<SizeType>::value,
                  "SizeType must unsigned for SLL");
    return first << Utility::lowerNBits<5>(second);
  }) {
  }
};

/**
 * Represents a RISC-V "srl/srli" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate on
 */
template <typename SizeType>
struct ShiftRightLogicalInstructionNode
    : public AbstractIntegerInstructionNode<SizeType> {
  using super = AbstractIntegerInstructionNode<SizeType>;
  using typename super::Operands;
  explicit ShiftRightLogicalInstructionNode(
      const InstructionInformation& information, Operands operands)
  : super(information, operands, [this](const auto& first, const auto& second) {
    return first >> Utility::lowerNBits<5>(second);
  }) {
  }
};

/**
 * Represents a RISC-V "sra/srai" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate on
 */
template <typename SizeType>
struct ShiftRightArithmeticInstructionNode
    : public AbstractIntegerInstructionNode<SizeType> {
  using super = AbstractIntegerInstructionNode<SizeType>;
  using typename super::Operands;
  explicit ShiftRightArithmeticInstructionNode(
      const InstructionInformation& information, Operands operands)
  : super(information, operands) {
  }

  /** \copydoc AbstractIntegerInstructionNode::_compute() */
  SizeType _compute(SizeType first, SizeType second) const noexcept override {
    static const auto width = sizeof(SizeType) * 8;
    static const auto one = static_cast<SizeType>(1);

    // C++ standard does not define a arithemtic shift operator.

    bool sign = first & (one << (width - 1));
    auto shiftAmount = Utility::lowerNBits<5>(second);
    auto result = first >> shiftAmount;

    // Create a mask of <shiftAmount> bits and OR them in, if necessary
    if (sign) result |= ((one << shiftAmount) - 1) << (width - shiftAmount);

    return result;
  }
};
}

#endif /* ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTIONS_HPP */
