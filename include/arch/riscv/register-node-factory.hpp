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

#ifndef ERAGPSIM_ARCH_RISCV_REGISTER_NODE_FACTORY_HPP
#define ERAGPSIM_ARCH_RISCV_REGISTER_NODE_FACTORY_HPP

#include <unordered_map>

#include "arch/common/abstract-register-node-factory.hpp"

namespace riscv {

/**
 * \brief The RegisterNodeFactory class is a concrete implementation
 * of AbstractRegisterNodeFactory for the RISC-V architecture.
 */
struct RegisterNodeFactory : public AbstractRegisterNodeFactory {
  /**
   * \brief createRegisterNode
   * Creates and returns a new AbstractSyntaxTreeNode that handles register
   * access, or nullptr if the given id is invalid
   * \param id register-id that identifies 1 register of the RISC-V architecture
   * \return std::unique_ptr pointing to the newly created register access node,
   * or nullptr if the given id cannot be mapped to a valid RISC-V register
   */
  virtual std::unique_ptr<AbstractSyntaxTreeNode>
  createRegisterNode(const std::string &id) const override;
};
}

#endif /* ERAGPSIM_ARCH_RISCV_REGISTER_NODE_FACTORY_HPP */
