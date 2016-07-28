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

#include <memory>
#include <string>
#include <cassert>

#include "arch/common/abstract-syntax-tree.hpp"
#include "core/memory-value.hpp"

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
  virtual std::unique_ptr<AbstractSyntaxTreeNode>
  createInstructionNode(const std::string& token) = 0;
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
  virtual std::unique_ptr<AbstractSyntaxTreeNode>
  createImmediateNode(MemoryValue& numericalValue) = 0;
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
  virtual std::unique_ptr<AbstractSyntaxTreeNode>
  createRegisterAccessNode(const std::string& registerAddress) = 0;
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
  virtual std::unique_ptr<AbstractSyntaxTreeNode>
  createArithmeticOperationNode(const int opType) = 0;
};

/**
 * \brief The AbstractNodeFactoryCollection class is a convienience class
 * containing an instance of each AbstractNodeFactory.
 * The interface of each AbstractNodeFactory is copied for convienient use.
 */
class AbstractNodeFactoryCollection {
 public:
  /**
   * \brief AbstractNodeFactoryCollection
   * Constructs a AbstractFactoryCollection containing no instances.
   * Instances can be added by calling the corresponding set-methods
   * \see setInstructionFactory
   * \see setImmediateFactory
   * \see setRegisterFactory
   * \see setMemoryFactory
   * \see setArithmeticOpFactory
   */
  AbstractNodeFactoryCollection() = default;

  /*! No copy constructor as this class uses std::unique_ptr for holding the
   * factory-instances. Use std::move instead */
  AbstractNodeFactoryCollection(AbstractNodeFactoryCollection& copy) = delete;

  /*! Default constructed move constructor */
  AbstractNodeFactoryCollection(AbstractNodeFactoryCollection&& move) = default;

  /**
   * \brief ~AbstractNodeFactoryCollection default dtor
   */
  ~AbstractNodeFactoryCollection() = default;

  /**
   * It is asserted that a corresponding factory must be set prior to this
   * method call, otherwise the assertion will fail
   * \copydoc AbstractInstructionNodeFactory::createInstructionNode
   */
  inline std::unique_ptr<AbstractSyntaxTreeNode> createInstructionNode(
      std::string& token) {
    assert(_instructionFactory);
    return _instructionFactory->createInstructionNode(token);
  }

  /**
   * It is asserted that a corresponding factory must be set prior to this
   * method call, otherwise the assertion will fail
   * \copydoc AbstractImmediateNodeFactory::createImmediateNode
   */
  inline std::unique_ptr<AbstractSyntaxTreeNode> createImmediateNode(
      MemoryValue numericalValue) {
    assert(_immediateFactory);
    return _immediateFactory->createImmediateNode(numericalValue);
  }

  /**
   * It is asserted that a corresponding factory must be set prior to this
   * method call, otherwise the assertion will fail
   * \copydoc AbstractRegisterAccessNodeFactory::createRegisterAccessNode
   */
  inline std::unique_ptr<AbstractSyntaxTreeNode> createRegisterAccessNode(
      const std::string& registerAddress) {
    assert(_registerAccessFactory);
    return _registerAccessFactory->createRegisterAccessNode(registerAddress);
  }

  /**
   * It is asserted that a corresponding factory must be set prior to this
   * method call, otherwise the assertion will fail
   * \copydoc AbstractMemoryAccessNodeFactory::createMemoryAccessNode
   */
  inline std::unique_ptr<AbstractSyntaxTreeNode> createMemoryAccessNode() {
    assert(_memoryAccessFactory);
    return _memoryAccessFactory->createMemoryAccessNode();
  }

  /**
   * It is asserted that a corresponding factory must be set prior to this
   * method call, otherwise the assertion will fail
   * \copydoc AbstractArithmeticOpNodeFactory::createArithmeticOperationNode
   */
  inline std::unique_ptr<AbstractSyntaxTreeNode> createArithmeticOperationNode(
      const int opType) {
    assert(_arithmeticOperationFactory);
    return _arithmeticOperationFactory->createArithmeticOperationNode(opType);
  }

