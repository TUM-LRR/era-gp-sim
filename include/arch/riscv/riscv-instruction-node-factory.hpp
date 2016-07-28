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

#ifndef ERAGPSIM_ARCH_RISCV_INSTRUCTION_NODE_FACTORY_HPP
#define ERAGPSIM_ARCH_RISCV_INSTRUCTION_NODE_FACTORY_HPP

#include <unordered_map>

#include "arch/common/abstract-instruction-node-factory.hpp"

namespace riscv {

/**
 * \brief The InstructionNodeFactory class is a
 * concrete Implementation of AbstractInstructionNodeFactory for the RISC-V
 * architecture.
 * \see AbstractInstructionNodeFactory
 */
class InstructionNodeFactory : public AbstractInstructionNodeFactory {
 public:
  using InstructionMap = std::unordered_map<
      std::string,
      std::function<std::unique_ptr<AbstractSyntaxTreeNode>()>>;

  /**
   * \brief InstructionNodeFactory
   * Creates a Instruction Node Factory for RISC-V architecture
   */
  InstructionNodeFactory() {
    initializeInstructionMap();
  }

  /*! Default constructed copy constructor */
  InstructionNodeFactory(InstructionNodeFactory &copy) = default;

  /*! Default constructed move constructor */
  InstructionNodeFactory(InstructionNodeFactory &&move) = default;

  /**
   * \brief createInstructionNode
   * Creates and returns a RISC-V Instruction Node for a valid input token, or
   * nullptr if the token cannot be mapped to a implemented RISC-V instruction
   * \param token
   * \return std::uniqe_ptr pointing to the newly created instruction node, or
   * nullptr if the token cannot be mapped to a implemented RISC-V instruction
   */
  virtual std::unique_ptr<AbstractSyntaxTreeNode>
  createInstructionNode(const std::string &token) const override;

  ~InstructionNodeFactory() = default;

 private:
  /**
   * \brief _instructionMap
   * Table, that maps the instruction identifier (e.g. the token "ADD" for
   * Addition) to a function that creates the special instruction node (e.g.
   * AddInstructionNode)
   */
  InstructionMap _instructionMap;

  /**
   * \brief initializeInstructionMap
   * Fills instructionMap with values.
   * Use lambda-functions with no parameters and return type
   * std::unique_ptr<AbstractSyntaxTreeNode> as value.
   * Use UPPERCASE instruction identifier as key.
   */
  void initializeInstructionMap();
};
}

#endif /* ERAGPSIM_ARCH_RISCV_INSTRUCTION_NODE_FACTORY_HPP */
