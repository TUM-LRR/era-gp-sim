/* C++ Assembler Interpreter
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.*/
#ifndef ERAGPSIM_ARCH_RISCV_WORDINSTRUCTIONWRAPPER_HPP
#define ERAGPSIM_ARCH_RISCV_WORDINSTRUCTIONWRAPPER_HPP

#include <climits>
#include <memory>
#include <utility>

#include "arch/riscv/instruction-node-factory.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "arch/riscv/integer-instructions.hpp"

namespace riscv {

using ResultSizeType = riscv::unsigned64_t;
using OperandSizeType = riscv::unsigned32_t;

/**
 * Wrapper class for reusing the "arithmetic calculation" code for most of the
 * word instructions in rv64i.
 * This class itself is a 64bit IntegerInstructionNode, but the real arithmetic
 * calculation will be performed in the WrappedType.
 * This class will truncate the 64bit integer operands to 32bit integer
 * operands, uses then the WrappedType for calculating a 32bit result and will
 * sign-extend the result to 64bit.
 * \tparam WrappedType The class type that performs the arithmetic calculations.
 * WrappedType must have
 * IntegerInstructionNode<InstructionNodeFactory::RV32_integral_t>> as a base
 * class.
 */
template <class WrappedType>
class WordInstructionWrapper
    : public AbstractIntegerInstructionNode<ResultSizeType> {
 public:
  using super = AbstractIntegerInstructionNode<ResultSizeType>;
  using WordInstruction = AbstractIntegerInstructionNode<OperandSizeType>;
  using Operands = super::Operands;

  /**
  * Creates a WordInstructionWrapper instance that wraps around a instance of
  * WrappedType created from args
  * \tparam Args Arguments types for a constructor of WrappedType (can often be
  *omitted (template param auto deduction))
  * \param expectSignedResult if true, the result will be treated as signed and
  * will possible get sign-extended if negative, if false the result will be
  * treated as unsigned
  * \param instructionInformation InstructionInformation for the instruction
  * that this wrapper represents (e.g. "mulw" for a
  * WordInstructionWrapper<MultiplicationInstruction<32bit>>(...))
  * \param isImmediateInstruction \see
  * IntegerInstructionNode::IntegerInstructionNode(InstructionInformation&,bool)
  *\param args Arguments for a constructor of WrappedType
  */
  template <class... Args>
  WordInstructionWrapper(const InstructionInformation& instructionInformation,
                         Operands operands, bool expectSignedResult, Args&&... args)
      : AbstractIntegerInstructionNode<ResultSizeType>(instructionInformation,
                                                       operands),
        _signedResult(expectSignedResult) {
    // create an instance of WrappedType
    _wrapped = std::make_unique<WrappedType>(instructionInformation, std::forward<Args>(args)...);
  }

protected:

  /**
   * Truncates op1, op2 to 32bit, performs the arithmetic calculation using the
   * WrappedType and then converts the 32bit result into a 64bit result, which
   * may be sign-extended depending on the constructor arguments
   * \param op1 First operand for the arithmetic operation of WrappedType
   * \param op2 Second operand for the arithmetic operation of WrappedType
   * \return result of the arithmetic operation
   */
  ResultSizeType _compute(ResultSizeType op1,
                          ResultSizeType op2) const noexcept override {
    OperandSizeType op1_trunc = op1;
    OperandSizeType op2_trunc = op2;
    OperandSizeType result_trunc =
        _wrapped->_compute(op1_trunc, op2_trunc);
    ResultSizeType result = result_trunc;
    if (_signedResult && isNegative(result_trunc)) {
      result = result | SIGN_EXTENSION_MASK;
      return result;
    } else {
      result = result & WORD_MASK_LOWER32;
    }
    return result;
  }

 private:
  /**
   * Determines if the given value n has the signbit set
   * \param n
   * \return true if the signbit is set and therefore n represents a negative
   * number, otherwise false
   */
  bool isNegative(OperandSizeType n) const noexcept {
    OperandSizeType signBit = n & (OperandSizeType(1) << 31);
    return signBit != 0;
  }

  /**
   * The wrapped type instance that will be used for the arithmetic calculations
   */
  std::unique_ptr<WordInstruction> _wrapped;

  /**
   * Describes how the result is treated in terms of signedness.
   * True, if the result is a signed result;
   * False, if the result is unsigned
   */
  bool _signedResult;

  /** and-bitmask for preserving the lower 31bit (a 32bit value without its
   * signbit)*/
  static constexpr ResultSizeType SIGNED_WORD_MASK_LOWER31 = 0x7FFFFFFF;
  /** and-bitmask for preserving the lower 32bit (a 32bit unsigned value)*/
  static constexpr ResultSizeType WORD_MASK_LOWER32 = 0xFFFFFFFF;
  /** or-bitmask to fill the bits 63...32 with 1 (sign extension for a 64bit
   * value that could be represented in 32bit signed)*/
  static constexpr ResultSizeType SIGN_EXTENSION_MASK = 0xFFFFFFFF8 << 28;
};
}
#endif  // ERAGPSIM_ARCH_RISCV_WORDINSTRUCTIONWRAPPER_HPP
