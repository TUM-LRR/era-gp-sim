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

#ifndef ERAGPSIM_ARCH_ABSTRACT_ARITHMETIC_NODE_FACTORY_HPP
#define ERAGPSIM_ARCH_ABSTRACT_ARITHMETIC_NODE_FACTORY_HPP

#include <memory>

#include "arch/common/abstract-syntax-tree-node.hpp"

/**
 * @brief The AbstractArithmeticNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type arithmetic
 * operation
 */
class AbstractArithmeticNodeFactory {
 public:
  using Node = std::unique_ptr<AbstractSyntaxTreeNode>;

  enum class Operation { ADDITION, SUBTRACTION, MULTIPLICATION, DIVISION };

  AbstractArithmeticNodeFactory() {
  }

  virtual ~AbstractArithmeticNodeFactory();

  /**
   * \brief createArithmeticerationNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type arithmetic operation. Each operation type
   * can be described by constants defined in this class. If the given opType is
   * invalid, a nullptr is returned.
   *
   * @param opType constant, defined in this class, describing the type of the
   * arithmetical operation
   * \return std::unique_ptr pointing to the newly created SyntaxTreeNode, or
   * nullptr if the given opType is invalid
   */
  virtual Node createArithmeticNode(Operation operation) const = 0;
};

#endif /* ERAGPSIM_ARCH_ABSTRACT_ARITHMETIC_NODE_FACTORY_HPP */
