/* ERASIM Assembler Interpreter
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

#ifndef ERASIM_ARCH_ABSTRACT_REGISTER_NODE_FACTORY_HPP
#define ERASIM_ARCH_ABSTRACT_REGISTER_NODE_FACTORY_HPP

#include <memory>
#include <string>

#include "arch/common/abstract-register-node.hpp"

/**
 * \brief The AbstractRegisterNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type register access
 */
class AbstractRegisterNodeFactory {
 public:
  using Node = std::shared_ptr<AbstractRegisterNode>;

  /**
   * Destructor.
   */
  virtual ~AbstractRegisterNodeFactory() = default;

  /**
   * Creates a register node for the given identifier.
   *
   * \id registerAdress Identifier or alias linking to a register.
   * \return If the identifier is valid, an `AbstractRegisterNode`, else
   * a `nullptr`.
   */
  virtual Node createRegisterNode(const std::string& id) const = 0;
};

#endif /* ERASIM_ARCH_ABSTRACT_REGISTER_NODE_FACTORY_HPP */
