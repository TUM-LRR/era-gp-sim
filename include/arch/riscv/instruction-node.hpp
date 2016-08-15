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
#include "core/memory-value.hpp"
#include "arch/riscv/conversion-stub.hpp"

namespace riscv {
/** A node that represents a RISC V specific instruction */
class InstructionNode : public AbstractSyntaxTreeNode {
 public:
  /**
   * Constructs a new node that represents a RISC V specific instruction.
   */
  InstructionNode(InstructionInformation& instructionInformation)
      : AbstractSyntaxTreeNode(Type::INSTRUCTION),
        _instructionInformation(instructionInformation) {}


  /* Ensure this class is also pure virtual */
  virtual MemoryValue getValue(
      DummyMemoryAccess& memory_access) const override = 0;
  virtual bool validate() const override = 0;

  MemoryValue assemble() const override;

  /* Can be retreived using the InstructionInformation */
  const std::string& getIdentifier() const override;

  /**
   * Checks if this node has 'amount' children of type 'type', starting at
   * index
   * 'startIndex'.
   *
   * \param startIndex The index to start checking for registers.
   * \param amount The amount of registers required.
   * \return true if this node matches the requirements.
   */
  bool requireChildren(Type type, size_t startIndex, size_t amount) const;

  /** byte order used in RISC-V architecture*/
  static constexpr ByteOrder RISCV_BYTEORDER = ByteOrder::kLittleEndian;
  /** bits per byte in RISC-V architecture*/
  static constexpr std::size_t RISCV_BITS_PER_BYTE = 8;

 private:
  InstructionInformation& _instructionInformation;
};
}

#endif /* ERAGPSIM_ARCH_RISCV_INSTRUCTION_NODE_HPP */
