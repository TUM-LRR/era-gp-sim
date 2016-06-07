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

#ifndef AST_H
#define AST_H

#include <vector>

/* A dummy memory access. It will be replaced by
 * a proper implementation soon. */
struct DummyMemoryAccess {};

/* The different node types of the Syntax Tree */
enum struct NodeType {
  INSTRUCTION,
  IMMEDIATE,
  REGISTER,
  MEMORY_ACCESS,
  ARITHMETIC
};

/* The class definition of a Syntax Tree Node */
template <class _IntType> class AbstractSyntaxTreeNode {
private:
  NodeType _node_type;

protected:
  std::vector<AbstractSyntaxTreeNode *> _children;

public:
  /**
   * Executes this node and it's children recursively.
   *
   * @param memory_access Possibility to access memory and registers
   * @return An Integer, that represents the the result of the execution.
   * The meaning differs between different node types.
   */
  virtual _IntType GetValue(DummyMemoryAccess memory_access) = 0;

  /**
   * Validates the structure of this syntax tree. This should be called
   * before every execution.
   *
   * @return Whether this syntax tree is valid for execution.
   */
  virtual bool Validate() = 0;

  /**
   * Assembles this syntax tree into its binary representation. So, this
   * simulates the architecture specific assemblage.
   *
   * @return The bit representation of this syntax tree.
   */
  virtual std::vector<bool> Assemble() = 0;

  /**
   * Getter for the type of this node.
   * @return The type of this node.
   */
  NodeType getType() { return _node_type; }

  /**
   * Adds a child to this node. Note, that the node will be added after
   * the last node, that has been added.
   * @param node The node to be added.
   */
  void AddChild(AbstractSyntaxTreeNode *node) { _children.pushBack(node); }
};

#endif // AST_H
