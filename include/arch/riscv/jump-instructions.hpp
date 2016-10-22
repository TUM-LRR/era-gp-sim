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
#include "common/utility.hpp"

namespace riscv {

/**
 * The `JAL` instruction node.
 *
 * The JAL instruction takes a link register and 20 bit immediate, specifying an
 * offset relative to the program counter, in multiples of two bytes.
 */
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
  using super::_getChildValue;

  /**
   * Performs the actual jump operation.
   *
   * `JAL` will add its immediate operand to the old program counter.
   *
   * \param programCounter The current program counter.
   * \param memoryAccess A memory access object.
   *
   * \return The new program counter.
   */
  UnsignedWord _jump(UnsignedWord programCounter,
                     MemoryAccess& memoryAccess) const override {
    // Load the immediate
    auto offset = super::template _getChildValue<SignedWord>(memoryAccess, 1);

    // The 20-bit immediate specifies an offset in multiples
    // of two, relative to the program counter.
    return programCounter + (offset * 2);
  }

  /**
   * Validates that the instruction has two children.
   *
   * \return A `ValidationResult` reflecting the result of the check.
   */
  ValidationResult _validateNumberOfChildren() const override {
    if (_children.size() != 2) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "JAL takes one destination register "
                            "and one immediate operand. "));
    }

    return ValidationResult::success();
  }

  /**
   * Validates the types of the operands.
   *
   * This check passes iff:
   * - The first operand is a register (link register).
   * - The second operand is an immediate (offset).
   *
   * \return A `ValidationResult` reflecting the result of the check.
   */
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

  /**
   * Range-checks the immediate operand.
   *
   * The immediate must not occupy more than 20 bit for `JAL`.
   *
   * \return A `ValidationResult` reflecting the result of the check.
   */
  ValidationResult _validateOffset(MemoryAccess& memoryAccess) const override {
    // Load the immediate
    auto offset = _children[1]->getValue(memoryAccess);

    // The immediate is 20 bit, but including the sign bit. Because it is
    // counted in multiples of two, you still get +- 20 bit, but the value
    // itself may still only occupy 19 bit!
    if (Utility::occupiesMoreBitsThan(offset, 20)) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "Immediate operand must be 20 bit or less"));
    }

    return ValidationResult::success();
  }

  /**
   * Range-checks the program counter that would result from the instruction.
   *
   * This checks if adding the immediate offset would make the program counter
   * negative or exceed the address space.
   *
   * \return A `ValidationResult` reflecting the result of the check.
   */
  ValidationResult
  _validateResultingProgramCounter(MemoryAccess& memoryAccess) const override {
    static const auto addressBoundary =
        std::numeric_limits<UnsignedWord>::max();

    auto programCounter = riscv::loadRegister<UnsignedWord>(memoryAccess, "pc");
    auto offset = super::template _getChildValue<SignedWord>(memoryAccess, 1);

    auto maximumAllowedOffset = addressBoundary - programCounter;

    // Check if the program counter would underflow or overflow
    if (-offset > programCounter || offset > maximumAllowedOffset) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "Branch offset would invalidate program counter"));
    }

    return ValidationResult::success();
  }
};

/**
 * The `JALR` instruction node.
 *
 * The JALR instruction takes a link register, a base register and a 12 bit
 * immediate offset in multiples of __one__ byte.
 */
