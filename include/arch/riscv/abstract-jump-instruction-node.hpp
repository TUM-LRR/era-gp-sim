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

#ifndef ERAGPSIM_ARCH_RISCV_ABSTRACT_JUMP_INSTRUCTION_NODE_HPP
#define ERAGPSIM_ARCH_RISCV_ABSTRACT_JUMP_INSTRUCTION_NODE_HPP

#include "gtest/gtest_prod.h"

#include "arch/common/instruction-information.hpp"
#include "arch/common/validation-result.hpp"
#include "arch/riscv/instruction-node.hpp"

namespace riscv {

/**
 * The abstract base class for jump instructions.
 *
 * Jump instructions mean any of the `JAL` and `JALR` assembler instructions as
 * well as the `J` pseudo instruction. This parent class handles the validation
 * checks, delegating the actual verifications to polymorphic calls. Moreover,
 * it handles the common part of jump instructions, such as storing the return
 * address in the destination register.
 *
 * On the instructions themselves: jump instructions are *unconditional* jumps,
 * as opposed to the `B-` class of instructions. They are intended for function
 * calls, like `CALL` in x86. All jump instructions take a destination or *link*
 * register, in which the return address of the jump is stored. This address is
 * always 4 bytes after the current program counter, since all instructions in
 * the base RISC-V ISA are 4 bytes long ^1. Then, JAL and JALR differ in the
 * second operand:
 * - `JAL` takes a 20 bit signed immediate as an offset relative to the program
 *   counter. The immediate specifies an offset in multiples of two. An example
 *   would be `JAL x2, -50`, which jumps 100 bytes backwards in the program and
 *   writes the return address into `x2`.
 * - `JALR` takes a base register and a 12 bit signed immediate that is added to
 *   the base register. The result of this is then the absolute target for the
 *   jump. An example command would be `JALR x4, x1, -50`, which sets the
 *   program counter to the value stored in `x1`, minus 50 bytes. Note how the
 *   immediate is __not__ a multiple of two. However, the specification does
 *   require the LSB of the resulting program counter value to be unset, i.e.
 *   the resulting address will never be odd. Using `JALR` with `pc` as the base
 *   register, combined with `AUIPC` lets you jump to 32-bit target relative to
 *   the program counter. `JALR` with any other register, combined with `LUI`,
 *   lets you access any 32-bit *absolute* target (i.e. absolute in the address
 *   space, from 0 to 2^32 - 1).
 * - `J` is a recommended pseudo-operation based on `JAL` that takes a single 20
 *   bit immediate and links the return address into `x0`, i.e. discards it.
 *
 * Note that the standard software calling convention will want to use `x1` as
 * the link register.
 *
 * ^1: While in the base RISC-V extensions we are implementing instructions
 *     definitely are 32 bits wide, RISC-V also supports compressed or extended
 *     formats, specified in multiples of 16 bit parcels. As such, adding 4 to
 *     the program counter may not be correct under certain exotic extension. It
 *     is noteworthy that only the size of the *jump instructions* themselves
 *     matters, since we are linking the address of the next instruction
 *     following the jump. Therefore, if there were to exist an extension in
 *     which the `JAL` instruction does not occupy 32 bits, but 16 or more than
 *     32, we could add this information to the `InstructionInformation` objects
 *     and add the offset dynamically. However, at this point, this is not
 *     neccessary.
 */
template <typename UnsignedWord, typename SignedWord>
class AbstractJumpAndLinkInstructionNode : public InstructionNode {
 public:
  using super = InstructionNode;

  /**
   * Constructs the jump and link instruction.
   *
   * Since this class is abstract, only children may use it.
   *
   * \param information The information object for the instruction.
   */
  explicit AbstractJumpAndLinkInstructionNode(
      const InstructionInformation& information)
  : super(information) {
  }

  /**
   * Make the constructor pure virtual so that the class is abstract.
   * This is necessary since the class, in itself, has no pure virtual methods.
   * Note the implementation outside (below) the class definition.
   */
  virtual ~AbstractJumpAndLinkInstructionNode() = 0;

  /**
   * Performs the branch instruction.
   *
   * The actual condition check is delegated polymorphically.
   *
   * \param memoryAccess The memory access object.
   *
   * \return An empty memory value.
   */
  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    assert(validate(memoryAccess).isSuccess());
    auto destination = _children[0]->getIdentifier();
    auto programCounter = riscv::loadRegister<UnsignedWord>(memoryAccess, "pc");

    // Store the return address (pc + 4) in the destination register
    riscv::storeRegister<UnsignedWord>(
        memoryAccess, destination, programCounter + 4);

    auto result = _jump(programCounter, memoryAccess);

    return riscv::convert<UnsignedWord>(result);
  }

  /**
   * Validates the jump instruction node.
   *
   * The actual checks are delegated polymorphically.
   *
   * \return A `ValidationResult` indicating the validity of the node.
   */
  ValidationResult validate(MemoryAccess& memoryAccess) const override {
    auto result = _validateNumberOfChildren();
    if (!result.isSuccess()) return result;

    result = _validateChildren(memoryAccess);
    if (!result.isSuccess()) return result;

    result = _validateOperandTypes();
    if (!result.isSuccess()) return result;

    result = _validateOffset(memoryAccess);
    if (!result.isSuccess()) return result;

    return ValidationResult::success();
  }

  ValidationResult validateRuntime(MemoryAccess& memoryAccess) const override {
    auto result = _validateResultingProgramCounter(memoryAccess);
    if (!result) return result;

    return ValidationResult::success();
  }

 protected:
  FRIEND_TEST(TestJumpInstructions, TestJALValidation);
  FRIEND_TEST(TestJumpInstructions, TestJALRValidation);

  /**
   * The actual, instruction-specific jump code.
   *
   * This method is called polymorphically in `getValue()`.
   *
   * \param programCounter The old program counter value as an unsigned integer.
   * \param memoryAccess A memory access object.
   *
   * \return The new program counter.
   */
  virtual UnsignedWord
  _jump(UnsignedWord programCounter, MemoryAccess& memoryAccess) const = 0;

  /**
   * Validates the number of children of then node.
   *
   * This method is called polymorphically.
   *
   * \return A `ValidationResult` indicating the result of the check.
   */
  virtual ValidationResult _validateNumberOfChildren() const = 0;

  /**
   * Validates the operand types of the node.
   *
   * This method is called polymorphically.
   *
   * \return A `ValidationResult` indicating the result of the check.
   */
  virtual ValidationResult _validateOperandTypes() const = 0;

  /**
   * Validates the immediate offset.
   *
   * This method is called polymorphically.
   *
   * \return A `ValidationResult` indicating the result of the check.
   */
  virtual ValidationResult _validateOffset(MemoryAccess& memoryAccess) const = 0;

  /**
   * Validates the program counter that would resulting from the instruction.
   *
   * This method is called polymorphically.
   *
   * \return A `ValidationResult` indicating the result of the check.
   */
  virtual ValidationResult
  _validateResultingProgramCounter(MemoryAccess& memoryAccess) const = 0;
};

template <typename UnsignedWord, typename SignedWord>
AbstractJumpAndLinkInstructionNode<UnsignedWord, SignedWord>::
    ~AbstractJumpAndLinkInstructionNode() = default;
}

#endif /* ERAGPSIM_ARCH_RISCV_ABSTRACT_JUMP_INSTRUCTION_NODE_HPP */
