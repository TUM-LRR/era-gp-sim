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

/*
 * TODO Instructions: slt sltu and or xor sll srl sra
 *                    + their respective immediate equivalents
 */

namespace riscv {

/**
 * Validates an integer instruction node. Every RISC V integer instruction is
 * either register-register or a register-immediate. This is a util method, that
 * checks, if the given node fulfills these requirements. See RISC V
 * specification for more information.
 *
 * \param node The node to check.
 * \param immediate Whether the node is the register-immediate representation.
 * \return true if the node matches the requirements.
 */
bool validateIntegerInstruction(InstructionNode& node, bool immediate);

/**
 * This node represents the sub/subi instruction.
 *
 * See RISC V specification for details about the instruction.
 */
class SubInstructionNode : public InstructionNode {
 public:
  SubInstructionNode(bool immediate)
      : InstructionNode(), _immediate(immediate) {}

  virtual MemoryValue getValue(DummyMemoryAccess& memory_access);

  virtual bool validate();

  virtual MemoryValue assemble() {
    return MemoryValue();  // TODO
  }

  virtual std::string getIdentifier() { return _immediate ? "SUBI" : "SUB"; }

 private:
  bool _immediate;
};

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
  IntegerInstructionNode(InstructionInformation info, bool immediateInstruction)
      : _instructionInformation(info), _isImmediate(immediateInstruction) {}

  /** Default destructor*/
  ~IntegerInstructionNode() = default;

  MemoryValue getValue(DummyMemoryAccess& memory_access) override {
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
    return MemoryValue();
  }

  std::string getIdentifier() override {
    assert(_instructionInformation.isValid() &&
           _instructionInformation.hasMnemonic());
    //return the mnemonic
    return _instructionInformation.getMnemonic();
  }

  bool validate() override {
    return validateIntegerInstruction(*this, _isImmediate);
  }

  /*!
   * performs the specific arithmetic integer operation (such as +, -, and, or, etc) and returns the result
   * \param op1 first operand for the arithmetic operation
   * \param op2 second operand for the arithmetic operation
   * \return result of op1 <arithmeticOperation> op2
   */
  virtual SizeType performIntegerOperation(SizeType op1, SizeType op2) = 0;

 private:
  /** byte order used in RISC-V architecture*/
  static constexpr ByteOrder RISCV_BYTEORDER = ByteOrder::kLittleEndian;
  /** bits per byte in RISC-V architecture*/
  static constexpr std::size_t RISCV_BITS_PER_BYTE = 8;

  /*!
   * Holds information about this instruction (e.g. mnemonic)
   */
  InstructionInformation _instructionInformation;
  /*!
   * Indicates if this instruction is a register-immediate instruction.
   * If false this instruction is a register-register instruction.
   * This value is used for validation of operands
   */
  bool _isImmediate;
};

/*!
 * Represents a RISC-V "add/addi" instruction. For more information see RISC-V specification
 * \tparam integer type that can hold exactly the range of values that this operation should operate
 */
template <typename SizeType>
class AddInstructionNode : public IntegerInstructionNode<SizeType> {
 public:
  AddInstructionNode(InstructionInformation info, bool immediateInstruction)
      : IntegerInstructionNode<SizeType>(info, immediateInstruction) {}

  MemoryValue assemble() override { return MemoryValue(); }

  /*!
   * Adds the two operands
   * \param op1 summand
   * \param op2 summand
   * \return op1 + op2
   */
  SizeType performIntegerOperation(SizeType op1, SizeType op2) override {
    return op1 + op2;
  }
};
}

#endif /* ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTIONS_HPP_ */
