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

#ifndef ERAGPSIM_ARCH_RISCV_ARCHITECTURE_ONLY_INSTRUCTION_NODE_HPP_
#define ERAGPSIM_ARCH_RISCV_ARCHITECTURE_ONLY_INSTRUCTION_NODE_HPP_

#include <QtGlobal>
#include <climits>
#include <functional>
#include <string>
#include <type_traits>

#include "arch/common/validation-result.hpp"
#include "arch/riscv/instruction-node.hpp"

namespace riscv {

/**
 * \brief The ArchitectureOnlyInstructionNode class is a super class for
 *        instructions, that only exist within a concrete architecture, that
 *        builds opon another one (e.g. RV64 builds opon RV32).
 *
 * This class is an abstraction of the special instructions, that only exist
 * within RV64 (which are ADDIW, SLLIW, SRLIW, SRAIW, ADDW, SUBW, SLLW, SRLW
 * SRAW). Those instructions operate on 32 bit, although the word size of the
 * architecture is 64 bit. The result of the operation is either zero- or
 * sign-expanded.
 * The abstraction of this class allows those architecture-specific
 * instructions to be implemented for any kind of architecture type, but is
 * currently only used for the RV64 instructions, as RV128 bit is in a very
 * early stage.
 *
 * \tparam WordSize The unsigned word size integral of the architecture
 *         (e.g. uint64_t for RV64)
 * \tparam OperationSize The unsigned integral, the operations operate on
 *         (e.g. uint32_t for RV64)
 */
template <typename WordSize,
          typename OperationSize,
          typename = std::enable_if_t<std::is_integral<WordSize>::value &&
                                      std::is_unsigned<WordSize>::value &&
                                      std::is_integral<OperationSize>::value &&
                                      std::is_unsigned<OperationSize>::value>>
class ArchitectureOnlyInstructionNode : public InstructionNode {
 public:
  using Operation = std::function<WordSize(OperationSize, OperationSize)>;
  /**
   * \param information InstructionInformation that holds the mnemonic of this
   * instruction
   * \param immediate when the instruction is labeled as immediate
   * instruction, 2 register and one immediate operand are expected for
   * validation, if not 3 register operands are expected
   * \param operation The operation to be performed, if the node is executed.
   */
  ArchitectureOnlyInstructionNode(const InstructionInformation& information,
                                  bool immediate,
                                  Operation operation = Operation())
  : InstructionNode(information)
  , _isImmediate(immediate)
  , _operation(operation) {
  }

  /** Default destructor*/
  virtual ~ArchitectureOnlyInstructionNode() = default;

  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    assert(validate(memoryAccess).isSuccess());
    auto destination = _children[0]->getIdentifier();

    auto first = _getChildValue(1, memoryAccess);
    auto second = _getChildValue(2, memoryAccess);

    auto result = _compute(first, second);
    auto value = riscv::convert(result);

    memoryAccess.putRegisterValue(destination, value);

    return _incrementProgramCounter<WordSize>(memoryAccess);
  }

  ValidationResult validate(MemoryAccess& memoryAccess) const override {
    auto result = _validateNumberOfChildren();
    if (!result.isSuccess()) return result;

    auto childrenResult = _validateChildren(memoryAccess);
    if (!childrenResult.isSuccess()) {
      return childrenResult;
    }

    if (_isImmediate) {
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
  // Since all children will definitely be final classes, we can do this here.
  using super = ArchitectureOnlyInstructionNode<WordSize, OperationSize>;

  /**
   * Performs the actual computation of the instruction.
   *
   * This method can only be called on ArchitectureOnlyInstructionNode itself if
   * the
   * appropriate constructor was called.
   *
   * \first The first operand.
   * \second The second operand.
   *
   * \return The result of the computation.
   */
  virtual WordSize _compute(OperationSize first, OperationSize second) const
      noexcept {
    assert(_operation);
    return _operation(first, second);
  }

  OperationSize _getLower5Bits(OperationSize op) const {
    return op & 0b11111;
  }

  /**
   * Performs sign-expansion on the given value. Sign expansion means, that
   * the upper bits of the resulting type are set to 1, if the sign bit of
   * value is set.
   * \param value The value to be sign expanded
   */
  WordSize _signExpand(OperationSize value) const {
    // Aquire the sign bit
    constexpr auto length = sizeof(OperationSize) * CHAR_BIT;
    OperationSize sign = (value & (OperationSize{1} << (length - 1)));
    WordSize result = value;// This zero-expands value
    // Do sign-expansion if needed
    if (sign > 0) {
      // Sign-expansion is quite easy here: All the bits above the lower
      // 32 bits are set to 1.
      constexpr auto requiredLength = sizeof(WordSize) * CHAR_BIT;
      result |= (length < requiredLength) ? (~WordSize{0} << length) : 0;
    }
    return result;
  }

 private:
  OperationSize _getChildValue(size_t index, MemoryAccess& memoryAccess) const {
    auto memory = _children[index]->getValue(memoryAccess);
    return riscv::convert<OperationSize>(memory);
  }

  ValidationResult _validateNumberOfChildren() const {
    // a integer instruction needs exactly 3 operands
    if (_children.size() != 3) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "This instruction must have exactly %1 operands"),
          std::to_string(3));
    }

    return ValidationResult::success();
  }

  ValidationResult _validateImmediateSize(MemoryAccess& memoryAccess) const {
    if (_isImmediate && _children[2]->getType() == Type::IMMEDIATE) {
      // no memory access is needed for a immediate node
      MemoryValue value = _children.at(2)->getValue(memoryAccess);

      if (Utility::occupiesMoreBitsThan(value, 12)) {
        return ValidationResult::fail(
            QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                              "The immediate value of this instruction must "
                              "be representable by %1 bits"),
            std::to_string(12));
      }
    }

    return ValidationResult::success();
  }

  ValidationResult _validateOperandsForImmediateInstructions() const {
    if (!_requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 2) ||
        !_requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 2, 1)) {
      return ValidationResult::fail(QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                                                      "This instruction must "
                                                      "have 2 registers and 1 "
                                                      "immediate as operands"));
    }

    return ValidationResult::success();
  }

  ValidationResult _validateOperandsForNonImmediateInstructions() const {
    // a register integer instruction needs three register operands
    if (!_requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 3)) {
      return ValidationResult::fail(QT_TRANSLATE_NOOP(
          "Syntax-Tree-Validation",
          "This instruction must have 3 registers as operands"));
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

#endif /* ERAGPSIM_ARCH_RISCV_ARCHITECTURE_ONLY_INSTRUCTION_NODE_HPP_ */
