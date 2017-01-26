/*
* ERASIM Assembler Interpreter
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

#ifndef ERASIM_ARCH_RISCV_SPECIAL_INTEGER_INSTRUCTIONS_HPP
#define ERASIM_ARCH_RISCV_SPECIAL_INTEGER_INSTRUCTIONS_HPP

#include <QtGlobal>
#include <cstdint>
#include <limits>

#include <iostream>

#include "arch/common/validation-result.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "common/assert.hpp"

namespace riscv {

/**
 * \brief A superclass for LUI and AUIPC
 *
 * A superclass for special integer instructions, that require 2 operands
 * (register and 20 bit immediate).
 * This class mostly exist, to reduce redundancy.
 */
class LuiAuipcValidationNode : public InstructionNode {
 public:
  explicit LuiAuipcValidationNode(const InstructionInformation &information);

  MemoryValue getValue(MemoryAccess &memoryAccess) const override = 0;

  ValidationResult validate(MemoryAccess &memoryAccess) const override {
    if (_children.size() != 2) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "This instruction must have exactly %1 operands"),
          std::to_string(2));
    }

    ValidationResult resultAll = _validateChildren(memoryAccess);
    if (!resultAll.isSuccess()) {
      return resultAll;
    }

    if (!_requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 1) ||
        !_requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 1, 1)) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "This instruction must have 1 register and 1 "
                            "immediate as operands"));
    }

    // Get the value of the immediate value and check, if it is representable by
    // 20 bits (unsigned representation)
    MemoryValue value = _children.at(1)->getValue(memoryAccess);
    if (Utility::occupiesMoreBitsThan(value, 20, false)) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "The immediate value of this instruction must "
                            "be representable by %1 bits"),
          std::to_string(20));
    }
    return ValidationResult::success();
  }

 protected:
  /* The LUI and AUIPC instructions performs its operation ALWAYS on 32 bits
   * (See RISC-V specification for reference). So a custom datatype is
   * defined to use in the getValue() method. */
  using InternalUnsigned = std::uint32_t;

  /**
   * The common portion of both AUIPC and LUI that retrieves the immediate.
   *
   * Some preprocessing and possible sign-extension is done.
   *
   * \param  memoryAccess Access to the memory.
   */
  template <typename UnsignedWord>
  UnsignedWord _getImmediate(MemoryAccess &memoryAccess) const {
    static const auto internalWidth =
        std::numeric_limits<InternalUnsigned>::digits;

    auto immediate = _children[1]->getValue(memoryAccess);

    // Convert the offset to an internal integer representation
    auto immediateConverted = riscv::convert<InternalUnsigned>(immediate);
    // Perform a bitwise shift, filling the lower 12 bits with zeros
    immediateConverted <<= 12;

    // On non-RV32 architectures, the result of the last
    // operation (that has always a width of 32 bits) must be sign extended,
    // to the architectures word size (e.g. 64 bit)
    auto result = static_cast<UnsignedWord>(immediateConverted);

    // Check if sign-expansion is needed
    auto sign = Utility::mostSignificantBit(immediateConverted);
    if (sign && sizeof(UnsignedWord) > sizeof(InternalUnsigned)) {
      return result | (~std::uint64_t{0} << internalWidth);
    }

    return result;
  }
};

/**
 * \brief The LUI instruction node
 * This class represents the LUI instruction (Load Upper Immediate).
 */
template <typename UnsignedWord>
class LuiInstructionNode : public LuiAuipcValidationNode {
 public:
  using super = LuiAuipcValidationNode;

  LuiInstructionNode(const InstructionInformation &information)
  : super(information) {
  }

  MemoryValue getValue(MemoryAccess &memoryAccess) const override {
    assert::that(validate(memoryAccess).isSuccess());

    auto immediate = _getImmediate<UnsignedWord>(memoryAccess);

    // Convert back into a memory value and store it in the register
    auto destination = _children[0]->getIdentifier();
    memoryAccess.putRegisterValue(destination, riscv::convert(immediate));

    return _incrementProgramCounter<UnsignedWord>(memoryAccess);
  }
};

/**
 * \brief The AUIPC instruction node
 * This class represents the AUIPC instruction (Add Upper Immediate to PC).
 */
template <typename UnsignedWord>
class AuipcInstructionNode : public LuiAuipcValidationNode {
 public:
  using super = LuiAuipcValidationNode;

  AuipcInstructionNode(const InstructionInformation &information)
  : super(information) {
  }

  MemoryValue getValue(MemoryAccess &memoryAccess) const override {
    assert::that(validate(memoryAccess).isSuccess());

    auto programCounter = memoryAccess.getRegisterValue("pc").get();
    auto programCounterConverted = riscv::convert<UnsignedWord>(programCounter);

    auto immediate = _getImmediate<UnsignedWord>(memoryAccess);

    // Add the offset to the program counter
    programCounterConverted += immediate;
    // Convert the result back into a MemoryValue
    auto result = riscv::convert(programCounterConverted);

    auto destination = _children[0]->getIdentifier();
    memoryAccess.putRegisterValue(destination, result);

    return _incrementProgramCounter<UnsignedWord>(memoryAccess);
  }
};

}// Namespace riscv

#endif /* ERASIM_ARCH_RISCV_SPECIAL_INTEGER_INSTRUCTIONS_HPP */
