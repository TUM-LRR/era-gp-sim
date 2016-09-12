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

#ifndef ERAGPSIM_ARCH_RISCV_JUMP_INSTRUCTIONS_HPP
#define ERAGPSIM_ARCH_RISCV_JUMP_INSTRUCTIONS_HPP

#include <memory>

#include "arch/common/instruction-information.hpp"
#include "arch/common/register-node.hpp"
#include "arch/riscv/abstract-jump-instruction-node.hpp"
#include "arch/riscv/instruction-node.hpp"

namespace riscv {

template <typename UnsignedWord, typename SignedWord>
class JumpAndLinkImmediateInstructionNode
    : public AbstractJumpAndLinkInstructionNode<UnsignedWord, SignedWord> {
 public:
  using super = AbstractJumpAndLinkInstructionNode<UnsignedWord, SignedWord>;

  /** Inherit constructor */
  using super::super;

 private:
  using super::_children;
  using super::_compareChildTypes;
  using super::_child;

  UnsignedWord _jump(UnsignedWord programCounter,
                     MemoryAccess& MemoryAccess) const override {
    // Load the immediate
    auto offset = super::template _child<SignedWord>(1);

    // The 20-bit immediate specifies an offset in multiples
    // of two, relative to the program counter.
    return programCounter + (offset * 2);
  }

  ValidationResult _validateNumberOfChildren() const override {
    if (_children.size() != 2) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "JAL takes one destination register "
                            "and one immediate operand. "));
    }

    return ValidationResult::success();
  }

  ValidationResult _validateOperandTypes() const override {
    using Type = AbstractSyntaxTreeNode::Type;

    if (!_compareChildTypes({Type::REGISTER, Type::IMMEDIATE})) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "JAL takes one destination register "
                            "and one immediate operand. "));
    }

    return ValidationResult::success();
  }

  ValidationResult _validateOffset() const override {
    // Load the immediate
    auto offset = super::template _child<SignedWord>(1);

    // Mask off the top 12 bit and check if they are non-zero
    // If so the immediate is not 20 bit, but larger
    if ((offset & ~0xFFFFF) > 0) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "Immediate operand must be 20 bit or less"));
    }

    return ValidationResult::success();
  }

  ValidationResult _validateResultingProgramCounter() const {
    static const auto addressBoundary =
        std::numeric_limits<UnsignedWord>::max();

    // auto programCounter = _getRegister<UnsignedWord>(memoryAccess, "pc");
    // auto offset         = _child<SignedWord>(1, memoryAccess);
    //
    // auto maximumAllowedOffset = addressBoundary - programCounter;
    //
    // // Check if the program counter would underflow or overflow
    // if (-offset > programCounter || offset > maximumAllowedOffset) {
    //   return ValidationResult::fail(
    //       QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
    //                         "Branch offset would invalidate program
    //                         counter"));
    // }

    return ValidationResult::success();
  }
};

template <typename UnsignedWord, typename SignedWord>
class JumpAndLinkRegisterInstructionNode
    : public AbstractJumpAndLinkInstructionNode<UnsignedWord, SignedWord> {
 public:
  using super = AbstractJumpAndLinkInstructionNode<UnsignedWord, SignedWord>;

  /** Inherit constructor */
  using super::super;

 private:
  using super::_child;
  using super::_children;
  using super::_compareChildTypes;

  UnsignedWord _jump(UnsignedWord programCounter,
                     MemoryAccess& memoryAccess) const override {
    auto base   = super::template _child<UnsignedWord>(1, memoryAccess);
    auto offset = super::template _child<SignedWord>(2, memoryAccess);

    auto address = base + offset;

    // The least-significant bit must be
    // unset according to the RISC-V spec
    return address & ~1;
  }

  ValidationResult _validateNumberOfChildren() const override {
    if (_children.size() != 2) {
      return ValidationResult::fail(QT_TRANSLATE_NOOP(
          "Syntax-Tree-Validation",
          "JALR takes one destination register, one base register "
          "and one immediate operand. "));
    }

    return ValidationResult::success();
  }

  ValidationResult _validateOperandTypes() const override {
    using Type = AbstractSyntaxTreeNode::Type;

    // clang-format off
    std::initializer_list<Type> expected = {
      Type::REGISTER,
      Type::REGISTER,
      Type::IMMEDIATE
    };
    // clang-format on

    if (!_compareChildTypes(expected)) {
      return ValidationResult::fail(QT_TRANSLATE_NOOP(
          "Syntax-Tree-Validation",
          "JALR takes one destination register, one base register "
          "and one immediate operand. "));
    }

    return ValidationResult::success();
  }

  ValidationResult _validateOffset() const override {
    // Load the immediate
    auto offset = super::template _child<SignedWord>(2);

    // Mask off the top 20 bit and check if they are non-zero
    // If so the immediate is not 12 bit, but larger
    if ((offset & ~0xFFF) > 0) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "Immediate operand must be 12 bit or less"));
    }

    return ValidationResult::success();
  }

  ValidationResult _validateResultingProgramCounter() const {
    static const auto addressBoundary =
        std::numeric_limits<UnsignedWord>::max();

    // auto base   = super::template _child<UnsignedWord>(1, memoryAccess);
    // auto offset = super::template _child<SignedWord>(2, memoryAccess);
    //
    // auto maximumAllowedOffset = addressBoundary - base;
    //
    // // Check if the program counter would underflow or overflow
    // if (-offset > base || offset > maximumAllowedOffset) {
    //   return ValidationResult::fail(
    //       QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
    //                         "Jump offset would invalidate program counter"));
    // }

    return ValidationResult::success();
  }
};

template <typename UnsignedWord, typename SignedWord>
class JumpInstructionNode
    : public JumpAndLinkImmediateInstructionNode<UnsignedWord, SignedWord> {
 public:
  using super = JumpAndLinkImmediateInstructionNode<UnsignedWord, SignedWord>;
  explicit JumpInstructionNode(const InstructionInformation& information)
  : super(information) {
    auto zero = std::make_unique<RegisterNode>("x0");
    super::insertChild(0, zero);
  }
};
}


#endif /* ERAGPSIM_ARCH_RISCV_JUMP_INSTRUCTIONS_HPP */
