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
#include <functional>
#include <limits>

#include "arch/riscv/instruction-node.hpp"

namespace riscv {
template <typename UnsignedWord, typename SignedWord>
class AbstractBranchInstructionNode : public InstructionNode {
 public:
  using super     = InstructionNode;
  using Condition = std::function<bool(const MemoryValue&, const MemoryValue&)>;

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

  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    auto first  = _children[0]->getValue(memoryAccess);
    auto second = _children[1]->getValue(memoryAccess);

    if (_checkCondition(first, second)) {
      auto programCounter = _loadRegister<UnsignedWord>(memoryAccess, "pc");
      auto offset         = _child<SignedWord>(2, memoryAccess);

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

      memoryAccess.setRegisterValue("pc", programCounter);
    }

    return {};
  }

  ValidationResult validate() const override {
    auto result = _validateNumberOfChildren();
    if (!result.isSuccess()) return result;

    result = _validateChildren();
    if (!result.isSuccess()) return result;

    result = _validateOperandTypes();
    if (!result.isSuccess()) return result;

    result = _validateOffset();
    if (!result.isSuccess()) return result;

    result = _validateResultingProgramCounter();
    if (!result.isSuccess()) return result;

    return ValidationResult::success();
  }


 protected:
  enum class Operands { SIGNED, UNSIGNED };

  // Idea: refactor all validation into a validator class, which just takes
  // `this` as a reference and performs all the checks on its children. For this
  // we could either make the validator a friend or give the noder a friendlier
  // interface

  virtual bool
  _checkCondition(const MemoryValue& first, const MemoryValue& second) const {
    assert(static_cast<bool>(_condition));
    return _condition(first, second);
  }

  ValidationResult _validateNumberOfChildren() const {
    if (_children.size() != 3) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "Branch instructions take two "
                            "register and one immediate operand"));
    }

    return ValidationResult::success();
  }

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

  ValidationResult _validateOffset() const {
    auto offset = _child<SignedWord>(2);
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

    // auto programCounter = _loadRegister<UnsignedWord>(memoryAccess, "pc");
    // auto offset         = _child<SignedWord>(2, memoryAccess);
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
