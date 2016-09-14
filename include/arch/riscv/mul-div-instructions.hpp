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
#include "arch/riscv/word-instruction-wrapper.hpp"

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

  /**
   * Describes the type of this multiplication instruction.
   * UNSIGNED: all operands are treated as unsigned values
   * SIGNED: all operands are treated as signed values
   * SIGNED_UNSIGNED: the first operand is a signed value, the second operand is
   * a unsigned value
   */
  enum Type { UNSIGNED, SIGNED, SIGNED_UNSIGNED };

  MultiplicationInstruction(InstructionInformation& info,
                            MultiplicationResultPart partOfResultReturned,
                            Type type)
      : IntegerInstructionNode<SizeType>(info, false),
        _usePart(partOfResultReturned),
        _type(type) {
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
      /*
       * The "signedness" of the operation is ignored, as the sign does not
       * affect multiplying the lower part
       */
      return op1 * op2;
    } else if (_usePart == HIGH) {
      /*
       * Native * multiplication only leads to the low order bits.
       * For 32bit a cast to 64bit, multiplication and then shifting could be
       * done,
       * but 64bit x 64bit multiplication would need 128bit integer types which
       * are not included in the c++ standard. So we do this manually for all
       * sizes
       */

      if (_type == UNSIGNED) {
        return computeHighOrderBitsOfMultiplication(op1, op2);
      } else if (_type == SIGNED) {
        /*
         * We can only get the high part for a unsigned x unsigned
         * multiplication.
         * For a signed multiplication we convert the signed value into an
         * unsigned value (every signed value fits into the unsigned version
         * with the same size without losing data) with the same magnitude, get
         * ordinary the high part and possibly negate the result, depending on
         * the sign rules
         *
         * The same applies for signed x unsigned multiplication. Here just the
         * first operand is treated as signed and converted
         */

        SizeType unsignedOp1 = toUnsigned(op1);
        SizeType unsignedOp2 = toUnsigned(op2);
        SizeType result =
            computeHighOrderBitsOfMultiplication(unsignedOp1, unsignedOp2);
        if (getSign(op1) != getSign(op2)) {
          // the signs are either +,- or -,+ -> result will be negative
          return negateUpperPart(result, unsignedOp1, unsignedOp2);
        } else {
          return result;
        }
      } else if (_type == SIGNED_UNSIGNED) {
        // convert op1 to unsigned
        SizeType unsignedOp1 = toUnsigned(op1);
        SizeType result =
            computeHighOrderBitsOfMultiplication(unsignedOp1, op2);
        // unsigned type has always sign +
        if (getSign(op1) != 0) {
          // singed type has sign - -> result will be negative
          return negateUpperPart(result, unsignedOp1, op2);
        } else {
          return result;
        }
      } else {
        assert(false);  // invalid _type
      }
    }
    assert(false);  // invalid MultiplicationResultPart
  }

 private:
  /** The index of the sign bit of the given SizeType, for 32bit SizeType
   * signIndex == 31*/
  static constexpr SizeType signIndex =
      sizeof(SizeType) * CHAR_BIT - SizeType(1);
  /** A and-mask for cutting out everything except the sign bit*/
  static constexpr SizeType signMask = SizeType(1) << signIndex;

  /**
   * Returns the magnitude of the given value t. Specificly returns the same
   * value, if the sign bit of t is 0, t itself is returned. If the sign bit is
   * 1, t is negated by using two's complement
   * @param t value to be converted to an unsigned value
   * @return value v, so that v = |t|
   */
  SizeType toUnsigned(SizeType t) const {
    if (getSign(t) == 0) {
      return t;
    } else {
      return (~t) + 1;
    }
  }

  /**
   * Returns the sign bit of t at index 0
   * @param t value to get the sign bit from
   * @return 0, if the sign bit of t is 0; 1, if the sign bit of t is 1
   */
  SizeType getSign(SizeType t) const {
    SizeType sign = (t & signMask) >> signIndex;
    return sign;
  }

  /**
   * Returns the negated value of upper by using two's complement. upper is
   * treated to be the upper part of the unsigned x unsigned multiplication of a
   * and b. a and b are needed to retrieve the carry bit of the lower part of
   * the multiplication result when adding 1
   * @param upper value to be negated
   * @param a factor of multiplication
   * @param b factor of multiplication
   * @return
   */
  SizeType negateUpperPart(SizeType upper, SizeType a, SizeType b) const {
    SizeType signedUpper = ~upper;
    // when adding 1, a carry bit to the upper part can only occur, when the
    // lower part is 0 (after flipping 11..11 +1 => 1 00..00)
    if (a * b == SizeType(0)) {
      signedUpper += 1;
    }
    return signedUpper;
  }

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

  /**
   * Defines behaviour of the h extension in addition of "/u/su" for signed x
   * signed, unsigned x unsigned, signed x unsigned multiplication.
   */
  Type _type;
};

