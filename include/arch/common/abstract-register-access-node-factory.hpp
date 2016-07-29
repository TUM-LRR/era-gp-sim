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

#ifndef ERAGPSIM_ARCH_ABSTRACT_REGISTER_ACCESS_NODE_FACTORY_HPP
#define ERAGPSIM_ARCH_ABSTRACT_REGISTER_ACCESS_NODE_FACTORY_HPP

#include <string>

#include "arch/common/abstract-syntax-tree-node.hpp"

/**
 * \brief The AbstractRegisterAccessNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type register access
 */
class AbstractRegisterAccessNodeFactory {
 public:
  using Node = std::unique_ptr<AbstractSyntaxTreeNode>;

  virtual ~AbstractRegisterAccessNodeFactory() = default;

  /**
   * \brief createRegisterAccessNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type register access, linked to the given
   * registerAddress. If the given registerAddress is invalid a nullptr is
   * returned.
   *
   * \param registerAdress Identifier or alias linking to a valid register
   * \return std::unique_ptr pointing to the newly created SyntaxTreeNode, or
   * nullptr if the given registerAddress is invalid
   */
  virtual Node
  createRegisterAccessNode(const std::string &registerAddress) const = 0;
};

#endif /* ERAGPSIM_ARCH_ABSTRACT_REGISTER_ACCESS_NODE_FACTORY_HPP */
