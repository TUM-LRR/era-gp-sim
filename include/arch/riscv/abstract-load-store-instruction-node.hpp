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

#ifndef ERASIM_ARCH_RISCV_ABSTRACT_LOAD_STORE_INSTRUCTION_NODE_HPP
#define ERASIM_ARCH_RISCV_ABSTRACT_LOAD_STORE_INSTRUCTION_NODE_HPP

#include <QtGlobal>
#include <cstddef>
#include <string>

#include "arch/riscv/instruction-node.hpp"
#include "arch/riscv/utility.hpp"
#include "common/utility.hpp"
#include "core/memory-access.hpp"
#include "core/memory-value.hpp"

namespace riscv {

/**
 * \Brief a super class for both load & store instruction nodes
 */
template <typename SignedWord, typename UnsignedWord>
class AbstractLoadStoreInstructionNode : public InstructionNode {
 public:
  using size_t = std::size_t;

  AbstractLoadStoreInstructionNode(
      const InstructionInformation& instructionInformation,
      size_t byteAmount,
      bool writesProtectedMemory)
  : InstructionNode(instructionInformation)
  , _byteAmount(byteAmount)
  , _writesProtectedMemory(writesProtectedMemory) {
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
    size_t effectiveAddress = _getEffectiveAddress(memoryAccess);

    if (effectiveAddress + _byteAmount - 1 >
        memoryAccess.getMemorySize().get()) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "The memory area you are trying to access is "
                            "out of range (area: [%1,%2])"),
          std::to_string(effectiveAddress),
          std::to_string(effectiveAddress + _byteAmount - 1));
    }

    if (_writesProtectedMemory &&
        memoryAccess.isMemoryProtectedAt(effectiveAddress, _byteAmount).get()) {
      return ValidationResult::fail(
          QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                            "The memory area you are trying to access is "
                            "protected (area: [%1,%2])"),
          std::to_string(effectiveAddress),
          std::to_string(effectiveAddress + _byteAmount - 1));
    }

    return ValidationResult::success();
  }

 protected:
  /**
   * \returns the value of the child that holds the base address.
   * \parma memoryAccess The memory access to retrive the register value.
   */
  MemoryValue _getBase(MemoryAccess& memoryAccess) const {
    return super::_children.at(1)->getValue(memoryAccess);
  }

  /**
   * \returns the value of the child that holds the offset, added to the base
   * address.
   * \param memoryAccess The memory access to retrive the register value.
   */
  MemoryValue _getOffset(MemoryAccess& memoryAccess) const {
    return super::_children.at(2)->getValue(memoryAccess);
  }

  /**
   * A helper method to calculate the effective address of the load/store
   * instruction.
   *
   * \param base The base address
   * \param offset The offset, added to the base address
   * \return the effective address
   */
  size_t _getEffectiveAddress(MemoryAccess& memoryAccess) const {
    // The base (that comes from a register) has to be converted using an
    // unsigned integer, to be able to address the whole address space
    UnsignedWord baseConverted =
        riscv::convert<UnsignedWord>(_getBase(memoryAccess));
    // The offset (that comes from the immediate value) has to be converted
    // using an unsigned integer, to be able to have negative offsets
    SignedWord offsetConverted =
        riscv::convert<SignedWord>(_getOffset(memoryAccess));
    return baseConverted + offsetConverted;
  }

  /**
   * The amount of bytes, this load/store operation operates on.
   */
  size_t _byteAmount;

  /**
   * Whether this node writes into protected memory.
   */
  bool _writesProtectedMemory;
};
}  // namespace riscv

#endif /* ERASIM_ARCH_RISCV_ABSTRACT_LOAD_STORE_INSTRUCTION_NODE_HPP */
