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

#ifndef ERAGPSIM_ARCH_COMMON_REGISTER_NODE_HPP
#define ERAGPSIM_ARCH_COMMON_REGISTER_NODE_HPP

#include <QtGlobal>
#include <memory>

#include "arch/common/abstract-syntax-tree-node.hpp"

/** A node that represents a register. */
class RegisterNode : public AbstractSyntaxTreeNode {
 public:
  using super = AbstractSyntaxTreeNode;

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
  MemoryValue getValue(DummyMemoryAccess& memory_access) const override {
    // TODO Return the actual content of the register using the proper
    // memory access
    return MemoryValue();
  }

  /**
   * \return success, if there are no children.
   */
  const ValidationResult validate() const override {
    // Registers can't have any children
    return AbstractSyntaxTreeNode::_children.size() == 0
               ? ValidationResult::success()
               : ValidationResult::fail(QT_TRANSLATE_NOOP(
                     "Syntax-Tree-Validation",
                     "The register node must not have any arguments"));
  }

  /**
   * \return success, always
   */
  const ValidationResult
  validateRuntime(DummyMemoryAccess& memoryAccess) const override {
    return ValidationResult::success();
  }

  const std::string& getIdentifier() const override {
    return _identifier;
  }

  MemoryValue assemble() const override {
    MemoryValue memValue(1, 8);

    std::string::size_type sz;

    int index;

    try {
      index = std::stoi(_identifier, &sz);
    } catch (const std::exception&) {
      index = 0;
    }

    const int regSize = 8;

    for (int i = 0; i < 5; i++) {
      if (index % 2 == 0)
        memValue.put(regSize - i - 1, false);
      else
        memValue.put(regSize - i - 1, true);

      index /= 2;
    }

    return memValue;
  }

 private:
  /*!
   * Identifies a register
   */
  std::string _identifier;
};

#endif /* ERAGPSIM_ARCH_COMMON_REGISTER_NODE_HPP */
