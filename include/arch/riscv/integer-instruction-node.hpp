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

#include <QtCore/qglobal.h>
#include <functional>
#include <string>
#include <type_traits>

#include "arch/common/validation-result.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "arch/riscv/properties.hpp"

namespace riscv {

/**
 * \brief The AbstractIntegerInstructionNode is a superclass for all integer
 * arithmetic
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
template <typename SizeType>
class AbstractIntegerInstructionNode : public InstructionNode {
 public:
  enum class Operands { IMMEDIATES, REGISTERS };
  using Operation = std::function<SizeType(SizeType, SizeType)>;

  /**
 * Creates an integer instruction with the given information.
 *
 * \param information InstructionInformation that holds the mnemonic of this
 * instruction
 * \param immediate when the instruction is labeled as immediate
 * instruction, 2 register and one immediate operand are expected for
 * validation, if not 3 register operands are expected
 */
  AbstractIntegerInstructionNode(const InstructionInformation& information,
                                 Operands operands,
                                 Operation operation = Operation())
  : InstructionNode(information), _operands(operands), _operation(operation) {
  }

  /**
   * Make the constructor pure virtual so that the class is abstract.
   * This is necessary since the class, in itself, has no pure virtual methods.
   * Note the implementation outside (below) the class definition.
   */
  virtual ~AbstractIntegerInstructionNode() = 0;

  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    assert(validate(memoryAccess).isSuccess());
    // Get the destination register
    auto destination = _children.at(0)->getIdentifier();

    auto first = _getChildValue(1, memoryAccess);
    auto second = _getChildValue(2, memoryAccess);

    auto result = _compute(first, second);
    auto value = riscv::convert(result);

    memoryAccess.putRegisterValue(destination, value);

    return _incrementProgramCounter<SizeType>(memoryAccess);
  }

  ValidationResult validate(MemoryAccess& memoryAccess) const override {
    auto result = _validateNumberOfChildren();
    if (!result.isSuccess()) return result;

    // check if all operands are valid themselves
    result = _validateChildren(memoryAccess);
    if (!result.isSuccess()) return result;

    if (_operands == Operands::IMMEDIATES) {
      result = _validateOperandsForImmediateInstructions();
      if (!result.isSuccess()) return result;

      result = _validateImmediateSize(memoryAccess);
      if (!result.isSuccess()) return result;
    } else {
      result = _validateOperandsForNonImmediateInstructions();
      if (!result.isSuccess()) return result;
    }

    return ValidationResult::success();
  }

 protected:
  /**
  * Performs the actual computation of the instruction.
  *
  * This method can only be called on AbstractIntegerInstructionNode itself if
  * the
  * appropriate constructor was called.
  *
  * \first The first operand.
  * \second The second operand.
  *
  * \return The result of the computation.
  */
  virtual SizeType _compute(SizeType first, SizeType second) const noexcept {
    assert(static_cast<bool>(_operation));
    return _operation(first, second);
  }

 private:
  SizeType _getChildValue(size_t index, MemoryAccess& memoryAccess) const {
    auto memory = _children[index]->getValue(memoryAccess);
    return riscv::convert<SizeType>(memory);
  }

  ValidationResult _validateNumberOfChildren() const {
    // a integer instruction needs exactly 3 operands
    if (_children.size() != 3) {
      return ValidationResult::fail(QT_TRANSLATE_NOOP(
          "Syntax-Tree-Validation",
          "Integer instructions must have exactly 3 operands"));
    }

    return ValidationResult::success();
  }

  ValidationResult _validateImmediateSize(MemoryAccess& memoryAccess) const {
    assert(_children[2]->getType() == Type::IMMEDIATE);

    // No memory access is needed for a immediate node
    MemoryValue value = _children.at(2)->getValue(memoryAccess);
    if (Utility::occupiesMoreBitsThan(value, 12)) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "The immediate value of this instruction must "
                            "be representable by %1 bits"),
          std::to_string(12));

      //    if (value.getSize() > 12) {
      //      // Look for the sign bit to determine what bits to expect in the
      //      "upper"
      //      // region (i.e. 11...size).
      //      // Index 0 <-> MSB in Memory Value
      //      bool isSignBitSet = value.get(value.getSize() - 1);
      //      for (std::size_t index = 11; index < value.getSize(); ++index) {
      //        if ((isSignBitSet && !value.get(index)) ||
      //            (!isSignBitSet && value.get(index))) {
      //          return ValidationResult::fail(
      //              QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
      //                                "The immediate value of this instruction
      //                                must "
      //                                "be representable by %1 bits"),
      //              std::to_string(12));
      //        }
      //      }
    }

    return ValidationResult::success();
  }

  ValidationResult _validateOperandsForImmediateInstructions() const {
    if (!_requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 2) ||
        !_requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 2, 1)) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "The immediate-integer instructions must have 2 "
                            "registers and 1 immediate as operands"));
    }

    return ValidationResult::success();
  }

  ValidationResult _validateOperandsForNonImmediateInstructions() const {
    // a register integer instruction needs three register operands
    if (!_requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 3)) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "The register-integer instructions must have 3 "
                            "registers as operands"));
    }

    return ValidationResult::success();
  }

  /** The type of operands this command takes (registers or immediates). */
  Operands _operands;

  /** An optional function with which the node can be constructed. */
  Operation _operation;
};

// clang-format off
template <typename SizeType>
AbstractIntegerInstructionNode<SizeType>::~AbstractIntegerInstructionNode<SizeType>() = default;
// clang-format on
}

#endif /* ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTION_NODE_HPP */
