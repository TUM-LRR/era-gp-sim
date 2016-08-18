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

#ifndef ERAGPSIM_ARCH_RISCV_MUL_DIV_INSTRUCTIONS_HPP_
#define ERAGPSIM_ARCH_RISCV_MUL_DIV_INSTRUCTIONS_HPP_

#include <climits>

#include "arch/riscv/integer-instructions.hpp"

/*
 * TODO Instructions: mul mulh mulhu mulhsu div divu rem remu
 */
namespace riscv {

template <typename SizeType>
class MultiplicationInstruction : public IntegerInstructionNode<SizeType> {
 public:
  enum RESULT { LOW, HIGH };

  MultiplicationInstruction(InstructionInformation info,
                            RESULT partOfResultReturned)
      : IntegerInstructionNode<SizeType>(info, false),
        _usePart(partOfResultReturned) {
    // assert that SizeType is an unsigned integer type
    assert(SizeType(0) - 1 >= 0);
  }

  SizeType performIntegerOperation(SizeType op1, SizeType op2) const override {
    if (_usePart == LOW) {
      return op1 * op2;
    } else if (_usePart == HIGH) {
      /*
       * Native * multiplication only leads to the low order bits.
       * For 32bit a cast to 64bit, multiplication and then shifting could done,
       * but 64bit x 64bit multiplication would need 128bit integer types which
       * are not included in the c++ standard. So we do this manually for all
       * sizes
       */
      return computeHighOrderBitsOfMultiplication(op1, op2);
    }
    // Invalid enum
    assert(false);
  }

 private:
  /**
   * \brief A n-bit x n-bit multiplication leads up to a n+n-bit result.
   * "Normal" multiplication (*) only yields the lower n-bit part.
   * This method computes the upper n-bit part of such a multiplication.
   * Using long multiplication:
   * The numbers a and b are each split up in high and low parts, so that
   * a*b = (ah+al)*(bh+bl).
   * All expansions (ah*bh, ah*bl, ...) are computed within the range of the
   * given integer size type to register possible overflows that need to be
   * accounted for. In the last step ah*bh is added to correctly shifted ah*bl,
   * al*bh and the carry bit of the low order bit multiplication
   * \param a factor
   * \param b factor
   * \return high order bits of a*b
   */
  SizeType computeHighOrderBitsOfMultiplication(SizeType a, SizeType b) const{
    // size of each(high/low) part of each number (CHAR_BIT describes number of
    // bits in one byte)
    // e.g. for 32bit numbers -> 16, for 64bit numbers -> 32
    auto halfBitSize = (sizeof(SizeType) * CHAR_BIT) / 2;
    // computes a bitmask for and that ignores the upper part
    // e.g. for 32bit -> 0xFFFF, for 64bit -> 0xFFFF FFFF
    SizeType maskLowerBits = -SizeType(1) >> halfBitSize;

    // high and low parts of each number
    SizeType ah = a >> halfBitSize;
    SizeType al = a & maskLowerBits;
    SizeType bh = b >> halfBitSize;
    SizeType bl = b & maskLowerBits;

    // compute expansions of (ah+al)*(bh+bl)
    SizeType ahxbh = ah * bh;
    SizeType ahxbl = ah * bl;
    SizeType alxbh = al * bh;
    SizeType alxbl = al * bl;

    // grab carry bit of low order bit multiplication
    SizeType carryLowerPart =
        ((ahxbl & maskLowerBits) + (alxbh & maskLowerBits) +
         (alxbl >> halfBitSize)) >>
        halfBitSize;
    SizeType result = ahxbh + (ahxbl >> halfBitSize) + (alxbh >> halfBitSize) +
                      carryLowerPart;
    return result;
  }

  RESULT _usePart;
};
}
#endif /* ERAGPSIM_ARCH_RISCV_MUL_DIV_INSTRUCTIONS_HPP_ */
