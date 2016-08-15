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

#ifndef ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTIONS_HPP_
#define ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTIONS_HPP_

#include <cassert>
#include <string>

#include "arch/common/instruction-information.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "arch/riscv/conversion-stub.hpp"

/*
 * TODO Instructions: slt sltu and or xor sll srl sra
 *                    + their respective immediate equivalents
 */

namespace riscv {

/*!
 * \brief The IntegerInstructionNode is a superclass for all integer arithmetic
 * instructions.
 * As the behaviour of all of these instructions is very similar, this class
 * summarizes the common behaviour
 * to avoid code redundancy.
 * The instruction internally converts the MemoryValues of their operands to a
 * built-in integer type, performs the operation with this built-in type and
 * converts the result to a MemoryValue which is stored in the destination
 * register
 * \tparam SizeType built-in integer type on which the actual operation is
 * performed
 */
template <typename SizeType>
class IntegerInstructionNode : public InstructionNode {
 public:
  /*!
 * Creates a IntegerInstructionNode using the specified InstructionInformation
 * to describe the instruction
 * \param info InstructionInformation that holds the mnemonic of this
 * instruction
 * \param immediateInstruction when the instruction is labeled as immediate
 * instruction, 2 register and one immediate operand are expected for
 * validation, if not 3 register operands are expected
 */
  IntegerInstructionNode(InstructionInformation& info,
                         bool immediateInstruction)
      : InstructionNode(info), _isImmediate(immediateInstruction) {}

  /** Default destructor*/
  ~IntegerInstructionNode() = default;

  MemoryValue getValue(DummyMemoryAccess& memory_access) const override {
    assert(validate());
    // Get the destination register
    std::string destination = _children.at(0)->getIdentifier();

    // Evaluate the operands
    MemoryValue memoryV1 = _children.at(1)->getValue(memory_access);
    MemoryValue memoryV2 = _children.at(2)->getValue(memory_access);

    SizeType operand1 = convert<SizeType>(memoryV1, RISCV_BYTEORDER);
    SizeType operand2 = convert<SizeType>(memoryV2, RISCV_BYTEORDER);

    SizeType result = performIntegerOperation(operand1, operand2);

    MemoryValue resultValue =
        convert(result, RISCV_BITS_PER_BYTE, RISCV_BYTEORDER);
    memory_access.setRegisterValue(destination, resultValue);
    return MemoryValue{};
  }

  bool validate() const override {
    // a integer instruction needs exactly 3 operands
    if (_children.size() != 3) {
      return false;
    }
    // check if all operands are valid themselves
    if (!validateAllChildren()) {
      return false;
    }

    if (_isImmediate &&
        _children.at(2)->getType() == AbstractSyntaxTreeNode::Type::IMMEDIATE) {
      // check if immediate operand is represented by only 20 bits
      DummyMemoryAccessStub stub;
      // no memory access is needed for a immediate node
      MemoryValue value = _children.at(2)->getValue(stub);
      //look for 1 in bits 20...value.getSize()
      for(std::size_t index = 20; index < value.getByteSize(); ++index) {
          if(value.get(index)) {
              return false;//1 detected
          }
      }
//      auto bits20 = value.getValue() & (~0xFFFFF);  // 2097151 = 0b11111...1 (20
//                                                    // times a 1) -> erase lower
//                                                    // 20 bits
//      if (value != lower20bit) {
//        // there is a 1 somewhere in bit 20 to x => the value is not represented
//        // by only bit 0...19
//        return false;
//      }
    }

    // a immediate integer instruction needs two register operands followed by
    // one immediate operand
    if (_isImmediate) {
      return requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 2) &&
             requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 2, 1);
    } else {
      // a register integer instruction needs three register operands
      return requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 3);
    }
  }

  /*!
   * performs the specific arithmetic integer operation (such as +, -, and, or,
   * etc) and returns the result
   * \param op1 first operand for the arithmetic operation
   * \param op2 second operand for the arithmetic operation
   * \return result of op1 <arithmeticOperation> op2
   */
  virtual SizeType performIntegerOperation(SizeType op1,
                                           SizeType op2) const = 0;

 protected:
  SizeType getLower5Bit(SizeType op) const {
    constexpr SizeType andValue = 0b11111;
    return op & andValue;
  }

 private:
  /*!
   * Indicates if this instruction is a register-immediate instruction.
   * If false this instruction is a register-register instruction.
   * This value is used for validation of operands
   */
  bool _isImmediate;
};

/*!
 * Represents a RISC-V "add/addi" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class AddInstructionNode : public IntegerInstructionNode<SizeType> {
 public:
  AddInstructionNode(InstructionInformation info, bool immediateInstruction)
      : IntegerInstructionNode<SizeType>(info, immediateInstruction) {}

  /*!
   * Adds the two operands
   * \param op1 summand
   * \param op2 summand
   * \return op1 + op2
   */
  SizeType performIntegerOperation(SizeType op1, SizeType op2) const override {
    return op1 + op2;
  }
};

