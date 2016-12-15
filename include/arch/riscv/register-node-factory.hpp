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

#include <string>

#include "arch/common/abstract-register-node-factory.hpp"
#include "arch/common/architecture-properties.hpp"

class Architecture;

namespace riscv {

/**
 * \brief The RegisterNodeFactory class is a concrete implementation
 * of AbstractRegisterNodeFactory for the RISC-V architecture.
 */
class RegisterNodeFactory : public AbstractRegisterNodeFactory {
 public:
  using super = AbstractRegisterNodeFactory;

  /**
   * Constructs a new `RegisterNodeFactory`.
   * \param  architecture The architecture object to give
   *                      information to the factory.
   */
  explicit RegisterNodeFactory(const Architecture& architecture);

  /**
   * Creates a register node.
   *
   * \param id Register-id that identifies 1 register of the RISC-V architecture
   * \return A pointer pointing to the newly created register access node,
   * or `nullptr` if the given id cannot be mapped to a valid RISC-V register
   */
  Node createRegisterNode(const std::string& id) const override;

 private:
  using id_t = ArchitectureProperties::register_id_t;

  /** A map from register identifiers to their numeric IDs. */
  std::unordered_map<std::string, id_t> _registerIdentifiers;
};
}

#endif /* ERAGPSIM_ARCH_RISCV_REGISTER_NODE_FACTORY_HPP */
