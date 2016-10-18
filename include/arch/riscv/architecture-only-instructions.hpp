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

#ifndef ERAGPSIM_ARCH_RISCV_ARCHITECTURE_ONLY_INSTRUCTIONS_HPP_
#define ERAGPSIM_ARCH_RISCV_ARCHITECTURE_ONLY_INSTRUCTIONS_HPP_

/**
 * This file defines the following operations:
 * ADDIW, SLLIW, SRLIW, SRAIW, ADDW, SUBW, SLLW, SRLW, SRAW
 */

#include "arch/riscv/architecture-only-instruction-node.hpp"

namespace riscv {

template <typename WordSize, typename OperationSize>
class AddOnlyInstructionNode
    : public ArchitectureOnlyInstructionNode<WordSize, OperationSize> {
 public:
  using super = ArchitectureOnlyInstructionNode<WordSize, OperationSize>;
  AddOnlyInstructionNode(const InstructionInformation& information, bool immediate)
  : super(information,
          immediate,
          [this](OperationSize first, OperationSize second) -> WordSize {
            return super::_signExpand(first + second);
          }) {
  }
};

template <typename WordSize, typename OperationSize>
class SubOnlyInstructionNode
    : public ArchitectureOnlyInstructionNode<WordSize, OperationSize> {
 public:
  using super = ArchitectureOnlyInstructionNode<WordSize, OperationSize>;
  SubOnlyInstructionNode(const InstructionInformation& information)
  : super(information,
          false,// There is no SUBIW
          [this](OperationSize first, OperationSize second) -> WordSize {
            return super::_signExpand(first - second);
          }) {
  }
};

template <typename WordSize, typename OperationSize>
class ShiftLogicalLeftOnlyInstructionNode
    : public ArchitectureOnlyInstructionNode<WordSize, OperationSize> {
 public:
  using super = ArchitectureOnlyInstructionNode<WordSize, OperationSize>;
  ShiftLogicalLeftOnlyInstructionNode(const InstructionInformation& information,
                                      bool immediate)
  : super(information,
          immediate,
          [this](OperationSize first, OperationSize second) -> WordSize {
            return super::_signExpand(first << super::_getLower5Bits(second));
          }) {
  }
};

template <typename WordSize, typename OperationSize>
class ShiftLogicalRightOnlyInstructionNode
    : public ArchitectureOnlyInstructionNode<WordSize, OperationSize> {
 public:
  using super = ArchitectureOnlyInstructionNode<WordSize, OperationSize>;
  ShiftLogicalRightOnlyInstructionNode(const InstructionInformation& information,
                                       bool immediate)
  : super(information,
          immediate,
          [this](OperationSize first, OperationSize second) -> WordSize {
            return super::_signExpand(first >> super::_getLower5Bits(second));
          }) {
  }
};

template <typename WordSize, typename OperationSize>
class ShiftArithmeticRightOnlyInstructionNode
    : public ArchitectureOnlyInstructionNode<WordSize, OperationSize> {
 public:
  using super = ArchitectureOnlyInstructionNode<WordSize, OperationSize>;
  ShiftArithmeticRightOnlyInstructionNode(const InstructionInformation& information,
                                          bool immediate)
  : super(information,
          immediate,
          [this](OperationSize first, OperationSize second) -> WordSize {
            static const auto width = sizeof(OperationSize) * 8;
            static const auto one   = static_cast<OperationSize>(1);

            bool sign        = first & (one << (width - 1));
            auto shiftAmount = super::_getLower5Bits(second);
            auto result      = first >> shiftAmount;

            // Create a mask of <shiftAmount> bits and OR them in, if necessary
            if (sign) {
              result |= ((one << shiftAmount) - 1) << (width - shiftAmount);
            }

            return super::_signExpand(result);
          }) {
  }
};

}// Namespace riscv

#endif// ERAGPSIM_ARCH_RISCV_ARCHITECTURE_ONLY_INSTRUCTIONS_HPP_
