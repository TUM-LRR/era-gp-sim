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

///**
// * This node represents the add/addi instruction.
// *
// * See RISC V specification for details about the instruction.
// */
// class AddInstructionNode : public InstructionNode {
// public:
//  AddInstructionNode(bool immediate)
//  : InstructionNode(), _immediate(immediate) {
//  }

//  virtual MemoryValue getValue(DummyMemoryAccess &memory_access);

//  virtual bool validate();

//  virtual MemoryValue assemble() {
//    return MemoryValue{};// TODO
//  }

//  virtual std::string getIdentifier() {
//    return _immediate ? "ADDI" : "ADD";
//  }

// private:
//  bool _immediate;
//};

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

template <typename SizeType>
class IntegerInstructionNode : public InstructionNode {
 public:
  IntegerInstructionNode(InstructionInformation info, bool immediateInstruction)
      : _instructionInformation(info), _isImmediate(immediateInstruction) {}

  IntegerInstructionNode(IntegerInstructionNode& copy) = default;
  IntegerInstructionNode(IntegerInstructionNode&& move) = default;
  ~IntegerInstructionNode() = default;

  MemoryValue getValue(DummyMemoryAccess& memory_access) override {
    assert(validate());
    // Get the destination register
    std::string destination = _children.at(0)->getIdentifier();

    // Evaluate the operands
    MemoryValue memoryV1 = _children.at(1)->getValue(memory_access);
    MemoryValue memoryV2 = _children.at(2)->getValue(memory_access);

    SizeType operand1 = convert<SizeType>(memoryV1, ByteOrder::kBigEndian);
    SizeType operand2 = convert<SizeType>(memoryV2, ByteOrder::kBigEndian);

    SizeType result = performIntegerOperation(operand1, operand2);

    MemoryValue resultValue = convert(result, 8, ByteOrder::kBigEndian);
    memory_access.setRegisterValue(destination, resultValue);
    return MemoryValue();
  }

  std::string getIdentifier() override {
    assert(_instructionInformation.isValid() &&
           _instructionInformation.hasMnemonic());
    return _instructionInformation.getMnemonic();
  }

  bool validate() override {
    return validateIntegerInstruction(*this, _isImmediate);
  }

  virtual SizeType performIntegerOperation(SizeType op1, SizeType op2) = 0;

 private:
  InstructionInformation _instructionInformation;
  bool _isImmediate;
};

template <typename SizeType>
class AddInstructionNode : public IntegerInstructionNode<SizeType> {
 public:
  AddInstructionNode(InstructionInformation info, bool immediateInstruction)
      : IntegerInstructionNode<SizeType>(info, immediateInstruction) {}

  MemoryValue assemble() override { return MemoryValue(); }

  SizeType performIntegerOperation(SizeType op1, SizeType op2) override {
    return op1 + op2;
  }
};
}

#endif /* ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTIONS_HPP_ */
