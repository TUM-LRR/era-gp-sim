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

#include <cstdint>
#include <string>
#include <unordered_map>

#include "arch/common/abstract-instruction-node-factory.hpp"
#include "arch/common/instruction-information.hpp"
#include "arch/common/instruction-set.hpp"
#include "arch/riscv/control-flow-instructions.hpp"
#include "arch/riscv/factory-map.hpp"
#include "arch/riscv/integer-instructions.hpp"
#include "arch/riscv/load-store-instructions.hpp"

class Architecture;

namespace riscv {

/**
 * \brief The InstructionNodeFactory class is a
 * concrete Implementation of AbstractInstructionNodeFactory for the RISC-V
 * architecture.
 * \see AbstractInstructionNodeFactory
 */
class InstructionNodeFactory : public AbstractInstructionNodeFactory {
 public:
  using Node = std::unique_ptr<AbstractSyntaxTreeNode>;

  /**
   * \brief InstructionNodeFactory
   * Creates a Instruction Node Factory for RISC-V architecture
   */
  InstructionNodeFactory(const InstructionSet &instructions,
                         const Architecture &architecture);

  /**
   * \brief createInstructionNode
   * Creates and returns a RISC-V Instruction Node for a valid input mnemonic,
   * or
   * nullptr if the mnemonic cannot be mapped to a implemented RISC-V
   * instruction
   * \param mnemonic
   * \return std::uniqe_ptr pointing to the newly created instruction node, or
   * nullptr if the mnemonic cannot be mapped to a implemented RISC-V
   * instruction
   */
  Node createInstructionNode(const std::string &mnemonic) const override;

 private:

  using Factory = std::function<std::unique_ptr<AbstractSyntaxTreeNode>(
      const InstructionInformation &)>;

  /**
   * \brief Sets up non-integer instructions.
   */
  void _setupOtherInstructions();

  /**
   * \brief Sets up "w"-Instructions only present in RV64
   */
  void _setup64BitOnlyInstructions();

  /**
   * Table, that maps the instruction identifier (e.g. the mnemonic "add" for
   * Addition) to a factory function that creates the special instruction node
   * (e.g.
   * AddInstructionNode)
   */
  FactoryMap _factories;

  /*!
   * \brief _instructionSet
   * Description of all instructions that can be created by this factory
   */
  InstructionSet _instructionSet;
};
}

#endif /* ERAGPSIM_ARCH_RISCV_INSTRUCTION_NODE_FACTORY_HPP */
