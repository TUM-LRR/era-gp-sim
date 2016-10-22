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

#ifndef ERAGPSIM_ARCH_RISCV_ABSTRACT_BRANCH_INSTRUCTION_NODE_HPP
#define ERAGPSIM_ARCH_RISCV_ABSTRACT_BRANCH_INSTRUCTION_NODE_HPP

#include <QtCore/qglobal.h>
#include <cassert>
#include <functional>
#include <limits>

#include "gtest/gtest_prod.h"

#include "arch/common/validation-result.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "common/utility.hpp"

namespace riscv {

/**
 * The abstract base class for branch instructions.
 *
 * Branch instrutions include `BEQ`, `BNE`, `BLT[U]` and `BGT[U]`. This class
 * facilitates implementation of any branching subclasses by requiring them to
 * instantiate their parent (this class) only with a comparison predicate
 * lambda. This class takes care of common validation checks and the common
 * branching code, such as loading the branch offset and adding it to the
 * program counter.
 *
 * Note that subclasses can either construct this abstract parent with a lambda
 * *or* by overriding the _checkCondition method, for more heavy lifting.
 *
 * On the instructions themselves: branch instructions are suffixed by a
 * condition, such as `EQ`, specifying the comparison that is to be performed
 * between the first and second register operands. The third operand of any
 * branch instruction is a 12-bit immediate specifying a relative offset from
 * the current program counter. It is __important__ to note that the offset is
 * counted in multiples of two. As such specifying `16` denotes an offset of 32
 * bytes and not 16, relative to the program counter. This results in a total
 * range of 13 bit, or +- 18 KiB. With the sign bit, we are back at 12 bit in
 * either the positive or negative direction, i.e. +- 12KiB.
 * As such, a typical instruction would be:
 * `BEQ r6, r9, 16`, which compares `r6` and `r9` for equality
 */
template <typename UnsignedWord, typename SignedWord>
class AbstractBranchInstructionNode : public InstructionNode {
 public:
  using super = InstructionNode;
  using Condition = std::function<bool(const MemoryValue&, const MemoryValue&)>;

  /**
   * An enum indicating whether the
   * instruction performs signed or unsigned
   * comparison.
   */
  enum class OperandTypes { SIGNED, UNSIGNED };

  /**
   * Constructs an abstract branch instruction.
   *
   * As this class is abstract, only subclasses
   * are allowed to call this constructor.
   *
   * \param information The information object for this instruction.
   * \param condition An optional condition predicate lambda.
   *
   * \return
   */
  explicit AbstractBranchInstructionNode(
      const InstructionInformation& information,
      Condition condition = Condition())
  : super(information), _condition(condition) {
  }

  /**
   * Make the constructor pure virtual so that the class is abstract.
   * This is necessary since the class, in itself, has no pure virtual methods.
   * Note the implementation outside (below) the class definition.
   */
  virtual ~AbstractBranchInstructionNode() = 0;

  /**
   * Performs the branch instruction.
   *
   * The acutal condition check is delegated polymorphically.
   *
   * \param memoryAccess The memory access object.
   *
   * \return An empty memory value.
   */
  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    assert(validate(memoryAccess).isSuccess());
    auto first = _children[0]->getValue(memoryAccess);
    auto second = _children[1]->getValue(memoryAccess);

    if (_checkCondition(first, second)) {
      auto programCounter =
          riscv::loadRegister<UnsignedWord>(memoryAccess, "pc");
      auto offset = super::_getChildValue<SignedWord>(memoryAccess, 2);

      // The 12-bit immediate specifies an offset in multiples
      // of two, relative to the program counter.
      // A note on signed / unsigned addition, since the program counter
      // is unsigned while the offset is signed: C++ will cast the *signed*
      // value to an *unsigned* value and then perform the operation. For this,
      // the signed value is added to the largest value representable in the
      // unsigned type, after which the actual operation is performed. The
      // result is then cast to the final unsigned value. This is done because
      // integer overflow is well-defined for unsigned types, but undefined
      // behavior for signed types. So, basically, not as trivial as
      // one might think: http://bit.ly/2c8sfdh
      programCounter += (offset * 2);

      return riscv::convert<UnsignedWord>(programCounter);
    }

    return _incrementProgramCounter<UnsignedWord>(memoryAccess);
  }

  /**
   * Validates the instruction node.
   *
   * A branch instruction is considered valid iff:
   * - It has three children: two registers to compare and 12-bit branch offset.
   * - The first two children are of register type and the third of immediate
   *   type.
   * - The offset does not occupy more than 12 bit.
   * - The resulting program counter would still be a valid address.
   *
   * \return A ValidationResult reflecting the validity according to the above
   *         criteria.
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
  // Idea: refactor all validation into a validator class, which just takes
  // `this` as a reference and performs all the checks on its children. For this
  // we could either make the validator a friend or give the noder a friendlier
  // interface. Would also solve the testability problem.

  FRIEND_TEST(TestBranchInstructions, TestValidation);

  /**
   * Checks a condition predicate between two operands.
   *
   * This method can either be overriden or, if not, will attempt to call the
   * condition lambda passed by the subclass. If neither is possible, an
   * AssertionError is thrown. The operands are passed as memory values because
   * it is not clear at this point if they should be converted to signed or
   * unsigned types.
   *
   * \param first The first operand's memory value.
   * \param second The second operand's memory value.
   *
   * \return True if the condition w.r.t. the operands holds, else false.
   */
  virtual bool
  _checkCondition(const MemoryValue& first, const MemoryValue& second) const {
    assert(static_cast<bool>(_condition));
    return _condition(first, second);
  }

  /**
   * Validates that the branch instruction has exactly three children.
   *
   * Branch instructions must have two register and one immediate operand.
   *
   * \return A ValidationResult reflectingt the result of the check.
   */
  ValidationResult _validateNumberOfChildren() const {
    if (_children.size() != 3) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "Branch instructions take two "
                            "register and one immediate operand"));
    }

    return ValidationResult::success();
  }

  /**
   * Validates the types of the branch instruction's operands.
   *
   * Branch instructions must have two register and one immediate operand.
   *
   * \return A ValidationResult reflecting the result of the check.
   */
  ValidationResult _validateOperandTypes() const {
    using Type = AbstractSyntaxTreeNode::Type;

    // clang-format off
    std::initializer_list<Type> expected = {
      Type::REGISTER,
      Type::REGISTER,
      Type::IMMEDIATE
    };
    // clang-format on

    if (!_compareChildTypes(expected)) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "Branch instructions take two "
                            "register and one immediate operand"));
    }

    return ValidationResult::success();
  }

  /**
   * Validates that the offset immediate has the right number of bits (12).
   *
   * \return A ValidationResult reflecting the result of the check.
   */
  ValidationResult _validateOffset(MemoryAccess& memoryAccess) const {
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
   * Boundary check the program counter resulting from the operation.
   *
   * This check ensures the program counter would neither underflow (become
   * negative) or overflow (exceed the edge of the address space).
   *
   * \return A ValidationResult reflecting the result of the check.
   */
  ValidationResult
  _validateResultingProgramCounter(MemoryAccess& memoryAccess) const {
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

  Condition _condition;
};

// clang-format off
template<typename UnsignedWord, typename SignedWord>
AbstractBranchInstructionNode<
  UnsignedWord, SignedWord
>::~AbstractBranchInstructionNode() = default;
// clang-format on
}

#endif /* ERAGPSIM_ARCH_RISCV_ABSTRACT_BRANCH_INSTRUCTION_NODE_HPP_ */
