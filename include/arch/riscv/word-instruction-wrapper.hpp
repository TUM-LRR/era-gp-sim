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
#include <type_traits>
#include <utility>

#include "arch/riscv/instruction-node-factory.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "arch/riscv/integer-instructions.hpp"

#include <iostream>

namespace riscv {

template <typename SizeType>
class ISpecialcaseWordInstruction {
 public:
  virtual ~ISpecialcaseWordInstruction() = default;
  virtual bool signExtensionNeeded(SizeType op1, SizeType op2)const  = 0;
};

using ResultSizeType = InstructionNodeFactory::RV64_integral_t;
using OperandSizeType = InstructionNodeFactory::RV32_integral_t;

template <class WrappedType>
class WordInstructionWrapper : public IntegerInstructionNode<ResultSizeType> {
 public:
  using WordInstruction = IntegerInstructionNode<OperandSizeType>;

  template <class... Args>
  WordInstructionWrapper(bool expectSignedResult,
                         InstructionInformation& instructionInformation,
                         bool isImmediateInstruction, Args&&... args)
      : IntegerInstructionNode<ResultSizeType>(instructionInformation,
                                               isImmediateInstruction),
        _signedResult(expectSignedResult) {
    _wrapped = std::make_unique<WrappedType>(std::forward<Args>(args)...);
  }

  ResultSizeType performIntegerOperation(ResultSizeType op1,
                                         ResultSizeType op2) const override {
    OperandSizeType op1_trunc = op1;
    OperandSizeType op2_trunc = op2;
    OperandSizeType result_trunc =
        _wrapped->performIntegerOperation(op1_trunc, op2_trunc);
    ResultSizeType result = result_trunc;
    if ((std::is_base_of < ISpecialcaseWordInstruction<OperandSizeType>, WrappedType>::value &&
         dynamic_cast<const ISpecialcaseWordInstruction<OperandSizeType>*>(&(*_wrapped))->signExtensionNeeded(
             op1_trunc, op2_trunc)) ||
        (_signedResult && isNegative(result_trunc))) {
      // result = result & SIGNED_WORD_MASK_LOWER31;  // preserve lower 31 bit
      result = result | SIGN_EXTENSION_MASK;
      return result;
    } else {
      result = result & WORD_MASK_LOWER32;
    }
    return result;
  }

 private:
  bool isNegative(OperandSizeType n) const {
    OperandSizeType signBit = n & (OperandSizeType(1) << 31);
    return signBit != 0;
  }

  std::unique_ptr<WordInstruction> _wrapped;

  bool _signedResult;

  static constexpr ResultSizeType SIGNED_WORD_MASK_LOWER31 = 0x7FFFFFFF;
  static constexpr ResultSizeType WORD_MASK_LOWER32 = 0xFFFFFFFF;
  static constexpr ResultSizeType SIGN_EXTENSION_MASK = 0xFFFFFFFF8 << 28;
};
}
#endif  // ERAGPSIM_ARCH_RISCV_WORDINSTRUCTIONWRAPPER_HPP
