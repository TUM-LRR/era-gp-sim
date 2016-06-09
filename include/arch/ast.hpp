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
  REGISTER
  /*MEMORY_ACCESS,
  ARITHMETIC*/ // Not needed for RISC V - Can be added for further architectures
};

/* The class definition of a Syntax Tree Node */
template <class IntType> class AbstractSyntaxTreeNode {
private:
  NodeType _node_type;

protected:
  std::vector<AbstractSyntaxTreeNode *> _children;

  /**
   * Constructs a new node. The constructor is supposed to be called in
   * the subclasses.
   *
   * @param node_type The type of this node.
   */
  AbstractSyntaxTreeNode(NodeType node_type) : _node_type(node_type) {}

public:
  /**
   * Executes this node and it's children recursively.
   *
   * @param memory_access Possibility to access memory and registers
   * @return An Integer, that represents the the result of the execution.
   * The meaning differs between different node types.
   */
  virtual IntType getValue(DummyMemoryAccess memory_access) = 0;

  /**
   * Validates the structure of this syntax tree. This should be called
   * before every execution.
   *
   * @return Whether this syntax tree is valid for execution.
   */
  virtual bool validate() = 0;

  /**
   * Assembles this syntax tree into its binary representation. So, this
   * simulates the architecture specific assemblage.
   *
   * @return The bit representation of this syntax tree.
   */
  virtual std::vector<bool> assemble() = 0;

  /**
   * Getter for the type of this node.
   * @return The type of this node.
   */
  NodeType getType() { return _node_type; }

  /**
   * Adds a child to this node. Note, that the node will be added after
   * the last node, that has been added.
   *
   * @param node The node to be added.
   */
  void addChild(AbstractSyntaxTreeNode *node) { _children.pushBack(node); }
};

/* A node that contains a concrete int value. Can be used for immediate
 * and register nodes */
template <class IntType>
class ConcreteValueNode : public AbstractSyntaxTreeNode<IntType> {
private:
  IntType _value;

public:
  /**
   * Constructs a new node that contains a concrete value.
   *
   * @param node_type The type of this node. Should either be
   * IMMEDIATE or REGISTER
   * @param value The value of this node.
   */
  ConcreteValueNode(NodeType node_type, IntType value)
      : AbstractSyntaxTreeNode<IntType>(node_type),
        _value(value) {}

  /**
   * @return The concrete value
   */
  virtual IntType getValue(DummyMemoryAccess memory_access) override {
    return _value;
  }

  /**
   * @return true, if there are no children.
   */
  virtual bool validate() override {
    // Immediate values can't have any children
    return AbstractSyntaxTreeNode<IntType>::_children.size() == 0;
  }

  /**
   * @return An empty std::vector<bool>, because the instruction has to be
   * assembled in the instruction node.
   */
  virtual std::vector<bool> assemble() override { return std::vector<bool>{}; }
};

#endif // AST_H
