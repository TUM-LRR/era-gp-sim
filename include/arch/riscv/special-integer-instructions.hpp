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

#ifndef ERAGPSIM_ARCH_RISCV_SPECIAL_INTEGER_INSTRUCTIONS_HPP_
#define ERAGPSIM_ARCH_RISCV_SPECIAL_INTEGER_INSTRUCTIONS_HPP_

#include <QtGlobal>
#include <cassert>
#include "arch/riscv/instruction-node.hpp"

namespace riscv {

/**
 * \brief A superclass for special integer instructions
 *
 * A superclass for special integer instructions, that require 2 operands
 * (register and 20 bit immediate).
 * This class mostly exist, to reduce redundancy.
 */
class SpecialIntegerInstructionNode : public InstructionNode {
 public:
  SpecialIntegerInstructionNode(InstructionInformation& info) : InstructionNode(info) {
  }

  MemoryValue getValue(DummyMemoryAccess& memoryAccess) const override = 0;

  const ValidationResult validate() const override {
    if (_children.size() != 2) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "This instruction must have exactly 2 operands"));
    }

    const ValidationResult resultAll = validateAllChildren();
    if (!resultAll.isSuccess()) {
      return resultAll;
    }

    if (!requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 1) ||
        !requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 1, 1)) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "This instruction must have 1 register and 1 "
                            "immediate as operands"));
    }

    // Get the value of the immediate value and check, if it is representable by
    // 20 bits
    DummyMemoryAccessStub stub;
    MemoryValue value = _children.at(1)->getValue(stub);
    for (std::size_t index = 20; index < value.getSize(); ++index) {
      if (value.get(index)) {
        return ValidationResult::fail(
            QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                              "The immediate value of this instruction "
                              "must be representable by 20 bits"));
      }
    }

    return ValidationResult::success();
  }
};

/**
 * \brief The LUI instruction node
 * This class represents the LUI instruction (Load Upper Immediate).
 */
template <typename UnsignedType>
class LuiInstructionNode : public SpecialIntegerInstructionNode {
 public:
  LuiInstructionNode(InstructionInformation& info)
  : SpecialIntegerInstructionNode(info) {
  }

  MemoryValue getValue(DummyMemoryAccess& memoryAccess) const override {
    assert(validate().isSuccess());

    const std::string& destination = _children.at(0)->getIdentifier();
    MemoryValue offset             = _children.at(1)->getValue(memoryAccess);

    // Convert to the unsigned type of the architecture
    UnsignedType offsetConverted =
        convert<UnsignedType>(offset, RISCV_ENDIANNESS);
    // Perform a bitwise shift, filling the lower 12 bits with zeros
    offsetConverted <<= 12;
    // Convert back into a memory value
    MemoryValue result = convert<UnsignedType>(
        offsetConverted, RISCV_BITS_PER_BYTE, RISCV_ENDIANNESS);

    memoryAccess.setRegisterValue(destination, result);
  }
};

/**
 * \brief The AUIPC instruction node
 * This class represents the AUIPC instruction (Add Upper Immediate to PC).
 */
template <typename UnsignedType>
class AuipcInstructionNode : public SpecialIntegerInstructionNode {
 public:
  AuipcInstructionNode(InstructionInformation& info)
  : SpecialIntegerInstructionNode(info) {
  }

  MemoryValue getValue(DummyMemoryAccess& memoryAccess) const override {
    assert(validate().isSuccess());

    const std::string& destination = _children.at(0)->getIdentifier();
    MemoryValue offset             = _children.at(1)->getValue(memoryAccess);
    MemoryValue programCounter     = memoryAccess.getRegisterValue("pc");

    // Convert to the unsigned type of the architecture
    UnsignedType offsetConverted =
        convert<UnsignedType>(offset, RISCV_ENDIANNESS);
    UnsignedType programCounterConverted =
        convert<UnsignedType>(programCounter, RISCV_ENDIANNESS);
    // Perform a bitwise shift, filling the lower 12 bits with zeros
    offsetConverted <<= 12;
    programCounterConverted += offsetConverted;
    // Convert the result back into a MemoryValue
    MemoryValue result = convert<UnsignedType>(
        programCounterConverted, RISCV_BITS_PER_BYTE, RISCV_ENDIANNESS);

    memoryAccess.setRegisterValue(destination, result);
  }
};


}// Namespace riscv

#endif /* ERAGPSIM_ARCH_RISCV_SPECIAL_INTEGER_INSTRUCTIONS_HPP_ */
