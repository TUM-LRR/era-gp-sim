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

#ifndef ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTION_NODE_HPP
#define ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTION_NODE_HPP

#include <QtGlobal>
#include <functional>
#include <string>
#include <type_traits>

#include "arch/riscv/instruction-node.hpp"
#include "common/assert.hpp"
#include "core/memory-access.hpp"

namespace riscv {

/**
 * \brief The IntegerInstructionNode is a superclass for all integer arithmetic
 * instructions.
 *
 * As the behaviour of all of these instructions is very similar, this class
 * summarizes the common behaviour to avoid code redundancy.
 * The instruction internally converts the MemoryValues of their operands to a
 * built-in integer type, performs the operation with this built-in type and
 * converts the result to a MemoryValue which is stored in the destination
 * register.
 *
 * \tparam SizeType An integral type to perform the actual operations on.
 */
template <typename SizeType,
          typename = std::enable_if_t<std::is_integral<SizeType>::value>>
class IntegerInstructionNode : public InstructionNode {
 public:
  using Operation = std::function<SizeType(SizeType, SizeType)>;
  /**
 * Creates an integer instructino with the given information.
 *
 * \param information InstructionInformation that holds the mnemonic of this
 * instruction
 * \param immediate when the instruction is labeled as immediate
 * instruction, 2 register and one immediate operand are expected for
 * validation, if not 3 register operands are expected
 */
  IntegerInstructionNode(InstructionInformation& information,
                         bool immediate,
                         Operation operation = Operation())
  : InstructionNode(information)
  , _isImmediate(immediate)
  , _operation(operation) {
  }

  /** Default destructor*/
  virtual ~IntegerInstructionNode() = default;

  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    // Get the destination register
    auto destination = _children[0]->getIdentifier();

    auto first  = _child(1, memoryAccess);
    auto second = _child(2, memoryAccess);

    auto result = _compute(first, second);
    auto value  = convert(result, RISCV_BITS_PER_BYTE, RISCV_ENDIANNESS);

    memoryAccess.setRegisterValue(destination, value);

    return {};
  }

  const ValidationResult validate() const override {
    auto result = _validateNumberOfChildren();
    if (!result.isSuccess()) return result;

    // check if all operands are valid themselves
    auto childrenResult = validateChildren();
    if (!childrenResult.isSuccess()) {
      return childrenResult;
    }

    if (_isImmediate) {
      result = _validateOperandsForImmediateInstructions();
      if (!result.isSuccess()) return result;

      result = _validateImmediateSize();
      if (!result.isSuccess()) return result;
    } else {
      result = _validateOperandsForNonImmediateInstructions();
      if (!result.isSuccess()) return result;
    }

    return ValidationResult::success();
  }

 protected:
  // Since all children will definitely be final classes, we can do this here.
  using super = IntegerInstructionNode<SizeType>;

  /**
   * Performs the actual computation of the instruction.
   *
   * This method can only be called on IntegerInstructionNode itself if the
   * appropriate constructor was called.
   *
   * \first The first operand.
   * \second The second operand.
   *
   * \return The result of the computation.
   */
  virtual SizeType _compute(SizeType first, SizeType second) const noexcept {
    assert::that(static_cast<bool>(_operation));
    return operation(first, second);
  }

  SizeType _getLower5Bits(SizeType op) const {
    return op & 0b11111;
  }

 private:
  SizeType _child(size_t index, MemoryAccess& memoryAccess) {
    auto memory = _children[index]->getValue(memoryAccess);
    return convert<SizeType>(memory, RISCV_ENDIANNESS);
  }

  ValidationResult _validateNumberOfChildren() {
    // a integer instruction needs exactly 3 operands
    if (_children.size() != 3) {
      return ValidationResult::fail(QT_TRANSLATE_NOOP(
          "Syntax-Tree-Validation",
          "Integer instructions must have exactly 3 operands"));
    }

    return ValidationResult::success();
  }

  ValidationResult _validateImmediateSize() {
    if (_isImmediate && _children[2]->getType() == Type::IMMEDIATE) {
      // no memory access is needed for a immediate node
      MemoryValue value = _children.at(2)->getValue(stub);

      if ((value & ~0xFFF) > 0) {
        return ValidationResult::fail(
            QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                              "The immediate value of this instruction must "
                              "be representable by 12 bits"));
      }
    }

    return ValidationResult::success();
  }

  ValidationResult _validateOperandsForImmediateInstructions() {
    if (!_requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 2) ||
        !_requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 2, 1)) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "The immediate-integer instructions must have 2 "
                            "registers and 1 immediate as operands"));
    }

    return ValidationResult::success();
  }

  ValidationResult _validateOperandsForNonImmediateInstructions() {
    // a register integer instruction needs three register operands
    if (!_requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 3)) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "The register-integer instructions must have 3 "
                            "registers as operands"));
    }

    return ValidationResult::success();
  }

  /**
   * Indicates if this instruction is a register-immediate instruction.
   * If false this instruction is a register-register instruction.
   * This value is used for validation of operands
   */
  bool _isImmediate;

  /** An optional function with which the node can be constructed. */
  Operation _operation;
};
}

#endif /* ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTION_NODE_HPP */
