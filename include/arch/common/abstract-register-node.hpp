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

#ifndef ERAGPSIM_ARCH_COMMON_ABSTRACT_REGISTER_NODE_HPP
#define ERAGPSIM_ARCH_COMMON_ABSTRACT_REGISTER_NODE_HPP


/** A node that represents a register. */
class RegisterNode : public AbstractSyntaxTreeNode {
 public:
  /**
   * Constructs a new node that represents a register.
   *
   * \param value The identifier for the register.
   */
  RegisterNode(std::string identifier)
  : AbstractSyntaxTreeNode(Type::REGISTER), _identifier(identifier) {
  }

  /**
   * \return The content of the register, represented by this node.
   */
  virtual MemoryValue
  getValue(DummyMemoryAccess& memory_access) const override {
    // TODO Return the actual content of the register using the proper
    // memory access
    return MemoryValue{};
  }

  /**
   * \return true, if there are no children.
   */
  virtual bool validate() const override {
    // Registers can't have any children
    return AbstractSyntaxTreeNode::_children.size() == 0;
  }

  /**
   * \return An empty MemoryValue, because the instruction has to be
   * assembled in the instruction node.
   */
  virtual MemoryValue assemble() const override {
    return MemoryValue{};
  }

  /**
   * \return The identifier of the register.
   */
  virtual const std::string& getIdentifier() const override {
    return _identifier;
  }

 private:
  std::string _identifier;
};
