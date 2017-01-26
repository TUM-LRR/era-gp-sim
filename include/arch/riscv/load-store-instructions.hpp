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

#ifndef ERASIM_ARCH_RISCV_LOAD_STORE_INSTRUCTIONS_HPP
#define ERASIM_ARCH_RISCV_LOAD_STORE_INSTRUCTIONS_HPP

#include <QtGlobal>
#include <cstddef>
#include <string>

#include "arch/common/validation-result.hpp"
#include "arch/riscv/abstract-load-store-instruction-node.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "common/assert.hpp"

namespace riscv {
/**
 * \brief Represents a load instruction.
 */
template <typename SignedWord, typename UnsignedWord>
class LoadInstructionNode
    : public AbstractLoadStoreInstructionNode<SignedWord, UnsignedWord> {
 public:
  using super = AbstractLoadStoreInstructionNode<SignedWord, UnsignedWord>;
  using super::_byteAmount;

  /* The different types of a load instruction. See RISC V specification
     for reference.*/
  enum class Type {
    DOUBLE_WORD,         // LD (Only in RVI64)
    WORD,                // LW
    WORD_UNSIGNED,       // LWU (Only in RVI64)
    HALF_WORD,           // LH
    HALF_WORD_UNSIGNED,  // LHU
    BYTE,                // LB
    BYTE_UNSIGNED        // LBU
  };

  LoadInstructionNode(const InstructionInformation& instructionInformation,
                      Type type)
  : super(instructionInformation, getByteAmount(type), false), _type(type) {
  }

  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    assert::that(super::validate(memoryAccess));

    const auto& destination = super::_children.at(0)->getIdentifier();
    auto effectiveAddress = super::_getEffectiveAddress(memoryAccess);

    if (isSigned(_type)) {
      performSignedLoad(
          memoryAccess, effectiveAddress, _byteAmount, destination);
    } else {
      performUnsignedLoad(
          memoryAccess, effectiveAddress, _byteAmount, destination);
    }

    return super::template _incrementProgramCounter<UnsignedWord>(memoryAccess);
  }

 private:
  /**
   * This function returns the amount of bytes, a load instruction
   * loads from memory, depending on its type.
   */
  static size_t getByteAmount(Type type) {
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
                           size_t address,
                           size_t byteAmount,
                           const std::string& destination) const {
    auto result = memoryAccess.getMemoryValueAt(address, byteAmount).get();

    // Check if zero-expansion is needed. This is the case, if the amount of
    // bits loaded from memory is not equal to the amount of bits, a register
    // can hold.
    if (result.getSize() != sizeof(UnsignedWord) * riscv::BITS_PER_BYTE) {
      // Do sign expansion by creating a new memory value of proper size
      // and setting the lower bits to the bits of the old memory value.
      MemoryValue temp(sizeof(UnsignedWord) * riscv::BITS_PER_BYTE);
      for (size_t i = 0; i < result.getSize(); ++i) {
        temp.put(i, result.get(i));
      }
      result = temp;
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
                         size_t address,
                         size_t byteAmount,
                         const std::string& destination) const {
    auto result = memoryAccess.getMemoryValueAt(address, byteAmount).get();
    // Check if sign-expansion is needed. This is the case, if the amount of
    // bits loaded from memory is not equal to the amount of bits, a register
    // can hold.
    if (result.getSize() != sizeof(SignedWord) * riscv::BITS_PER_BYTE) {
      // Do sign expansion by creating a new memory value, copying the old
      // memory value in the lower region, and setting the upper region to the
      // sign bit of the old memory value.
      bool isSignBitSet = result.get(result.getSize() - 1);
      MemoryValue temp(sizeof(SignedWord) * riscv::BITS_PER_BYTE);
      for (size_t i = 0; i < temp.getSize(); ++i) {
        if (i < result.getSize()) {
          temp.put(i, result.get(i));
        } else {
          temp.put(i, isSignBitSet);
        }
      }
      result = temp;
    }
    memoryAccess.putRegisterValue(destination, result);
  }

  /** The type of load instruction this is. */
  Type _type;
};

/**
 * \brief Represents a store instruction.
 */
template <typename SignedWord, typename UnsignedWord>
class StoreInstructionNode
    : public AbstractLoadStoreInstructionNode<SignedWord, UnsignedWord> {
 public:
  using super = AbstractLoadStoreInstructionNode<SignedWord, UnsignedWord>;
  using typename super::size_t;

  /* The different types of a store instruction. See RISC V specification
     for reference. */
  enum struct Type {
    DOUBLE_WORD,  // SD (Only in RVI64)
    WORD,         // SW
    HALF_WORD,    // SH
    BYTE          // SB
  };

  StoreInstructionNode(const InstructionInformation& instructionInformation,
                       Type type)
  : super(instructionInformation, getByteAmount(type), true), _type(type) {
  }

  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    assert::that(super::validate(memoryAccess));

    const auto& sourceRegister = super::_children.at(0)->getIdentifier();
    auto effectiveAddress = super::_getEffectiveAddress(memoryAccess);

    auto registerValue = memoryAccess.getRegisterValue(sourceRegister).get();
    MemoryValue resultValue(super::_byteAmount * riscv::BITS_PER_BYTE);
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
  static size_t getByteAmount(Type type) {
    switch (type) {
      case Type::DOUBLE_WORD: return 8;
      case Type::WORD: return 4;
      case Type::HALF_WORD: return 2;
      case Type::BYTE: return 1;
      default: assert::that(false); return 0;
    }
  }

  /** The type of store instruction this is. */
  Type _type;
};
}

#endif /* ERASIM_ARCH_RISCV_LOAD_STORE_INSTRUCTIONS_HPP */