/**
 * Superclass for instructions, that have a word variant in rv64 but cannot use
 * the WordInstructionWrapper. This may be due to special return values that
 * need special treatment (e.g. division by 0).
 * It is intended that the instructions derive from this class and handle the word variant and special value treatment.
 * This class provided utility functions for the task of creating word sized values.
 * \tparam \see IntegerInstructionNode template param
 */
template <typename SizeType>
class WordableIntegerInstruction : public IntegerInstructionNode<SizeType> {
 public:
    /**
   * \param info InstructionInformation for this instruction
   * \param isWordInstruction determinates if this instance represents a word instruction
   */
  WordableIntegerInstruction(InstructionInformation& info,
                             bool isWordInstruction)
      : IntegerInstructionNode<SizeType>(info, false),
        _isWordInstruction(isWordInstruction) {}

  /** default destructor */
  virtual ~WordableIntegerInstruction() = default;

 protected:
  /**
   * Truncates n to represent a value that could fit into a word representation (so half of the current bits).
   * The result is sign-extended under certain conditions: isSignedOperation and n represents a negative value.
   * The result is sign-extended (even when upper condition is false) if signExtendAnyway is true
   * \param n The value that will be truncated to fit into a representation half of its bit size
   * \param isSignedOperation If true, n is treated as a signed value, otherwise n is treated as an unsigned value
   * \param signExtendAnyway If true, n will be sign-extended no matter what the signedness of n is
   * \return worded n
   */
  SizeType toWord(SizeType n, bool isSignedOperation,
                  bool signExtendAnyway) const {
    bool negative = (n & SIGN_MASK) > 0;
    if (signExtendAnyway || (isSignedOperation && negative)) {
      n = n | SIGN_EXTENSION;
    } else {
      n = n & WORD_MASK;
    }
    return n;
  }

  /**
   * \return true, if this instruction is a word instruction; false, if not
   */
  bool isWordInstruction() const { return _isWordInstruction; }

 private:
  /** and-bitmask for retrieving the sign bit*/
  static constexpr SizeType SIGN_MASK = SizeType(1)
                                        << (sizeof(SizeType) * CHAR_BIT - 1);
  /** and-bitmask to only preserve the lower half of bits*/
  static constexpr SizeType WORD_MASK = SizeType(-1) >>
                                        ((sizeof(SizeType) * CHAR_BIT) / 2);
  /** or-bitmask for adding 1s to the higher half of bits*/
  static constexpr SizeType SIGN_EXTENSION =
      SizeType(-1) << ((sizeof(SizeType) * CHAR_BIT) / 2);
  /**
   * Describes if this instruction is a word instruction
   */
  bool _isWordInstruction;
};

/**
 * Represents a RISC-V "div/divu/divw/divuw" instruction. For more information see RISC-V
 * specification
 * \tparam unsigned integer type that can hold exactly the range of values that
 * divu should operate on
 * \tparam signed integer type that can hold exactly the range of values that
 * div should operate on
 */
