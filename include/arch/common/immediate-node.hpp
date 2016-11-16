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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef ERAGPSIM_ARCH_COMMON_IMMEDIATE_NODE_HPP
#define ERAGPSIM_ARCH_COMMON_IMMEDIATE_NODE_HPP

#include <QtCore/qglobal.h>
#include <memory>
#include <string>

#include "arch/common/abstract-syntax-tree-node.hpp"
#include "core/memory-value.hpp"

class MemoryAccess;

/** A node that contains a concrete memory value. */
class ImmediateNode : public AbstractSyntaxTreeNode {
 public:
  /**
   * Constructs a new node that contains a concrete value.
   *
   * \param value The value of this node.
   */
  ImmediateNode(const MemoryValue& value);

  ~ImmediateNode() = default;

  void setValue(const MemoryValue& value);

  /**
   * \return The concrete value
   */
  MemoryValue getValue(MemoryAccess& MemoryAccess) const override;

  /**
   * \return success, if there are no children.
   */
  ValidationResult validate(MemoryAccess& memoryAccess) const override;

  /**
   * \return Return the value of the immediate.
   */
  MemoryValue assemble() const override;

  /**
   * Returns always the same string: "imm".
   *
   * \return The string "imm"
   */
  const std::string& getIdentifier() const override;

 private:
  MemoryValue _value;

  // needed, because getIdentifier returns a reference
  static const std::string IMMEDIATE_IDENTIFIER;
};

#endif /* ERAGPSIM_ARCH_COMMON_IMMEDIATE_NODE_HPP */
