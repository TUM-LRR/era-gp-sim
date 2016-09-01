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

#ifndef ERAGPSIM_ARCH_COMMON_ABSTRACT_SYNTAX_TREE_NODE_HPP
#define ERAGPSIM_ARCH_COMMON_ABSTRACT_SYNTAX_TREE_NODE_HPP

#include <memory>
#include <string>
#include <vector>

#include "arch/common/validation-result.hpp"
#include "core/memory-value.hpp"

//Dummy definition of a memory-access
class DummyMemoryAccess {
public:
    virtual MemoryValue getRegisterValue(std::string& token) = 0;
    virtual void setRegisterValue(std::string& token, MemoryValue value) = 0;
};
//Dummy implementation of a memory-access
class DummyMemoryAccessStub : public DummyMemoryAccess {
public:
    MemoryValue getRegisterValue(std::string& token) override {
        return MemoryValue{};
     }
    void setRegisterValue(std::string& token, MemoryValue value) override {}
};

/** The base class for nodes in the abstract syntax tree */
class AbstractSyntaxTreeNode {
 public:
  enum struct Type {
    INSTRUCTION,
    IMMEDIATE,
    REGISTER,
    MEMORY_ACCESS,
    ARITHMETIC
  };

  using Node = std::unique_ptr<AbstractSyntaxTreeNode>;

  /**
   * Executes this node and it's children recursively.
   *
   * \param memory_access Access to memory and registers
   * \return An memory value, that represents the the result of the execution.
   * The meaning differs between different node types.
   */
  virtual MemoryValue getValue(DummyMemoryAccess& memory_access) const = 0;

  /**
   * Validates the structure of this syntax tree. This should be called
   * before every execution.
   *
   * \return Whether this syntax tree is valid for execution.
   */
  virtual const ValidationResult validate() const = 0;

  /**
   * Assembles this syntax tree into its binary representation. So, this
   * simulates the architecture specific assemblage.
   *
   * \return The bit representation of this syntax tree.
   */
  virtual MemoryValue assemble() const = 0;

  /**
   * Returns the identifier of this node. The identifier is formatted as a
   * string and depends on the node type.
   *
   * \return The identifier of this node.
   */
  virtual const std::string& getIdentifier() const = 0;

  /**
   * Getter for the type of this node.
   * \return The type of this node.
   */
  Type getType() {
    return _node_type;
  }

  /**
   * Adds a child to this node. Note, that the node will be added after
   * the last node, that has been added.
   *
   * \param node The node to be added.
   */
  void addChild(Node node) {
    _children.push_back(std::move(node));
  }

  /**
   * Calls validate() on all children
   * \return true, if all children return true, otherwise false
   */
  const ValidationResult validateAllChildren() const {
      for(auto &child : _children) {
          ValidationResult result = child->validate();
          if(!result.isSuccess()) {
              return result;
          }
      }
      return ValidationResult::success();
  }

 protected:
  /**
   * Constructs a new node. The constructor is supposed to be called in
   * the subclasses.
   *
   * \param node_type The type of this node.
   */
  AbstractSyntaxTreeNode(Type node_type) : _node_type(node_type) {
  }

  std::vector<Node> _children;

 private:
  Type _node_type;
};

#endif /* ERAGPSIM_ARCH_COMMON_ABSTRACT_SYNTAX_TREE_NODE_HPP */
