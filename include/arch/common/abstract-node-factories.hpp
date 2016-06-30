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

#ifndef ERAGPSIM_ARCH_ABSTRACTNODEFACTORIES_HPP
#define ERAGPSIM_ARCH_ABSTRACTNODEFACTORIES_HPP

#include <cassert>
#include <memory>
#include <string>

/**
 * @brief The AbstractSyntaxTreeNode struct
 * Placeholder struct, will be replaced by an #include, when
 * AbstractSyntaxTreeNode-Header is ready
 */
struct AbstractSyntaxTreeNode {};

/**
 * @brief The MemoryValue struct
 * Placeholder struct, will be replaced by an #include, when MemoryValue-Header
 * is ready
 */
struct MemoryValue {};

/**
 * @brief The AbstractInstructionNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type instruction
 */
class AbstractInstructionNodeFactory {
 public:
  AbstractInstructionNodeFactory() {}

  virtual ~AbstractInstructionNodeFactory();

  /**
   * @brief createInstructionNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type instruction for
   * the given token. If the architecture-specific implementation cannot find a
   * instruction with the given token, a nullptr is returned
   *
   * @param token std::string, that contains the identifer of a instruction
   * (e.g. "ADD" for Add-Instruction) in upper-case
   * @return std::unique_ptr pointing to the newly created SyntaxTreeNode, or
   * nullptr if the given token cannot be mapped to a architecture-specific
   * instruction
   */
  virtual std::unique_ptr<AbstractSyntaxTreeNode> createInstructionNode(
      std::string& token) = 0;
};

/**
 * @brief The AbstractImmediateNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type immediate
 */
class AbstractImmediateNodeFactory {
 public:
  AbstractImmediateNodeFactory() {}

  virtual ~AbstractImmediateNodeFactory();

  /**
   * @brief createImmediateNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type immediate with the given numericalValue. If the
   * architecture-specific implementation cannot create an immediate from the
   * given numericalValue, a nullptr is returned
   *
   * @param numericalValue representation of a number to create the immediate
   * @return std::unique_ptr pointing to the newly created SyntaxTreeNode, or
   * nullptr if the architecture-specific implementation cannot create an
   * immediate from the given numericalValue
   */
  virtual std::unique_ptr<AbstractSyntaxTreeNode> createImmediateNode(
      MemoryValue numericalValue) = 0;
};

/**
 * @brief The AbstractRegisterAccessNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type register access
 */
class AbstractRegisterAccessNodeFactory {
 public:
  AbstractRegisterAccessNodeFactory() {}

  virtual ~AbstractRegisterAccessNodeFactory();

  /**
   * @brief createRegisterAccessNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type register access, linked to the given
   * registerAddress. If the given registerAddress is invalid a nullptr is
   * returned.
   *
   * @param registerAdress Identifier or alias linking to a valid register
   * @return std::unique_ptr pointing to the newly created SyntaxTreeNode, or
   * nullptr if the given registerAddress is invalid
   */
  virtual std::unique_ptr<AbstractSyntaxTreeNode> createRegisterAccessNode(
      const std::string& registerAddress) = 0;
};

/**
 * @brief The AbstractMemoryAccessNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type memory access
 */
class AbstractMemoryAccessNodeFactory {
 public:
  AbstractMemoryAccessNodeFactory() {}

  virtual ~AbstractMemoryAccessNodeFactory();

  /**
   * @brief createMemoryAccessNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type memory access.
   *
   * @return std::unique_ptr pointing to the newly created SyntaxTreeNode
   */
  virtual std::unique_ptr<AbstractSyntaxTreeNode> createMemoryAccessNode() = 0;
};

/**
 * @brief The AbstractArithmeticOpNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type arithmetic
 * operation
 */
class AbstractArithmeticOpNodeFactory {
 public:
  static constexpr int ADDITION = 1;
  static constexpr int SUBTRACTION = 2;
  static constexpr int MULTIPLICATION = 3;
  static constexpr int DIVISION = 4;

  AbstractArithmeticOpNodeFactory() {}

  virtual ~AbstractArithmeticOpNodeFactory();

  /**
   * @brief createArithmeticOperationNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type arithmetic operation. Each operation type
   * can be described by constants defined in this class. If the given opType is
   * invalid, a nullptr is returned.
   *
   * @param opType constant, defined in this class, describing the type of the
   * arithmetical operation
   * @return std::unique_ptr pointing to the newly created SyntaxTreeNode, or
   * nullptr if the given opType is invalid
   */
  virtual std::unique_ptr<AbstractSyntaxTreeNode> createArithmeticOperationNode(
      const int opType) = 0;
};

class AbstractNodeFactoryCollection {
 public:
  AbstractNodeFactoryCollection() = default;
  ~AbstractNodeFactoryCollection() = default;

  inline std::unique_ptr<AbstractSyntaxTreeNode> createInstructionNode(
      std::string& token) {
    assert(_instructionF);
    return _instructionF->createInstructionNode(token);
  }

  inline std::unique_ptr<AbstractSyntaxTreeNode> createImmediateNode(
      MemoryValue numericalValue) {
    assert(_immediateF);
    return _immediateF->createImmediateNode(numericalValue);
  }

  inline std::unique_ptr<AbstractSyntaxTreeNode> createRegisterAccessNode(
      const std::string& registerAddress) {
    assert(_registerAccF);
    return _registerAccF->createRegisterAccessNode(registerAddress);
  }

  inline std::unique_ptr<AbstractSyntaxTreeNode> createMemoryAccessNode() {
    assert(_memoryAccF);
    return _memoryAccF->createMemoryAccessNode();
  }

  inline std::unique_ptr<AbstractSyntaxTreeNode> createArithmeticOperationNode(
      const int opType) {
    assert(_arithOpF);
    return _arithOpF->createArithmeticOperationNode(opType);
  }

  void setInstructionFactory(
      std::unique_ptr<AbstractInstructionNodeFactory> instructionFactory) {
    assert(instructionFactory);
    _instructionF = std::move(instructionFactory);
  }

  void setImmediateFactory(
      std::unique_ptr<AbstractImmediateNodeFactory> immediateFactory) {
    assert(immediateFactory);
    _immediateF = std::move(immediateFactory);
  }

  void setRegisterFactory(std::unique_ptr<AbstractRegisterAccessNodeFactory>
                              registerAccessFactory) {
    assert(registerAccessFactory);
    _registerAccF = std::move(registerAccessFactory);
  }

  void setMemoryFactory(
      std::unique_ptr<AbstractMemoryAccessNodeFactory> memoryAccessFactory) {
    assert(memoryAccessFactory);
    _memoryAccF = std::move(memoryAccessFactory);
  }

  void setArithmeticOpFactory(
      std::unique_ptr<AbstractArithmeticOpNodeFactory> arithOpFactory) {
    assert(arithOpFactory);
    _arithOpF = std::move(arithOpFactory);
  }

 private:
  std::unique_ptr<AbstractInstructionNodeFactory> _instructionF;
  std::unique_ptr<AbstractImmediateNodeFactory> _immediateF;
  std::unique_ptr<AbstractRegisterAccessNodeFactory> _registerAccF;
  std::unique_ptr<AbstractMemoryAccessNodeFactory> _memoryAccF;
  std::unique_ptr<AbstractArithmeticOpNodeFactory> _arithOpF;
};

#endif  // ERAGPSIM_ARCH_ABSTRACTNODEFACTORIES_HPP
