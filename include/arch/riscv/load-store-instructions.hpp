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

#ifndef ERAGPSIM_ARCH_RISCV_LOAD_STORE_INSTRUCTIONS_HPP_
#define ERAGPSIM_ARCH_RISCV_LOAD_STORE_INSTRUCTIONS_HPP_

#include "arch/riscv/instruction-node.hpp"

#include <QtGlobal>
#include <cassert>
#include <string>

namespace riscv {

/**
 * \brief The LoadInstructionNode class
 *
 * Represents a load instruction.
 */
template <typename SizeType>
class LoadInstructionNode : public InstructionNode {
 public:
  /* The different types of a load instruction. See RISC V specification
     for reference.*/
  enum struct Type {
    WORD,              // LW
    HALF_WORD,         // LH
    HALF_WORD_UNSIGNED,// LHU
    BYTE,              // LB
    BYTE_UNSIGNED      // LBU
  };

  LoadInstructionNode(InstructionInformation& instructionInformation, Type type)
  : InstructionNode(instructionInformation), _type(type) {
  }

  MemoryValue getValue(DummyMemoryAccess& memoryAccess) const override {
    assert(validate().isSuccess());

    std::string dest   = _children.at(0)->getIdentifier();
    MemoryValue base   = _children.at(1)->getValue(memory_access);
    MemoryValue offset = _children.at(2)->getValue(memory_access);

    SizeType baseConverted   = convert<SizeType>(base, RISCV_ENDIANNESS);
    SizeType offsetConverted = convert<SizeType>(offset, RISCV_ENDIANESS);

    SizeType effectiveAddress = baseConverted + offsetConverted;

    switch (_type) {
      case WORD:
        memoryAccess.setRegisterValue(
            dest, memoryAccess.getMemoryValueAt(effectiveAddress, 4));
        break;
    }
  }

  const ValidationResult validate() const override {
    if (!requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 2) ||
        !requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 2, 1)) {
      return ValidationResult::fail(QT_TRANSLATE_NOOP(
          "Syntax-Tree-Validation",
          "Load instructions must have 2 registers and 1 immediate"));
    }

    ValidationResult resultAll = validateAllChildren();
    if (!resultAll.isSuccess()) {
      return resultAll;
    }

    // Check if the immediate value is representable by 12 bits
    // We can use an empty stub here, because immediate values don't need to
    // access the memory
    DummyMemoryAccessStub stub;
    MemoryValue immediateValue = _children.at(2)->getValue(stub);

    for (std::size_t index = 12; index < value.getSize(); ++index) {
      if (value.get(index)) {
        return ValidationResult::fail(QT_TRANSLATE_NOOP(
            "Syntax-Tree-Validation",
            "The immediate value must be representable by 12 bits"));
      }
    }

    return ValidationResult::success();
  }

 private:
  Type _type;
};

/**
 * \brief The StoreInstructionNode class
 *
 * Represents a store instruction.
 */
template <typename SizeType>
class StoreInstructionNode : public InstructionNode {
 public:
  /* The different types of a store instruction. See RISC V specification
     for reference. */
  enum struct Type {
    WORD,     // SW
    HALF_WORD,// SH
    BYTE      // SB
  };

  StoreInstructionNode(InstructionInformation& instructionInformation,
                       Type type)
  : InstructionNode(instructionInformation), _type(type) {
  }

  MemoryValue getValue(DummyMemoryAccess& memory_access) const override {
    assert(validate().isSuccess());

    std::string dest   = _children.at(0)->getIdentifier();
    MemoryValue base   = _children.at(1)->getValue(memory_access);
    MemoryValue offset = _children.at(2)->getValue(memory_access);

    SizeType baseConverted = convert<SizeType>(base, RISCV_ENDIANNESS);
    // TODO Replace this with actual implementation
    // TODO Ensure the correct amount of bytes are loaded from memory
    return MemoryValue{};
  }

  const ValidationResult validate() const override {
    if (!requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 2) ||
        !requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 2, 1)) {
      return ValidationResult::fail(QT_TRANSLATE_NOOP(
          "Syntax-Tree-Validation",
          "Store instructions must have 2 registers and 1 immediate"));
    }

    ValidationResult resultAll = validateAllChildren();
    if (!resultAll.isSuccess()) {
      return resultAll;
    }

    // Check if the immediate value is representable by 12 bits
    // We can use an empty stub here, because immediate values don't need to
    // access the memory
    DummyMemoryAccessStub stub;
    MemoryValue immediateValue = _children.at(2)->getValue(stub);

    for (std::size_t index = 12; index < value.getSize(); ++index) {
      if (value.get(index)) {
        return ValidationResult::fail(QT_TRANSLATE_NOOP(
            "Syntax-Tree-Validation",
            "The immediate value must be representable by 12 bits"));
      }
    }

    return ValidationResult::success();
  }

 private:
  Type _type;
};
}

#endif /* ERAGPSIM_ARCH_RISCV_LOAD_STORE_INSTRUCTIONS_HPP_ */