template <typename UnsignedWord, typename SignedWord>
class JumpAndLinkRegisterInstructionNode
    : public AbstractJumpAndLinkInstructionNode<UnsignedWord, SignedWord> {
 public:
  using super = AbstractJumpAndLinkInstructionNode<UnsignedWord, SignedWord>;

  /** Inherit constructor */
  using super::super;

 private:
  using super::_getChildValue;
  using super::_children;
  using super::_compareChildTypes;

  /**
   * Performs the actual jump operation.
   *
   * `JALR` will *assign* the program counter to the value stored in the base
   * register, plus the 12 bit signed immediate offset. Note that it is *not*
   * interpreted as multiples of two.
   *
   * \param programCounter The current program counter.
   * \param memoryAccess A memory access object.
   *
   * \return The new program counter.
   */
  UnsignedWord _jump(UnsignedWord, MemoryAccess& memoryAccess) const override {
    auto base = super::template _getChildValue<UnsignedWord>(memoryAccess, 1);
    auto offset = super::template _getChildValue<SignedWord>(memoryAccess, 2);

    auto address = base + offset;

    // The least-significant bit must be
    // unset according to the RISC-V spec
    return address & ~1;
  }

  /**
   * Validates that the instruction has two children.
   *
   * \return A `ValidationResult` reflecting the result of the check.
   */
  ValidationResult _validateNumberOfChildren() const override {
    if (_children.size() != 3) {
      return ValidationResult::fail(QT_TRANSLATE_NOOP(
          "Syntax-Tree-Validation",
          "JALR takes one destination register, one base register "
          "and one immediate operand. "));
    }

    return ValidationResult::success();
  }

  /**
   * Validates the types of the operands.
   *
   * This check passes iff:
   * - The first operand is a register (link register).
   * - The second operand is a register (base register).
   * - The third operand is an immediate (the offset).
   *
   * \return A `ValidationResult` reflecting the result of the check.
   */
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

  /**
   * Range-checks the immediate operand.
   *
   * The immediate must not occupy more than 12 bit for `JALR`.
   *
   * \return A `ValidationResult` reflecting the result of the check.
   */
  ValidationResult _validateOffset(MemoryAccess& memoryAccess) const override {
    // Load the immediate
    auto offset = _children[2]->getValue(memoryAccess);

    // The immediate is 12 bit, but including the sign bit. Because it is
    // counted in multiples of two, you still get +- 12 bit, but the value
    // itself may still only occupy 11 bit!
    if (Utility::occupiesMoreBitsThan(offset, 12)) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "Immediate operand must be 12 bit or less"));
    }

    return ValidationResult::success();
  }

  /**
   * Range-checks the program counter that would result from the instruction.
   *
   * This checks if adding the immediate offset would make the program counter
   * negative or exceed the address space.
   *
   * \return A `ValidationResult` reflecting the result of the check.
   */
  ValidationResult
  _validateResultingProgramCounter(MemoryAccess& memoryAccess) const override {
    static const auto addressBoundary =
        std::numeric_limits<UnsignedWord>::max();

    auto base = super::template _getChildValue<UnsignedWord>(memoryAccess, 1);
    auto offset = super::template _getChildValue<SignedWord>(memoryAccess, 2);

    auto maximumAllowedOffset = addressBoundary - base;

    // Check if the program counter would underflow or overflow
    if (-offset > base || offset > maximumAllowedOffset) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "Jump offset would invalidate program counter"));
    }

    return ValidationResult::success();
  }
};

/**
 * The `J` instruction node.
 *
 * The `J` instruction is a pseudo-operation representing `JAL` with `rd` set to
 * zero, i.e. discarding the return address. It is like a `JMP` in x86. As such,
 * its only operand is a 20 bit signed immediate offset in multiples of two
 * bytes, relative to the program counter.
 */
template <typename UnsignedWord, typename SignedWord>
class JumpInstructionNode
    : public JumpAndLinkImmediateInstructionNode<UnsignedWord, SignedWord> {
 public:
  using super = JumpAndLinkImmediateInstructionNode<UnsignedWord, SignedWord>;

  /**
   * Constructs the `J` node.
   *
   * \param information The information object for the instruction.
   */
  explicit JumpInstructionNode(const InstructionInformation& information)
  : super(information) {
    // Create a register node and insert it a the first operand to `JAL`.
    auto zero = std::make_unique<RegisterNode>("x0");
    super::insertChild(0, std::move(zero));
  }
};
}


#endif /* ERAGPSIM_ARCH_RISCV_JUMP_INSTRUCTIONS_HPP */
