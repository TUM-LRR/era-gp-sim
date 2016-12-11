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
* along with this program. If not, see <http://www.gnu.org/licenses/>.*/
#ifndef ERAGPSIM_ARCH_COMMON_ABSTRACT_INSTRUCTION_NODE_HPP
#define ERAGPSIM_ARCH_COMMON_ABSTRACT_INSTRUCTION_NODE_HPP

#include "arch/common/abstract-syntax-tree-node.hpp"
#include "arch/common/instruction-information.hpp"
#include "common/translateable.hpp"

class AbstractInstructionNode : public AbstractSyntaxTreeNode {
 public:
  /**
   * Constructs a new node that represents a instruction.
   *
   * \param The information object associated with the instruction.
   */
  AbstractInstructionNode(const InstructionInformation& information);

  virtual ~AbstractInstructionNode() = default;

  // to make this class abstract
  /** \copydoc AbstractSyntaxTreeNode::getValue() */
  virtual MemoryValue getValue(MemoryAccess& memoryAccess) const override = 0;

  /** \copydoc AbstractSyntaxTreeNode::getIdentifier() */
  const std::string& getIdentifier() const override;

  /**
   * Returns a Translateable text containing any help text/instruction
   * documentation about this instruction.
   * Note that this member function should be independent from any children as
   * it may be called for the user
   * to display information how to define a valid instruction of this kind.
   * \return A Translateable text containing information dedicated to the user
   * about this instruction
   */
  virtual const Translateable& getInstructionDocumentation() const = 0;

 protected:
  /**
   * Returns the InstructionInformation of this Instruction
   */
  const InstructionInformation& getInstructionInformation() const;

 private:
  /** The information object associated with the instruction. */
  InstructionInformation _information;
};

#endif// ERAGPSIM_ARCH_COMMON_ABSTRACT_INSTRUCTION_NODE_HPP