  /**
   * \brief Sets the given AbstractInstructionNodeFactory implementation
   * to be used for creating further InstructionNodes
   * \param instructionFactory std::unqiue_ptr pointing to an implementation
   * instance of
   * AbstractInstructionNodeFactory; cannot be a nullptr (this is asserted)
   * \return this instance for method chaining
   * \see createInstructionNode(std::string&)
   */
  AbstractNodeFactoryCollection& setInstructionFactory(
      std::unique_ptr<AbstractInstructionNodeFactory>&& instructionFactory) {
    assert(instructionFactory);
    _instructionFactory = std::move(instructionFactory);
    return *this;
  }

  /**
   * \brief Sets the given AbstractImmediateNodeFactory implementation
   * to be used for creating further ImmediateNodes
   * \param immediateFactory std::unique_ptr pointing to an implementation
   * instance of
   * AbstractImmediateFactory; cannot be a nullptr (this is asserted)
   * \return this instance for method chaining
   * \see createImmediateNode(MemoryValue)
   */
  AbstractNodeFactoryCollection& setImmediateFactory(
      std::unique_ptr<AbstractImmediateNodeFactory>&& immediateFactory) {
    assert(immediateFactory);
    _immediateFactory = std::move(immediateFactory);
    return *this;
  }

  /**
   * \brief Sets the given AbstractRegisterAccessFactory implementation
   * to be used for creating further RegisterAccessNodes
   * \param registerAccessFactory std::unique_ptr pointing to an implementation
   * instance of
   * AbstractRegisterAccessFactory; cannot be a nullptr (this is asserted)
   * \return this instance for method chaining
   * \see createRegisterAccessNode(const std::string&)
   */
  AbstractNodeFactoryCollection& setRegisterFactory(
      std::unique_ptr<AbstractRegisterAccessNodeFactory>&&
          registerAccessFactory) {
    assert(registerAccessFactory);
    _registerAccessFactory = std::move(registerAccessFactory);
    return *this;
  }

  /**
   * \brief Sets the given AbstractMemoryAccessNodeFactory implementation
   * to be used for creating further MemoryAccessNodes
   * \param memoryAccessFactory std::unique_ptr pointing to an implementation
   * instance of
   * AbstractMemoryAccessNodeFactory; cannot be a nullptr (this is asserted)
   * \return this instance for method chaining
   * \see createMemoryAccessNode()
   */
  AbstractNodeFactoryCollection& setMemoryFactory(
      std::unique_ptr<AbstractMemoryAccessNodeFactory>&& memoryAccessFactory) {
    assert(memoryAccessFactory);
    _memoryAccessFactory = std::move(memoryAccessFactory);
    return *this;
  }

  /**
   * \brief Sets the given AbstractArithmeticOpNodeFactory implementation
   * to be used for creating further ArithmeticOpNodes
   * \param arithOpFactory std::unique_ptr pointing to an implementation
   * instance of
   * AbstractArithmeticOpFactory; cannot be a nullptr (this is asserted)
   * \return this instance for method chaining
   * \see createArithemticOperationNode(const int)
   */
  AbstractNodeFactoryCollection& setArithmeticOpFactory(
      std::unique_ptr<AbstractArithmeticOpNodeFactory>&& arithOpFactory) {
    assert(arithOpFactory);
    _arithmeticOperationFactory = std::move(arithOpFactory);
    return *this;
  }

 private:
  /**
   * \brief _instructionFactory points to a AbstractInstructionNodeFactory
   * implementation
   */
  std::unique_ptr<AbstractInstructionNodeFactory> _instructionFactory;
  /**
   * \brief _immediateFactory points to a AbstractImmediateNodeFactory
   * implementation
   */
  std::unique_ptr<AbstractImmediateNodeFactory> _immediateFactory;
  /**
   * \brief _registerAccessFactory points to a AbstractRegisterAccessNodeFactory
   * implementation
   */
  std::unique_ptr<AbstractRegisterAccessNodeFactory> _registerAccessFactory;
  /**
   * \brief _memoryAccessFactory points to a AbstractMemoryAccessNodeFactory
   * implementation
   */
  std::unique_ptr<AbstractMemoryAccessNodeFactory> _memoryAccessFactory;
  /**
   * \brief _arithmeticOperationFactory points to a
   * AbstractArithmeticOpNodeFactory implementation
   */
  std::unique_ptr<AbstractArithmeticOpNodeFactory> _arithmeticOperationFactory;
};

#endif  // ERAGPSIM_ARCH_ABSTRACTNODEFACTORIES_HPP
