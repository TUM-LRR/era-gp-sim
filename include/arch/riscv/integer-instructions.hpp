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
#include "common/assert.hpp"
#include "core/memory-access.hpp"

namespace riscv {

/**
 * Represents a RISC-V "add/addi" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class AddInstructionNode : public IntegerInstructionNode<SizeType> {
 public:
  AddInstructionNode(const InstructionInformation& information, bool immediate)
  : super(information, immediate, [](auto& first, auto& second) {
    return first + second;
  }) {
  }
};

/**
 * Represents a RISC-V "sub" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class SubInstructionNode : public IntegerInstructionNode<SizeType> {
 public:
  SubInstructionNode(const InstructionInformation& information)
  : super(information, immediate, [](auto& first, auto& second) {
    return first - second;
  }) {
  }
};

/**
 * Represents a RISC-V "and/andi" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class AndInstructionNode : public IntegerInstructionNode<SizeType> {
 public:
  AndInstructionNode(const InstructionInformation& information)
  : super(information, immediate, [](auto& first, auto& second) {
    return first & second;
  }) {
  }
};

/**
 * Represents a RISC-V "or/ori" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class OrInstructionNode : public IntegerInstructionNode<SizeType> {
 public:
  OrInstructionNode(const InstructionInformation& information, bool immediate)
  : super(information, immediate, [](auto& first, auto& second) {
    return first | second;
  }) {
  }
};

/**
 * Represents a RISC-V "xor/xori" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class XorInstructionNode : public IntegerInstructionNode<SizeType> {
 public:
  XorInstructionNode(const InstructionInformation& information, bool immediate)
  : super(information, immediate, [](auto& first, auto& second) {
    return first ^ second;
  }) {
  }
};

/**
 * Represents a RISC-V "sll/slli" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class ShiftLogicalLeftInstructionNode
    : public IntegerInstructionNode<SizeType> {
 public:
  ShiftLogicalLeftInstructionNode(const InstructionInformation& information,
                                  bool immediate)
  : super(information, immediate, [](auto& first, auto& second) {
    return first << super::_getLower5Bits(second);
  }) {
  }
};

/**
 * Represents a RISC-V "srl/srli" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType,
          typename = std::enable_if_t<std::is_unsigned<SizeType>::value>>
class ShiftLogicalRightInstructionNode
    : public IntegerInstructionNode<SizeType> {
 public:
  ShiftLogicalRightInstructionNode(const InstructionInformation& information,
                                   bool immediate)
  : super(information, immediate, [](auto& first, auto& second) {
    return first >> super::_getLower5Bits(second);
  }) {
  }
};

/**
 * Represents a RISC-V "sra/srai" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class ShiftArithmeticRightInstructionNode
    : public IntegerInstructionNode<SizeType> {
 public:
  ShiftArithmeticRightInstructionNode(const InstructionInformation& information,
                                      bool immediate)
  : super(information, immediate) {
  }

  /** \copydoc IntegerInstructionNode::_compute() */
  SizeType _compute(SizeType first, SizeType second) const noexcept override {
    static const auto width = sizeof(SizeType) * 8;
    static const auto one   = static_cast<SizeType>(1);

    bool sign        = first & (one << (width - 1));
    auto shiftAmount = super::_getLower5Bits(second);
    auto result      = first >> shiftAmount;

    // Create a mask of <shiftAmount> bits and OR them in, if necessary
    if (sign) result |= ((one << shiftAmount) - 1) << (width - shiftAmount);

    return result;
  }
};
}

#endif /* ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTIONS_HPP */
