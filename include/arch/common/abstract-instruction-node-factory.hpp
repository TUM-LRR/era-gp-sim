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

#ifndef ERAGPSIM_ARCH_ABSTRACT_INSTRUCTION_NODE_FACTORY_HPP
#define ERAGPSIM_ARCH_ABSTRACT_INSTRUCTION_NODE_FACTORY_HPP

#include <memory>
#include <string>

class AbstractSyntaxTreeNode;

/**
 * \brief The AbstractInstructionNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type instruction
 */
class AbstractInstructionNodeFactory {
 public:
  using Node = std::unique_ptr<AbstractSyntaxTreeNode>;

  virtual ~AbstractInstructionNodeFactory() = default;

  /**
   * \brief createInstructionNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type instruction for
   * the given token. If the architecture-specific implementation cannot find a
   * instruction with the given token, a nullptr is returned
   *
   * \param token std::string, that contains the identifer of a instruction
   * (e.g. "ADD" for Add-Instruction) in upper-case
   * \return std::unique_ptr pointing to the newly created SyntaxTreeNode, or
   * nullptr if the given token cannot be mapped to a architecture-specific
   * instruction
   */
  virtual Node createInstructionNode(const std::string& mnemonic) const = 0;
};

#endif /* ERAGPSIM_ARCH_ABSTRACT_INSTRUCTION_NODE_FACTORY_HPP */
