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
#include "arch/riscv/properties.hpp"
#include "arch/riscv/utility.hpp"
#include "core/conversions.hpp"
#include "core/memory-value.hpp"

namespace riscv {
/** A node that represents a RISC V specific instruction */
class InstructionNode : public AbstractSyntaxTreeNode {
 public:
  using super = AbstractSyntaxTreeNode;

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
   * Utility function to retrieve a child node for RISC-V.
   *
   * The riscv::ENDIANNESS is used.
   *
   * \tparam T The type to convert to.
   * \param memoryAccess The memory access object.
   * \param index The index of the child to retrieve.
   *
   * \return The value fo the child.
   */
  template <typename T>
  T _getChildValue(MemoryAccess& memoryAccess, size_t index) const {
    assert(index < _children.size());
    auto memory = _children[index]->getValue(memoryAccess);
    return riscv::convert<T>(memory);
  }

  /**
   * Increments the program counter by
   * `_information.getLength() /  BITS_PER_BYTE` and returns the new value.
   *
   * \tparam WordSize The unsigned word size integral of the architecture
   */
  template <typename WordSize>
  MemoryValue _incrementProgramCounter(MemoryAccess& memoryAccess) const {
    auto current = riscv::loadRegister<WordSize>(memoryAccess, "pc");
    current += (_information.getLength() / riscv::BITS_PER_BYTE);
    return riscv::convert<WordSize>(current);
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
