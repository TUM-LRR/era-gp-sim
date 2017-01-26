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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef ERASIM_ARCH_COMMON_ABSTRACT_REGISTER_NODE_HPP
#define ERASIM_ARCH_COMMON_ABSTRACT_REGISTER_NODE_HPP

#include <string>

#include "arch/common/abstract-syntax-tree-node.hpp"
#include "arch/common/architecture-properties.hpp"
#include "arch/common/validation-result.hpp"
#include "core/memory-value.hpp"

/**
 * A node that represents a register.
 */
class AbstractRegisterNode : public AbstractSyntaxTreeNode {
 public:
  using super = AbstractSyntaxTreeNode;

  /**
   * Constructs a new node that represents a register.
   *
   * \param identifier The identifier for the register.
   */
  explicit AbstractRegisterNode(const std::string& name);

  /**
   * Destructor.
   */
  virtual ~AbstractRegisterNode();

  /**
   * \return The content of the register, represented by this node.
   */
  MemoryValue getValue(MemoryAccess& memoryAccess) const override;

  /**
   * \return succefss, if there are no children.
   */
  ValidationResult validate(MemoryAccess& memoryAccess) const override;

  /**
   * \return The identifier of the register.
   */
  const std::string& getIdentifier() const override;

 protected:
  /** The name of the register (e.g. "x12" or "pc"). */
  std::string _name;
};

#endif /* ERASIM_ARCH_COMMON_ABSTRACT_REGISTER_NODE_HPP */
