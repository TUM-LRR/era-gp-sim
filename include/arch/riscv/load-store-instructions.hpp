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

#ifndef ERAGPSIM_ARCH_RISCV_LOAD_STORE_INSTRUCTIONS_HPP
#define ERAGPSIM_ARCH_RISCV_LOAD_STORE_INSTRUCTIONS_HPP

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
template <typename SignedType, typename UnsignedType>
class LoadInstructionNode : public InstructionNode {
 public:
  /* The different types of a load instruction. See RISC V specification
     for reference.*/
  enum struct Type {
    DOUBLE_WORD,         // LD (Only in RVI64)
    WORD,                // LW
    WORD_UNSIGNED,       // LWU (Only in RVI64)
    HALF_WORD,           // LH
    HALF_WORD_UNSIGNED,  // LHU
    BYTE,                // LB
    BYTE_UNSIGNED        // LBU
  };

  LoadInstructionNode(InstructionInformation& instructionInformation, Type type)
      : InstructionNode(instructionInformation), _type(type) {}

  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    assert(validate().isSuccess());

    const std::string& dest = _children.at(0)->getIdentifier();
    MemoryValue base = _children.at(1)->getValue(memoryAccess);
    MemoryValue offset = _children.at(2)->getValue(memoryAccess);

    // The base (that comes from a register) has to be converted using an
    // unsigned integer, to be able to address the whole address space
    UnsignedType baseConverted = riscv::convert<UnsignedType>(base);
    // The offset (that comes from the immediate value) has to be converted
    // using an unsigned integer, to be able to have negative offsets
    SignedType offsetConverted = riscv::convert<SignedType>(offset);

    std::size_t effectiveAddress = baseConverted + offsetConverted;

    switch (_type) {
      case Type::DOUBLE_WORD:
        performSignedLoad(memoryAccess, effectiveAddress, 8, dest);
        break;
      case Type::WORD:
        performSignedLoad(memoryAccess, effectiveAddress, 4, dest);
        break;
      case Type::WORD_UNSIGNED:
        performUnsignedLoad(memoryAccess, effectiveAddress, 4, dest);
        break;
      case Type::HALF_WORD:
        performSignedLoad(memoryAccess, effectiveAddress, 2, dest);
        break;
      case Type::HALF_WORD_UNSIGNED:
        performUnsignedLoad(memoryAccess, effectiveAddress, 2, dest);
        break;
      case Type::BYTE:
        performSignedLoad(memoryAccess, effectiveAddress, 1, dest);
        break;
      case Type::BYTE_UNSIGNED:
        performUnsignedLoad(memoryAccess, effectiveAddress, 1, dest);
        break;
      default:
        assert(false);
        break;
    }
    return MemoryValue{};
  }

  const ValidationResult validate() const override {
    if (!_requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 2) ||
        !_requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 2, 1)) {
      return ValidationResult::fail(QT_TRANSLATE_NOOP(
          "Syntax-Tree-Validation",
          "Load instructions must have 2 registers and 1 immediate"));
    }

    ValidationResult resultAll = _validateChildren();
    if (!resultAll.isSuccess()) {
      return resultAll;
    }

    // Check if the immediate value is representable by 12 bits
    // We can use an empty stub here, because immediate values don't need to
    // access the memory
    MemoryAccess stub;
    MemoryValue value = _children.at(2)->getValue(stub);

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
  /**
   * Internal method to perform an unsigned load operation
   *
   * \param memoryAccess Memory access reference
   * \param address The memory address to load from
   * \param byteAmount The amount of bytes to load
   * \param destination The destination register identifier, in which to store
   *        the loaded value
   */
  void performUnsignedLoad(MemoryAccess& memoryAccess, std::size_t address,
                           std::size_t byteAmount,
                           std::string destination) const {
    MemoryValue result = memoryAccess.getMemoryValueAt(address, byteAmount);

    // Check if zero-expansion is needed. This is the case, if the amount of
    // bits loaded from memory is not equal to the amount of bits, a register
    // can hold.
    if (result.getSize() != sizeof(UnsignedType) * riscv::BITS_PER_BYTE) {
      // Do the zero-expansion by converting to an int and converting back
      // to a memory value.
      // TODO This can be made faster, by copying the bits into a new memory
      // value of proper size. This can't be done yet, because the interface
      // of the memory value is not quite clear in that sense.
      UnsignedType converted = riscv::convert<UnsignedType>(result);
      result = riscv::convert<UnsignedType>(converted);
    }
    memoryAccess.setRegisterValue(destination, result);
  }

  /**
   * Internal method to perform a signed load operation
   *
   * \param memoryAccess Memory access reference
   * \param address The memory address to load from
   * \param byteAmount The amount of bytes to load
   * \param destination The destination register identifier, in which to store
   *        the loaded value
   */
  void performSignedLoad(MemoryAccess& memoryAccess, std::size_t address,
                         std::size_t byteAmount,
                         std::string destination) const {
    MemoryValue result = memoryAccess.getMemoryValueAt(address, byteAmount);
    // Check if sign-expansion is needed. This is the case, if the amount of
    // bits loaded from memory is not equal to the amount of bits, a register
    // can hold.
    if (result.getSize() != sizeof(SignedType) * riscv::BITS_PER_BYTE) {
      // Do a sign-expansion by converting the memory value to an integer
      // and back to a memory value.
      SignedType converted = riscv::convert<SignedType>(result);
      result = riscv::convert<SignedType>(converted);
    }
    memoryAccess.setRegisterValue(destination, result);
  }

  Type _type;
};

