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

#include "arch/common/ast.hpp"

/** A node that represents a RISC V specific instruction */
class InstructionNode : public AbstractSyntaxTreeNode {
public:
  /**
   * Constructs a new node that represents a RISC V specific instruction.
   *
   * \param instruction The name of this instruction.
   */
  RegisterNode(std::string instruction)
      : AbstractSyntaxTreeNode(NodeType::IMMEDIATE), _instruction(instruction) {
  }

  /* Ensure this class is also pure virtual */
  virtual MemoryValue getValue(DummyMemoryAccess memory_access) = 0;
  virtual bool validate() = 0;
  virtual MemoryValue assemble() = 0;
  virtual std::string getIdentifier() = 0;

  /**
   * Checks if this node has 3 register nodes as children.
   *
   * \return true if this node matches the requirements.
   */
  bool requireThreeRegisters();

  /**
   * Checks if this node has 2 register and 1 immediate node as children (in
   * that order).
   *
   * \return true if this node matches the requirements.
   */
  bool requireTwoRegistersAndImmediate();

private:
  std::string _instruction;
};

#endif // ERAGPSIM_ARCH_RISCV_INSTRUCTION_NODE_HPP
