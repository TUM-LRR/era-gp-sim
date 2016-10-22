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

#include <QtGlobal>
#include <cassert>
#include <string>

#include "arch/common/validation-result.hpp"
#include "arch/riscv/instruction-node.hpp"

namespace riscv {

/**
 * \Brief a super class for both load & store instruction nodes
 */
template <typename SignedWord, typename UnsignedWord>
class LoadStoreInstructionNode : public InstructionNode {
 public:
  LoadStoreInstructionNode(const InstructionInformation& instructionInformation,
                           std::size_t byteAmount)
  : InstructionNode(instructionInformation), _byteAmount(byteAmount) {
  }

  /* Ensure this class is pure virtual */
  virtual MemoryValue getValue(MemoryAccess& memoryAccess) const override = 0;

  ValidationResult validate(MemoryAccess& memoryAccess) const override {
    if (_children.size() != 3) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "This instruction must have exactly %1 operands"),
          std::to_string(3));
    }

    if (!_requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 2) ||
        !_requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 2, 1)) {
      return ValidationResult::fail(QT_TRANSLATE_NOOP(
          "Syntax-Tree-Validation",
          "This instruction must have 2 registers and 1 immediate"));
    }

    ValidationResult resultAll = _validateChildren(memoryAccess);
    if (!resultAll.isSuccess()) {
      return resultAll;
    }

    // Check if the immediate value is representable by 12 bits
    // We can use an empty stub here, because immediate values don't need to
    // access the memory
    MemoryValue value = _children.at(2)->getValue(memoryAccess);
    if (Utility::occupiesMoreBitsThan(value, 12)) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "The immediate value of this instruction must "
                            "be representable by %1 bits"),
          std::to_string(12));
    }

    return ValidationResult::success();
  }

  ValidationResult validateRuntime(MemoryAccess& memoryAccess) const override {
    std::size_t effectiveAddress = getEffectiveAddress(memoryAccess);
    if (effectiveAddress + _byteAmount - 1 >
        memoryAccess.getMemorySize().get()) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "The memory address %1 is out of range"),
          std::to_string(effectiveAddress));
    }

    return ValidationResult::success();
  }

 protected:
  /**
   * Returns the value of the child that holds the base address.
   */
  virtual MemoryValue getBase(MemoryAccess& memoryAccess) const = 0;
  /**
   * Returns the value of the child that holds the offset, added to the base
   * address.
   */
  virtual MemoryValue getOffset(MemoryAccess& memoryAccess) const = 0;

  /**
   * A helper method to calculate the effective address of the load/store
   * instruction.
   *
   * \param base The base address
   * \param offset The offset, added to the base address
   * \return the effective address
   */
  std::size_t getEffectiveAddress(MemoryAccess& memoryAccess) const {
    // The base (that comes from a register) has to be converted using an
    // unsigned integer, to be able to address the whole address space
    UnsignedWord baseConverted =
        riscv::convert<UnsignedWord>(getBase(memoryAccess));
    // The offset (that comes from the immediate value) has to be converted
    // using an unsigned integer, to be able to have negative offsets
    SignedWord offsetConverted =
        riscv::convert<SignedWord>(getOffset(memoryAccess));
    return baseConverted + offsetConverted;
  }

  /**
   * The amount of bytes, this load/store operation operates on.
   */
  std::size_t _byteAmount;
};

/**
 * \brief Represents a load instruction.
 */
