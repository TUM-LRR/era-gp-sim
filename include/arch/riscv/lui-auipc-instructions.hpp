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
#include <climits>
#include <cstdint>

#include "arch/common/validation-result.hpp"
#include "arch/riscv/instruction-node.hpp"

namespace riscv {

/**
 * \brief A superclass for LUI, AUIPC
 *
 * A superclass for special integer instructions, that require 2 operands
 * (register and 20 bit immediate).
 * This class mostly exist, to reduce redundancy.
 */
class LuiAuipcValidationNode : public InstructionNode {
 public:
  LuiAuipcValidationNode(const InstructionInformation& info)
  : InstructionNode(info) {
  }

  MemoryValue getValue(MemoryAccess& memoryAccess) const override = 0;

  ValidationResult validate(MemoryAccess& memoryAccess) const override {
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
};

/**
 * \brief The LUI instruction node
 * This class represents the LUI instruction (Load Upper Immediate).
 */
template <typename UnsignedType>
class LuiInstructionNode : public LuiAuipcValidationNode {
 public:
  LuiInstructionNode(const InstructionInformation& info)
  : LuiAuipcValidationNode(info) {
  }

  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    assert(validate(memoryAccess).isSuccess());

    const std::string& destination = _children.at(0)->getIdentifier();
    MemoryValue offset = _children.at(1)->getValue(memoryAccess);

    // Convert the offset to an internal integer representation
    InternalUnsigned offsetConverted = riscv::convert<InternalUnsigned>(offset);
    // Perform a bitwise shift, filling the lower 12 bits with zeros
    offsetConverted <<= 12;

    // On non-RV32 architectures, the result of the last
    // operation (that has always a width of 32 bits) must be sign expanded,
    // to the architectures word size (e.g. 64 bit)
    UnsignedType result = offsetConverted;
    // Check if sign-expansion is needed
    if ((sizeof(UnsignedType) - sizeof(InternalUnsigned)) > 0) {
      // Aquire the sign bit
      constexpr auto length = sizeof(InternalUnsigned) * CHAR_BIT;
      InternalUnsigned sign =
          (offsetConverted & (InternalUnsigned{1} << (length - 1)));
      // Do sign-expansion if needed
      if (sign > 0) {
        // Sign-expansion is quite easy here: All the bits above the lower
        // 32 bits are set to 1.
        constexpr auto requiredLength = sizeof(UnsignedType) * CHAR_BIT;
        result |= (length < requiredLength) ? (~UnsignedType{0} << length) : 0;
      }
    }

    // Convert back into a memory value and store it in the register
    MemoryValue resultMemoryValue = riscv::convert<UnsignedType>(result);
    memoryAccess.putRegisterValue(destination, resultMemoryValue);
    return MemoryValue{};
  }

 private:
  /* The LUI instruction performs its operation ALWAYS on 32 bits
   * (See RISC-V specification for reference). So a custom datatype is
   * defined to use in the getValue() method. */
  using InternalUnsigned = uint32_t;
};

/**
 * \brief The AUIPC instruction node
 * This class represents the AUIPC instruction (Add Upper Immediate to PC).
 */
template <typename UnsignedType>
class AuipcInstructionNode : public LuiAuipcValidationNode {
 public:
  AuipcInstructionNode(const InstructionInformation& info)
  : LuiAuipcValidationNode(info) {
  }

  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    assert(validate(memoryAccess).isSuccess());

    const std::string& destination = _children.at(0)->getIdentifier();
    MemoryValue offset = _children.at(1)->getValue(memoryAccess);
    MemoryValue programCounter = memoryAccess.getRegisterValue("pc").get();

    // Convert to the unsigned type of the architecture
    InternalUnsigned offsetConverted = riscv::convert<InternalUnsigned>(offset);
    UnsignedType programCounterConverted =
        riscv::convert<UnsignedType>(programCounter);

    // Fill lower 12 bits with 0
    offsetConverted <<= 12;

    // On non-RV32 architectures, the result of the last
    // operation (that has always a width of 32 bits) must be sign expanded,
    // to the architectures word size (e.g. 64 bit)
    UnsignedType result = offsetConverted;
    // Check if sign-expansion is needed
    if ((sizeof(UnsignedType) - sizeof(InternalUnsigned)) > 0) {
      // Aquire the sign bit
      constexpr UnsignedType length = sizeof(InternalUnsigned) * CHAR_BIT;
      InternalUnsigned sign =
          (offsetConverted & (InternalUnsigned{1} << (length - 1)));
      // Do sign-expansion if needed
      if (sign > 0) {
        // Sign-expansion is quite easy here: All the bits above the lower
        // 32 bits are set to 1.
        constexpr auto requiredLength = sizeof(UnsignedType) * CHAR_BIT;
        result |= (length < requiredLength) ? (~UnsignedType{0} << length) : 0;
      }
    }

    // Add the offset to the program counter
    programCounterConverted += result;
    // Convert the result back into a MemoryValue
    MemoryValue resultMemoryValue =
        riscv::convert<UnsignedType>(programCounterConverted);

    memoryAccess.putRegisterValue(destination, resultMemoryValue);
    return MemoryValue{};
  }

 private:
  /* The AUIPC instruction performs its operation ALWAYS on 32 bits
    * (See RISC-V specification for reference). So a custom datatype is
    * defined to use in the getValue() method. */
  using InternalUnsigned = uint32_t;
};

}// Namespace riscv

#endif /* ERAGPSIM_ARCH_RISCV_SPECIAL_INTEGER_INSTRUCTIONS_HPP_ */
