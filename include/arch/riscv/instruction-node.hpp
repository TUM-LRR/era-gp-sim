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
  virtual MemoryValue getValue(DummyMemoryAccess& memory_access) const = 0;
  virtual bool validate() const = 0;
  virtual MemoryValue assemble() const override;

  /* Can be retreived using the InstructionInformation */
  virtual const std::string& getIdentifier() const;

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

  /**
   * Calls validate() on all children
   * \return
   */
  bool validateAllChildren()const;

  /*!
   * Returns how many children this instruction node has.
   * \return
   */
  size_t getChildrenCount() const;

  /*!
   * returns the children at index
   * \param index index boundaries are asserted
   * \return
   */
  AbstractSyntaxTreeNode& getChild(size_t index)const;

 private:
  InstructionInformation& _instructionInformation;
};
}

#endif /* ERAGPSIM_ARCH_RISCV_INSTRUCTION_NODE_HPP */
