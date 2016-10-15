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

#ifndef ERAGPSIM_ARCH_ABSTRCT_MEMORY_ACCESS_NODE_FACTORY_HPP
#define ERAGPSIM_ARCH_ABSTRCT_MEMORY_ACCESS_NODE_FACTORY_HPP

#include <memory>

class AbstractSyntaxTreeNode;

/**
 * \brief The AbstractMemoryAccessNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type memory access
 */
class AbstractMemoryAccessNodeFactory {
 public:
  using Node = std::unique_ptr<AbstractSyntaxTreeNode>;

  virtual ~AbstractMemoryAccessNodeFactory() = default;

  /**
   * \brief createMemoryAccessNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type memory access.
   *
   * \return std::unique_ptr pointing to the newly created SyntaxTreeNode
   */
  virtual Node createMemoryAccessNode() const = 0;
};

#endif /* ERAGPSIM_ARCH_ABSTRCT_MEMORY_ACCESS_NODE_FACTORY_HPP */
