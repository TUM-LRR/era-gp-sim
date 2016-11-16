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

class MemoryValue;
class MemoryAccess;
class ValidationResult;

/** The base class for nodes in the abstract syntax tree */
class AbstractSyntaxTreeNode {
 public:
  using Node = std::unique_ptr<AbstractSyntaxTreeNode>;
  using size_t = std::size_t;

  enum class Type {
    INSTRUCTION,
    IMMEDIATE,
    REGISTER,
    MEMORY_ACCESS,
    ARITHMETIC,
    OTHER
  };

  /**
   * Constructs a new node. The constructor is supposed to be called in
   * the subclasses.
   *
   * \param nodeType The type of this node.
   */
  AbstractSyntaxTreeNode(Type nodeType);

  virtual ~AbstractSyntaxTreeNode() = default;

  /**
   * Executes this node recursively and returns a value.
   *
   * \param memoryAccess An access object to memory and registers.
   *
   * \return A type-dependent memory value, representing the the result of
   * the execution.
   */
  virtual MemoryValue getValue(MemoryAccess& memoryAccess) const = 0;

  /** \copydoc getValue() */
  MemoryValue operator()(MemoryAccess& memoryAccess) const;

  /**
   * Validates the structure of this syntax tree. This should be called
   * while the assembler code is parsed.
   *
   * \return Whether this syntax tree is valid for execution.
   */
  virtual ValidationResult validate(MemoryAccess& memoryAccess) const = 0;

  /**
   * Validates, if this instruction is semantically correct during runtime.
   * This should be called before the execution (i.e. a call to getValue()) of
   * each instruction.
   *
   * As most instructions don't need a runtime validation, this method has a
   * default implementation that always returns ValidationResult::success().
   * It is intended, that an instruction, that needs the runtime validation,
   * simply overrides this function.
   *
   * \return Whether this is semantically correct during runtime.
   */
  virtual ValidationResult validateRuntime(MemoryAccess& memoryAccess) const;

  /**
   * Assembles this syntax tree into its binary representation. So, this
   * simulates the architecture specific assemblage.
   *
   * \return The bit representation of this syntax tree.
   */
  virtual MemoryValue assemble() const = 0;

  /**
   * Returns the type-dependent identifier of this node.
   *
   * \return The identifier of this node.
   */
  virtual const std::string& getIdentifier() const = 0;

  /**
   * Getter for the type of this node.
   * \return The type of this node.
   */
  virtual Type getType() const noexcept;

  /**
   * Appends a child to the node.
   *
   * \param node The node to be added.
   */
  virtual void addChild(Node&& node);

  /**
   * Inserts the the child node at the given index.
   *
   * \param index The index to insert at.
   * \param node The child node to insert.
   */
  virtual void insertChild(size_t index, Node&& node);

  /**
   * Sets a child to a new node.
   *
   * \param index The index of the child to re-assign.
   * \param node The new node to assign to the child at the given index.
   */
  virtual void setChild(size_t index, Node&& node);

  /**
   * Returns the number of children this node has.
   */
  virtual size_t numberOfChildren() const noexcept;

  /**
   * Returns true if the node has children, else false.
   */
  virtual bool hasChildren() const noexcept;

 protected:
  /**
   * Calls validate() on all children
   *
   * \return True if all children are valid, else false.
   */
  ValidationResult _validateChildren(MemoryAccess& memoryAccess) const;

  /** The child nodes of this node. */
  std::vector<Node> _children;

 private:
  /** The (enum) type of the node. */
  Type _nodeType;
};

#endif /* ERAGPSIM_ARCH_COMMON_ABSTRACT_SYNTAX_TREE_NODE_HPP */
