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

#include "arch/common/abstract-syntax-tree-node.hpp"
#include "arch/common/instruction-information.hpp"
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
  InstructionNode(InstructionInformation& information)
  : super(Type::INSTRUCTION), _information(information) {
  }

  virtual ~InstructionNode() = default;

  /** \copydoc AbstractSyntaxTreeNode::assemble() */
  MemoryValue assemble() const override;

  /** \copydoc AbstractSyntaxTreeNode::getIdentifier() */
  const std::string& getIdentifier() const override;

 protected:
  /** Byte order used in RISC-V architecture. */
  static const Endianness RISCV_ENDIANNESS = Endianness::LITTLE;
  /** Bits per byte in RISC-V architecture. */
  static const std::size_t RISCV_BITS_PER_BYTE = 8;

  /**
   * Checks if this node has 'amount' children of type 'type', starting at
   * index
   * 'startIndex'.
   *
   * \param startIndex The index to start checking for registers.
   * \param amount The amount of registers required.
   * \return true if this node matches the requirements.
   */
  // Reference as super::Type
  bool _requireChildren(Type type, size_t startIndex, size_t amount) const;

  /** The information object associated with the instruction. */
  InstructionInformation _information;
};
}

#endif /* ERAGPSIM_ARCH_RISCV_INSTRUCTION_NODE_HPP */