template <typename UnsignedSizeType, typename SignedSizeType>
class DivisionInstruction
    : public WordableIntegerInstruction<UnsignedSizeType> {
 public:
  DivisionInstruction(InstructionInformation& info, bool isSignedDivision,
                      bool isWordInstruction)
      : WordableIntegerInstruction<UnsignedSizeType>(info, isWordInstruction),
        _isSignedDivision(isSignedDivision) {}

  UnsignedSizeType performIntegerOperation(UnsignedSizeType operand1,
                                           UnsignedSizeType operand2) const {
    // Semantics for division is defined in RISC-V specification in table 5.1

    UnsignedSizeType op1 = operand1;
    UnsignedSizeType op2 = operand2;
    if (this->isWordInstruction()) {
      op1 = this->toWord(op1, _isSignedDivision, false);
      op2 = this->toWord(op2, _isSignedDivision, false);
    }

    if (op2 == 0) {
      // Division by zero
      return UnsignedSizeType(-1);
    }
    UnsignedSizeType result = 0;
    bool resultIsNegative = false;
    if (_isSignedDivision && op1 == OVERFLOW_DIVIDENT &&
        op2 == OVERFLOW_DIVISOR) {
      // Signed Division overflow
      return operand1;  // operand1 is exactly -2^(n-1)
    }

    if (_isSignedDivision) {
      // this is a rather ugly solution to perform a native signed division. The
      // class cannot be instatiated with a signed template parameter, because
      // the conversions do not allow (at the moment) a distinction for
      // unsigned/signed conversion at runtime. Such a distinction if(isSigned)
      // then convertSigned(type) else convertUnsigned(type) will not compile,
      // as e.g. convertSigned() won't compile for a unsigned type and vice
      // versa
      SignedSizeType sop1 = static_cast<SignedSizeType>(op1);
      SignedSizeType sop2 = static_cast<SignedSizeType>(op2);
      SignedSizeType sresult = sop1 / sop2;
      result = static_cast<UnsignedSizeType>(sresult);
      resultIsNegative = sresult < 0;
    } else {
      result = op1 / op2;
    }
    if (this->isWordInstruction()) {
      result = this->toWord(result, _isSignedDivision, resultIsNegative);
    }

    return result;
  }

 private:
  static constexpr UnsignedSizeType OVERFLOW_DIVIDENT =
      UnsignedSizeType(1) << (sizeof(UnsignedSizeType) * CHAR_BIT - 1);
  static constexpr UnsignedSizeType OVERFLOW_DIVISOR = UnsignedSizeType(-1);
  bool _isSignedDivision;
};

/**
 * Represents a RISC-V "rem/remu/remw/remuw" instruction. For more information see RISC-V
 * specification
 * \tparam unsigned integer type that can hold exactly the range of values that
 * remu should operate on
 * \tparam signed integer type that can hold exactly the range of values that
 * rem should operate on
 */
template <typename UnsignedSizeType, typename SignedSizeType>
class RemainderInstruction
    : public WordableIntegerInstruction<UnsignedSizeType> {
 public:
  RemainderInstruction(InstructionInformation& info, bool isSignedRemainder,
                       bool isWordInstruction)
      : WordableIntegerInstruction<UnsignedSizeType>(info, isWordInstruction),
        _isSignedRemainder(isSignedRemainder) {}

  UnsignedSizeType performIntegerOperation(UnsignedSizeType operand1,
                                           UnsignedSizeType operand2) const {
    // Semantics for division is defined in RISC-V specification in table 5.1
    UnsignedSizeType op1 = operand1;
    UnsignedSizeType op2 = operand2;
    if (this->isWordInstruction()) {
      op1 = this->toWord(op1, _isSignedRemainder, false);
      op2 = this->toWord(op2, _isSignedRemainder, false);
    }
    if (op2 == 0) {
      // Division by zero
      return operand1;
    }

    if (_isSignedRemainder && op1 == OVERFLOW_DIVIDENT &&
        op2 == OVERFLOW_DIVISOR) {
      // Signed Division overflow
      return 0;
    }

    UnsignedSizeType result = 0;
    bool resultIsNegative = false;
    if (_isSignedRemainder) {
      // this is a rather ugly solution to perform a native signed modulus
      // operation. The
      // class cannot be instatiated with a signed template parameter, because
      // the conversions do not allow (at the moment) a distinction for
      // unsigned/signed conversion at runtime. Such a distinction if(isSigned)
      // then convertSigned(type) else convertUnsigned(type) will not compile,
      // as e.g. convertSigned() won't compile for a unsigned type and vice
      // versa
      SignedSizeType sop1 = static_cast<SignedSizeType>(op1);
      SignedSizeType sop2 = static_cast<SignedSizeType>(op2);
      SignedSizeType sresult = sop1 % sop2;
      resultIsNegative = sresult < 0;
      result = static_cast<UnsignedSizeType>(sresult);
    } else {
      result = op1 % op2;
    }

    if (this->isWordInstruction()) {
      result = this->toWord(result, _isSignedRemainder, resultIsNegative);
    }
    return result;
  }

 private:
  static constexpr UnsignedSizeType OVERFLOW_DIVIDENT =
      UnsignedSizeType(1) << (sizeof(UnsignedSizeType) * CHAR_BIT - 1);
  static constexpr UnsignedSizeType OVERFLOW_DIVISOR = UnsignedSizeType(-1);
  bool _isSignedRemainder;
};
}
#endif /* ERAGPSIM_ARCH_RISCV_MUL_DIV_INSTRUCTIONS_HPP_ */