/**
 * \brief The StoreInstructionNode class
 *
 * Represents a store instruction.
 */
template <typename SignedType, typename UnsignedType>
class StoreInstructionNode : public InstructionNode {
 public:
  /* The different types of a store instruction. See RISC V specification
     for reference. */
  enum struct Type {
    DOUBLE_WORD,  // SD (Only in RVI64)
    WORD,         // SW
    HALF_WORD,    // SH
    BYTE          // SB
  };

  StoreInstructionNode(InstructionInformation& instructionInformation,
                       Type type)
      : InstructionNode(instructionInformation), _type(type) {}

  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    assert(validate().isSuccess());

    MemoryValue base = _children.at(0)->getValue(memoryAccess);
    const std::string& src = _children.at(1)->getIdentifier();
    MemoryValue offset = _children.at(2)->getValue(memoryAccess);

    // Unsigned to be able to address the whole address space
    UnsignedType baseConverted = riscv::convert<UnsignedType>(base);
    // Signed to be able to have negative offsets
    SignedType offsetConverted = riscv::convert<SignedType>(base);

    std::size_t effectiveAddress = baseConverted + offsetConverted;
    std::size_t byteAmount;
    switch (_type) {
      case Type::DOUBLE_WORD:
        byteAmount = 8;
        break;
      case Type::WORD:
        byteAmount = 4;
        break;
      case Type::HALF_WORD:
        byteAmount = 2;
        break;
      case Type::BYTE:
        byteAmount = 1;
        break;
      default:
        assert(false);
        break;
    }

    MemoryValue registerValue = memoryAccess.getRegisterValue(src);
    MemoryValue resultValue{byteAmount, riscv::BITS_PER_BYTE};
    for (size_t i = 0; i < byteAmount * riscv::BITS_PER_BYTE; ++i) {
      resultValue.put(resultValue.getSize() - 1 - i,
                      registerValue.get(registerValue.getSize() - 1 - i));
    }
    memoryAccess.setMemoryValueAt(effectiveAddress, resultValue);
    return MemoryValue{};
  }

  const ValidationResult validate() const override {
    if (!_requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 2) ||
        !_requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 2, 1)) {
      return ValidationResult::fail(QT_TRANSLATE_NOOP(
          "Syntax-Tree-Validation",
          "Store instructions must have 2 registers and 1 immediate"));
    }

    ValidationResult resultAll = _validateChildren();
    if (!resultAll.isSuccess()) {
      return resultAll;
    }

    // Check if the immediate value is representable by 12 bits
    // We can use an empty stub here, because immediate values don't need to
    // access the memory
    MemoryAccess stub;
    MemoryValue value = _children.at(2)->getValue(stub);

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

#endif /* ERAGPSIM_ARCH_RISCV_LOAD_STORE_INSTRUCTIONS_HPP */
