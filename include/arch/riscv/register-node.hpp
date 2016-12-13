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

#ifndef ERAGPSIM_ARCH_RISCV_REGISTER_NODE_HPP
#define ERAGPSIM_ARCH_RISCV_REGISTER_NODE_HPP

#include <string>

#include "arch/common/abstract-register-node.hpp"

namespace riscv {

/**
 * A concrete register node for RISC-V.
 *
 * It differs mainly from other register nodes in that it must handle
 * assembly specifically for RISC-V.
 */
class RegisterNode : public AbstractRegisterNode {
 public:
  /**
   * Constructs the register node with the given identifier.
   * \param  identifier The string identifier for the register.
   */
  explicit RegisterNode(const std::string &identifier);

  /**
   * \copydoc AbstractSyntaxTreeNode::assemble()
   */
  MemoryValue assemble() const override;
};
}

#endif// ERAGPSIM_ARCH_RISCV_REGISTER_NODE_HPP
