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

/**
 * Represents the mul, mulh((s)u) register-register multiplication instruction.
 * For more information see RISC-V specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate on
 */
template <typename SizeType>
class MultiplicationInstruction : public IntegerInstructionNode<SizeType> {
 public:
  /**
 * Describes what part of the result will be saved in the result register.
 * A multiplication of of n bit with n bit leads to a 2n bit result
 * LOW: The lower part of the result (bit 0...n-1) will be used as result
 * HIGH: The higher part of the result (bit n...2n-1) will be used as result
 */
  enum MultiplicationResultPart { LOW, HIGH };

  MultiplicationInstruction(InstructionInformation& info,
                            MultiplicationResultPart partOfResultReturned)
      : IntegerInstructionNode<SizeType>(info, false),
        _usePart(partOfResultReturned) {
    // assert that SizeType is an unsigned integer type
    assert(SizeType(0) - 1 >= 0);
  }

  /**
   * Performs a multiplication with the two given factors op1, op2.
   * @param op1 Factor
   * @param op2 Factor
   * @return MultiplicationResult = LOW: lower SizeType bit of the
   * multiplication result, MultiplicationResult = HIGH: high SizeType bit of
   * the multiplication result
   */
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
  SizeType computeHighOrderBitsOfMultiplication(SizeType a, SizeType b) const {
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

  /**
   * Defines behaviour for the "h" extension of the mul-instruction
   * When _usePart == HIGH, this instruction is a mulh(...) instruction,
   * when _usePart == LOW, this instruction is the mul instructions
   */
  MultiplicationResultPart _usePart;
};

template <typename UnsignedSizeType, typename SignedSizeType>
class DivisionInstruction : public IntegerInstructionNode<UnsignedSizeType> {
 public:
  DivisionInstruction(InstructionInformation& info, bool isSignedDivision)
      : IntegerInstructionNode<UnsignedSizeType>(info, false), _isSignedDivision(isSignedDivision) {}

  UnsignedSizeType performIntegerOperation(UnsignedSizeType op1, UnsignedSizeType op2) const {
    // Semantics for division is defined in RISC-V specification in table 5.1

    if (op2 == 0) {
      // Division by zero
      // TODO kann vermutlich zusammengefasst werden
      if (_isSignedDivision) {
        return UnsignedSizeType(-1);
      } else {
        return -UnsignedSizeType(1);
      }
    }
    if (_isSignedDivision &&
        op1 == UnsignedSizeType(1) << ((sizeof(UnsignedSizeType) * CHAR_BIT) - 1) &&
        op2 == UnsignedSizeType(-1)) {
      // Signed Division overflow
      return op1;  // op1 is exactly -2^(n-1)
    }

    if(_isSignedDivision) {
        SignedSizeType sop1 = static_cast<SignedSizeType>(op1);
        SignedSizeType sop2 = static_cast<SignedSizeType>(op2);
        SignedSizeType result = sop1/sop2;
        return static_cast<UnsignedSizeType>(result);
    }
    return op1 / op2;
  }

 private:
  bool _isSignedDivision;
};

template <typename UnsignedSizeType, typename SignedSizeType>
class RemainderInstruction : public IntegerInstructionNode<UnsignedSizeType> {
 public:
  RemainderInstruction(InstructionInformation& info, bool isSignedRemainder)
      : IntegerInstructionNode<UnsignedSizeType>(info, false), _isSignedRemainder(isSignedRemainder) {}

  UnsignedSizeType performIntegerOperation(UnsignedSizeType op1, UnsignedSizeType op2) const {
    // Semantics for division is defined in RISC-V specification in table 5.1

    if (op2 == 0) {
      // Division by zero
      return op1;
    }
    if (_isSignedRemainder &&
        op1 == UnsignedSizeType(1) << ((sizeof(UnsignedSizeType) * CHAR_BIT) - 1) &&
        op2 == UnsignedSizeType(-1)) {
      // Signed Division overflow
      return 0;
    }
    if(_isSignedRemainder) {
        SignedSizeType sop1 = static_cast<SignedSizeType>(op1);
        SignedSizeType sop2 = static_cast<SignedSizeType>(op2);
        SignedSizeType result = sop1%sop2;
        return static_cast<UnsignedSizeType>(result);
    }
    return op1 % op2;
  }

 private:
  bool _isSignedRemainder;
};
}
#endif /* ERAGPSIM_ARCH_RISCV_MUL_DIV_INSTRUCTIONS_HPP_ */
