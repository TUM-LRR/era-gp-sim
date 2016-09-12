/* C++ Assembler Interpreter
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef ERAGPSIM_ARCH_RISCV_INSTRUCTION_NODE_HPP
#define ERAGPSIM_ARCH_RISCV_INSTRUCTION_NODE_HPP

#include <initializer_list>
#include <string>

#include "arch/common/abstract-syntax-tree-node.hpp"
#include "arch/common/instruction-information.hpp"
#include "core/conversions.hpp"
#include "core/memory-value.hpp"

namespace riscv {
/** A node that represents a RISC V specific instruction */
class InstructionNode : public AbstractSyntaxTreeNode {
 public:
  using super = AbstractSyntaxTreeNode;

  /** Byte order used in RISC-V architecture. */
  static const Endianness RISCV_ENDIANNESS = Endianness::LITTLE;
  /** Bits per byte in RISC-V architecture. */
  static const std::size_t RISCV_BITS_PER_BYTE = 8;

  /**
   * Constructs a new node that represents a RISC V specific instruction.
   *
   * \param The information object associated with the instruction.
   */
  InstructionNode(const InstructionInformation& information);

  virtual ~InstructionNode() = default;

  /** \copydoc AbstractSyntaxTreeNode::assemble() */
  MemoryValue assemble() const override;

  /** \copydoc AbstractSyntaxTreeNode::getIdentifier() */
  const std::string& getIdentifier() const override;

 protected:
  using TypeList = std::initializer_list<super::Type>;

  /**
   * Utility function to convert a memory value for RISC-V.
   *
   * The RISCV_ENDIANNESS is used.
   *
   * \tparam T The type to convert to.
   * \param memory The memory value to convert.
   *
   * \return The converted value.
   */
  template <typename T>
  static T _convert(const MemoryValue& memory) {
    return convert<T>(memory, RISCV_ENDIANNESS);
  }

  /**
   * Utility function to convert a value into a memory value for RISC-V.
   *
   * The RISCV_ENDIANNESS is used.
   *
   * \tparam T The type to convert to.
   * \param value The value to convert.
   *
   * \return The converted memory value.
   */
  template <typename T>
  static MemoryValue _convert(const T& value) {
    return convert(value, RISCV_BITS_PER_BYTE, RISCV_ENDIANNESS);
  }

  /**
   * Utility function to retrieve a register value for RISC-V.
   *
   * \tparam T The type to convert to.
   * \param memoryAccess The memory access object.
   * \param name The name of the register to access.
   *
   * \return The value for the child.
   */
  template <typename T>
  static T
  _loadRegister(MemoryAccess& memoryAccess, const std::string& registerName) {
    auto memory = memoryAccess.getRegisterValue(registerName);
    return _convert<T>(memory);
  }

  /**
   * Utility function to store a register value for RISC-V.
   *
   * \tparam T The type to convert to.
   * \param memoryAccess The memory access object.
   * \param name The name of the register to access.
   * \param value The value to convert and store.
   */
  template <typename T>
  static void _storeRegister(MemoryAccess& memoryAccess,
                             const std::string& registerName,
                             const T& value) {
    auto memory = _convert(value);
    memoryAccess.setRegisterValue(registerName, memory);
  }

  /**
   * Utility function to retrieve a child node for RISC-V.
   *
   * The RISCV_ENDIANNESS is used.
   *
   * \tparam T The type to convert to.
   * \param memoryAccess The memory access object.
   * \param index The index of the child to retrieve.
   *
   * \return The value fo the child.
   */
  template <typename T>
  T _child(MemoryAccess& memoryAccess, size_t index) const {
    assert(index < _children.size());
    auto memory = _children[index]->getValue(memoryAccess);
    return _convert<T>(memory);
  }

  // TODO: Leaving it like so to avoid conflicts for now
  /**
   * Checks if this node has 'amount' children of type 'type', starting at
   * index
   * 'startIndex'.
   *
   * \param startIndex The index to start checking for registers.
   * \param amount The amount of registers required.
   *
   * \return True if the children match the given types, else false.
   */
  bool _requireChildren(Type type, size_t startIndex, size_t amount) const;


  /**
   * Checks if the node's children are of the given types.
   *
   * \param list A list of types.
   * \param startIndex An optional starting index from where to check types.
   *
   * \return True if the children match the given types, else false.
   */
  bool _compareChildTypes(TypeList list, size_t startIndex = 0) const;

  /** The information object associated with the instruction. */
  InstructionInformation _information;
};
}

#endif /* ERAGPSIM_ARCH_RISCV_INSTRUCTION_NODE_HPP */
