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

#ifndef ERAGPSIM_ARCH_RISCV_RISCVNODEFACTORIES_HPP_
#define ERAGPSIM_ARCH_RISCV_RISCVNODEFACTORIES_HPP_

#include <unordered_map>

#include "arch/common/abstract_node_factories.hpp"

namespace riscv {

/**
 * \brief The RISCVInstructionNodeFactory class is a
 * concrete Implementation of AbstractInstructionNodeFactory for the RISC-V
 * architecture.
 * \see AbstractInstructionNodeFactory
 */
class RISCVInstructionNodeFactory : public AbstractInstructionNodeFactory {
 public:
  /**
   * \brief RISCVInstructionNodeFactory
   * Creates a Instruction Node Factory for RISC-V architecture
   */
  RISCVInstructionNodeFactory() { initializeInstructionMap(); }

  /*! Default constructed copy constructor */
  RISCVInstructionNodeFactory(RISCVInstructionNodeFactory& copy) = default;

  /*! Default constructed move constructor */
  RISCVInstructionNodeFactory(RISCVInstructionNodeFactory&& move) = default;

  /**
   * \brief createInstructionNode
   * Creates and returns a RISC-V Instruction Node for a valid input token, or
   * nullptr if the token cannot be mapped to a implemented RISC-V instruction
   * \param token
   * \return std::uniqe_ptr pointing to the newly created instruction node, or
   * nullptr if the token cannot be mapped to a implemented RISC-V instruction
   */
  std::unique_ptr<AbstractSyntaxTreeNode> createInstructionNode(
      std::string& token) override;

  ~RISCVInstructionNodeFactory() = default;

 private:
  /**
   * \brief _instructionMap
   * Table, that maps the instruction identifier (e.g. the token "ADD" for
   * Addition) to a function that creates the special instruction node (e.g.
   * AddInstructionNode)
   */
  std::unordered_map<std::string,
                     std::function<std::unique_ptr<AbstractSyntaxTreeNode>()>>
      _instructionMap;

  /**
   * \brief initializeInstructionMap
   * Fills instructionMap with values.
   * Use lambda-functions with no parameters and return type
   * std::unique_ptr<AbstractSyntaxTreeNode> as value.
   * Use UPPERCASE instruction identifier as key.
   */
  void initializeInstructionMap();
};

/**
 * \brief The RISCVImmediateNodeFactory class is a concrete implementation of
 * AbstractImmediateNodeFactory for the RISC-V architecture
 */
class RISCVImmediateNodeFactory : public AbstractImmediateNodeFactory {
 public:
  /**
   * \brief RISCVImmediateNodeFactory
   * Creates a Immediate Node Factory for the RISC-V architecture
   */
  RISCVImmediateNodeFactory() = default;

  /*! Default constructed copy constructor */
  RISCVImmediateNodeFactory(RISCVImmediateNodeFactory& copy) = default;

  /*! Default constructed move constructor */
  RISCVImmediateNodeFactory(RISCVImmediateNodeFactory&& move) = default;

  ~RISCVImmediateNodeFactory() = default;

  /**
   * \brief createImmediateNode
   * Creates and returns an immediate node from the given MemoryValue. If the
   * given input is invalid, a nullptr is returned
   * \param v MemoryValue, which contains the numerical value of the immediate
   * to be created
   * \return std::unique_ptr pointing to the newly created immediate node, or
   * nullptr if the given MemoryValue is invalid
   */
  std::unique_ptr<AbstractSyntaxTreeNode> createImmediateNode(
      MemoryValue& value) override;
};

/**
 * \brief The RISCVRegisterAccessNodeFactory class is a concrete implementation
 * of AbstractRegisterAccessNodeFactory for the RISC-V architecture.
 */
class RISCVRegisterAccessNodeFactory
    : public AbstractRegisterAccessNodeFactory {
 public:
  /**
   * \brief RISCVRegisterAccessNodeFactory
   * Creates a register access node factory for the RISC-V architecture
   */
  RISCVRegisterAccessNodeFactory() = default;

  /*! Default constructed copy constructor */
  RISCVRegisterAccessNodeFactory(RISCVRegisterAccessNodeFactory& copy) =
      default;

  /*! Default constructed move constructor */
  RISCVRegisterAccessNodeFactory(RISCVRegisterAccessNodeFactory&& move) =
      default;

  ~RISCVRegisterAccessNodeFactory() = default;

  /**
   * \brief createRegisterAccessNode
   * Creates and returns a new AbstractSyntaxTreeNode that handles register
   * access, or nullptr if the given id is invalid
   * \param id register-id that identifies 1 register of the RISC-V architecture
   * \return std::unique_ptr pointing to the newly created register access node,
   * or nullptr if the given id cannot be mapped to a valid RISC-V register
   */
  std::unique_ptr<AbstractSyntaxTreeNode> createRegisterAccessNode(
      const std::string& id) override;
};
}
#endif  // ERAGPSIM_ARCH_RISCV_RISCVNODEFACTORIES_HPP_
