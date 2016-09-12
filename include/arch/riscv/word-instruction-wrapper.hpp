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

#include <iostream>

namespace riscv {

using SizeType = InstructionNodeFactory::RV64_integral_t;

template <class WrappedType>
class WordInstructionWrapper : public IntegerInstructionNode<SizeType> {
 public:
  using DoubleWordInstruction = IntegerInstructionNode<SizeType>;

  template <class... Args>
  WordInstructionWrapper(bool expectSignedResult,
                         InstructionInformation& instructionInformation,
                         bool isImmediateInstruction, Args&&... args)
      : IntegerInstructionNode<SizeType>(instructionInformation,
                                         isImmediateInstruction),
        _signedResult(expectSignedResult) {
    _wrapped = std::make_unique<WrappedType>(std::forward<Args>(args)...);
  }

  SizeType performIntegerOperation(SizeType op1, SizeType op2) const override {
    SizeType result = _wrapped->performIntegerOperation(op1, op2);
    if (_signedResult && isNegative(result)) {
      //result = result & SIGNED_WORD_MASK_LOWER31;  // preserve lower 31 bit
      result = result | SIGN_EXTENSION_MASK;
      return result;
    } else {
      result = result & WORD_MASK_LOWER32;
    }
    return result;
  }

 private:
  bool isNegative(SizeType n) const {
    SizeType signBit = n & (SizeType(1) << 63);
    return signBit != 0;
  }

  std::unique_ptr<DoubleWordInstruction> _wrapped;

  bool _signedResult;

  static constexpr SizeType SIGNED_WORD_MASK_LOWER31 = 0x7FFFFFFF;
  static constexpr SizeType WORD_MASK_LOWER32 = 0xFFFFFFFF;
  static constexpr SizeType SIGN_EXTENSION_MASK = 0xFFFFFFFF8 << 28;
};
}
#endif  // ERAGPSIM_ARCH_RISCV_WORDINSTRUCTIONWRAPPER_HPP