/*!
 * Represents a RISC-V "sub" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class SubInstructionNode : public IntegerInstructionNode<SizeType> {
 public:
  SubInstructionNode(InstructionInformation info)
      : IntegerInstructionNode<SizeType>(
            info, false)  // RISC-V does not specifiy a subi
  {}

  /*!
   * Subtracts op2 from op1
   * \param op1
   * \param op2
   * \return op1 - op2
   */
  SizeType performIntegerOperation(SizeType op1, SizeType op2) const override {
    return op1 - op2;
  }
};

/*!
 * Represents a RISC-V "and/andi" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class AndInstructionNode : public IntegerInstructionNode<SizeType> {
 public:
  AndInstructionNode(InstructionInformation info, bool isImmediateInstruction)
      : IntegerInstructionNode<SizeType>(info, isImmediateInstruction) {}

  /*!
   * Performs a bitwise logical and with op1, op2
   * \param op1
   * \param op2
   * \return op1 bitand op2
   */
  SizeType performIntegerOperation(SizeType op1, SizeType op2) const override {
    return op1 & op2;
  }
};

/*!
 * Represents a RISC-V "or/ori" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class OrInstructionNode : public IntegerInstructionNode<SizeType> {
 public:
  OrInstructionNode(InstructionInformation info, bool isImmediateInstruction)
      : IntegerInstructionNode<SizeType>(info, isImmediateInstruction) {}

  /*!
   * Performs a bitwise logical or with op1, op2
   * \param op1
   * \param op2
   * \return op1 bitor op2
   */
  SizeType performIntegerOperation(SizeType op1, SizeType op2) const override {
    return op1 | op2;
  }
};

/*!
 * Represents a RISC-V "xor/xori" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class XorInstructionNode : public IntegerInstructionNode<SizeType> {
 public:
  XorInstructionNode(InstructionInformation info, bool isImmediateInstruction)
      : IntegerInstructionNode<SizeType>(info, isImmediateInstruction) {}

  /*!
   * Performs a bitwise logical xor with op1, op2
   * \param op1
   * \param op2
   * \return op1 xor op2
   */
  SizeType performIntegerOperation(SizeType op1, SizeType op2) const override {
    return op1 ^ op2;
  }
};

/*!
 * Represents a RISC-V "sll/slli" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class ShiftLogicalLeftInstructionNode
    : public IntegerInstructionNode<SizeType> {
 public:
  ShiftLogicalLeftInstructionNode(InstructionInformation info,
                                  bool isImmediateInstruction)
      : IntegerInstructionNode<SizeType>(info, isImmediateInstruction) {}

  /*!
   * Shifts bits in op1 logical left (shifts zeros into the lower part). How
   * many zeros are shifted in is
   * determined by the lower 5bit of op2
   * \param op1
   * \param op2
   * \return op1 << lower5bit(op2)
   */
  SizeType performIntegerOperation(SizeType op1, SizeType op2) const override {
    return op1 << this->getLower5Bit(op2);
  }
};

/*!
 * Represents a RISC-V "srl/srli" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class ShiftLogicalRightInstructionNode
    : public IntegerInstructionNode<SizeType> {
 public:
  ShiftLogicalRightInstructionNode(InstructionInformation info,
                                   bool isImmediateInstruction)
      : IntegerInstructionNode<SizeType>(info, isImmediateInstruction) {
    // For logical right shift, SizeType must be a unsigned integral type
    // Due to the fact that signed right shift is implementation/compiler
    // specific
    // and can be either a logical shift or a arithmetical shift
    assert((SizeType(0) - 1) >= 0);
  }

  /*!
   * Shifts bits in op1 logical right (shifts zeros into the upper part). How
   * many zeros are shifted in is
   * determined by the lower 5bit of op2
   * \param op1
   * \param op2
   * \return op1 >> lower5bit(op2)
   */
  SizeType performIntegerOperation(SizeType op1, SizeType op2) const override {
    return op1 >> this->getLower5Bit(op2);
  }
};

/*!
 * Represents a RISC-V "sra/srai" instruction. For more information see RISC-V
 * specification
 * \tparam integer type that can hold exactly the range of values that this
 * operation should operate
 */
template <typename SizeType>
class ShiftArithmeticRightInstructionNode
    : public IntegerInstructionNode<SizeType> {
 public:
  ShiftArithmeticRightInstructionNode(InstructionInformation info,
                                      bool isImmediateInstruction)
      : IntegerInstructionNode<SizeType>(info, isImmediateInstruction) {}

  /*!
   * Shifts bits in op1 arithmetic right (shifts sign bit into the upper part).
   * How
   * many bits are shifted in is determined by the lower 5bit of op2
   * \param op1
   * \param op2
   * \return op1 >> lower5bit(op2)
   */
  SizeType performIntegerOperation(SizeType op1, SizeType op2) const override {
    // c++ standard does not define a arithemtic shift operator
    constexpr auto length = sizeof(SizeType) * 8;
    SizeType sign = (op1 & (SizeType(1) << (length - 1))) >> (length - 1);
    SizeType shiftCount = this->getLower5Bit(op2);
    SizeType tmp = op1 >> shiftCount;
    // erase upper shiftCount bits
    // put in sign bit
    for (auto i = length - shiftCount; i < length; ++i) {
      tmp = tmp & ~(SizeType(1) << i);
      tmp = tmp | (sign << i);
    }
    return tmp;
  }
};
}

#endif /* ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTIONS_HPP_ */