template <typename SignedWord, typename UnsignedWord>
class LoadInstructionNode
    : public LoadStoreInstructionNode<SignedWord, UnsignedWord> {
 public:
  using super = LoadStoreInstructionNode<SignedWord, UnsignedWord>;

  /* The different types of a load instruction. See RISC V specification
     for reference.*/
  enum struct Type {
    DOUBLE_WORD,       // LD (Only in RVI64)
    WORD,              // LW
    WORD_UNSIGNED,     // LWU (Only in RVI64)
    HALF_WORD,         // LH
    HALF_WORD_UNSIGNED,// LHU
    BYTE,              // LB
    BYTE_UNSIGNED      // LBU
  };

  LoadInstructionNode(const InstructionInformation& instructionInformation,
                      Type type)
  : super(instructionInformation, getByteAmount(type)), _type(type) {
  }

  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    assert(super::validate(memoryAccess));

    const std::string& dest = super::_children.at(0)->getIdentifier();
    std::size_t effectiveAddress = super::getEffectiveAddress(memoryAccess);

    if (isSigned(_type)) {
      performSignedLoad(
          memoryAccess, effectiveAddress, super::_byteAmount, dest);
    } else {
      performUnsignedLoad(
          memoryAccess, effectiveAddress, super::_byteAmount, dest);
    }
    return super::template _incrementProgramCounter<UnsignedWord>(memoryAccess);
  }

 private:
  /**
   * This function returns the amount of bytes, a load instruction
   * loads from memory, depending on its type.
   */
  static std::size_t getByteAmount(Type type) {
    switch (type) {
      case Type::DOUBLE_WORD: return 8;
      case Type::WORD:
      case Type::WORD_UNSIGNED: return 4;
      case Type::HALF_WORD:
      case Type::HALF_WORD_UNSIGNED: return 2;
      case Type::BYTE:
      case Type::BYTE_UNSIGNED: return 1;
      default: assert::that(false); return 0;
    }
  }

  /**
   * Returns true, if the load type is a signed load.
   */
  static bool isSigned(Type type) {
    switch (type) {
      case Type::DOUBLE_WORD:
      case Type::WORD:
      case Type::HALF_WORD:
      case Type::BYTE: return true;
      default: return false;
    }
  }

  MemoryValue getBase(MemoryAccess& memoryAccess) const override {
    return super::_children.at(1)->getValue(memoryAccess);
  }

  MemoryValue getOffset(MemoryAccess& memoryAccess) const override {
    return super::_children.at(2)->getValue(memoryAccess);
  }

  /**
   * Internal method to perform an unsigned load operation
   *
   * \param memoryAccess Memory access reference
   * \param address The memory address to load from
   * \param byteAmount The amount of bytes to load
   * \param destination The destination register identifier, in which to store
   *        the loaded value
   */
  void performUnsignedLoad(MemoryAccess& memoryAccess,
                           std::size_t address,
                           std::size_t byteAmount,
                           const std::string& destination) const {
    MemoryValue result =
        memoryAccess.getMemoryValueAt(address, byteAmount).get();

    // Check if zero-expansion is needed. This is the case, if the amount of
    // bits loaded from memory is not equal to the amount of bits, a register
    // can hold.
    if (result.getSize() != sizeof(UnsignedWord) * riscv::BITS_PER_BYTE) {
      // Do sign expansion by creating a new memory value of proper size
      // and setting the lower bits to the bits of the old memory value.
      MemoryValue tmp =
          MemoryValue{sizeof(UnsignedWord) * riscv::BITS_PER_BYTE};
      for (std::size_t i = 0; i < result.getSize(); ++i) {
        tmp.put(i, result.get(i));
      }
      result = tmp;
    }
    memoryAccess.putRegisterValue(destination, result);
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
  void performSignedLoad(MemoryAccess& memoryAccess,
                         std::size_t address,
                         std::size_t byteAmount,
                         const std::string& destination) const {
    MemoryValue result =
        memoryAccess.getMemoryValueAt(address, byteAmount).get();
    // Check if sign-expansion is needed. This is the case, if the amount of
    // bits loaded from memory is not equal to the amount of bits, a register
    // can hold.
    if (result.getSize() != sizeof(SignedWord) * riscv::BITS_PER_BYTE) {
      // Do sign expansion by creating a new memory value, copying the old
      // memory value in the lower region, and setting the upper region to the
      // sign bit of the old memory value.
      bool isSignBitSet = result.get(result.getSize() - 1);
      MemoryValue tmp = MemoryValue{sizeof(SignedWord) * riscv::BITS_PER_BYTE};
      for (std::size_t i = 0; i < tmp.getSize(); ++i) {
        if (i < result.getSize()) {
          tmp.put(i, result.get(i));
        } else {
          tmp.put(i, isSignBitSet);
        }
      }
      result = tmp;
    }
    memoryAccess.putRegisterValue(destination, result);
  }


  Type _type;
};

/**
 * \brief Represents a store instruction.
 */
template <typename SignedWord, typename UnsignedWord>
class StoreInstructionNode
    : public LoadStoreInstructionNode<SignedWord, UnsignedWord> {
 public:
  using super = LoadStoreInstructionNode<SignedWord, UnsignedWord>;

  /* The different types of a store instruction. See RISC V specification
     for reference. */
  enum struct Type {
    DOUBLE_WORD,// SD (Only in RVI64)
    WORD,       // SW
    HALF_WORD,  // SH
    BYTE        // SB
  };

  StoreInstructionNode(const InstructionInformation& instructionInformation,
                       Type type)
  : super(instructionInformation, getByteAmount(type)), _type(type) {
  }

  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    assert(super::validate(memoryAccess));

    const std::string& src = super::_children.at(1)->getIdentifier();
    std::size_t effectiveAddress = super::getEffectiveAddress(memoryAccess);

    MemoryValue registerValue = memoryAccess.getRegisterValue(src).get();
    MemoryValue resultValue{super::_byteAmount * riscv::BITS_PER_BYTE};
    for (size_t i = 0; i < super::_byteAmount * riscv::BITS_PER_BYTE; ++i) {
      resultValue.put(i, registerValue.get(i));
    }
    memoryAccess.putMemoryValueAt(effectiveAddress, resultValue);
    return super::template _incrementProgramCounter<UnsignedWord>(memoryAccess);
  }

 private:
  /**
    * This function returns the amount of bytes, a store instruction
    * stores into memory, depending on its type.
    */
  static std::size_t getByteAmount(Type type) {
    switch (type) {
      case Type::DOUBLE_WORD: return 8;
      case Type::WORD: return 4;
      case Type::HALF_WORD: return 2;
      case Type::BYTE: return 1;
      default: assert::that(false); return 0;
    }
  }

  MemoryValue getBase(MemoryAccess& memoryAccess) const override {
    return super::_children.at(0)->getValue(memoryAccess);
  }

  MemoryValue getOffset(MemoryAccess& memoryAccess) const override {
    return super::_children.at(2)->getValue(memoryAccess);
  }

  Type _type;
};
}

#endif /* ERAGPSIM_ARCH_RISCV_LOAD_STORE_INSTRUCTIONS_HPP_ */
