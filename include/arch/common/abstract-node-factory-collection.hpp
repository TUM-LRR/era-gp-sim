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

#ifndef ERAGPSIM_ARCH_ABSTRACT_NODE_FACTORY_COLLECTION_HPP
#define ERAGPSIM_ARCH_ABSTRACT_NODE_FACTORY_COLLECTION_HPP

#include <memory>
#include <string>

#include "arch/common/instruction-set.hpp"

class Architecture;
class MemoryValue;
class AbstractSyntaxTreeNode;
class AbstractArithmeticNodeFactory;
class AbstractImmediateNodeFactory;
class AbstractInstructionNodeFactory;
class AbstractMemoryAccessNodeFactory;
class AbstractRegisterAccessNodeFactory;

/**
 * \brief The AbstractNodeFactoryCollection class is a convienience class
 * containing an instance of each AbstractNodeFactory.
 * The interface ofeach AbstractNodeFactory is copied for convienient use.
 */
class AbstractNodeFactoryCollection {
 public:
  using Node = std::unique_ptr<AbstractSyntaxTreeNode>;

  static AbstractNodeFactoryCollection
  CreateFor(const Architecture &architecture);

  template <typename FactoryTypes>
  static AbstractNodeFactoryCollection
  Create(const InstructionSet &instructionSet) {
    // clang-format off
    return AbstractNodeFactoryCollection(
        FactoryTypes::instructionFactory(instructionSet),
        FactoryTypes::immediateFactory(),
        FactoryTypes::memoryAccessFactory(),
        FactoryTypes::registerAccessFactory(),
        FactoryTypes::arithmetic()
    );
    // clang-format on
  }

  AbstractNodeFactoryCollection();

  AbstractNodeFactoryCollection(const AbstractNodeFactoryCollection &other);
  AbstractNodeFactoryCollection(AbstractNodeFactoryCollection &&other) noexcept;

  AbstractNodeFactoryCollection &operator=(AbstractNodeFactoryCollection other);

  void swap(AbstractNodeFactoryCollection &other) noexcept;
  friend void swap(AbstractNodeFactoryCollection &first,
                   AbstractNodeFactoryCollection &second) noexcept;

  ~AbstractNodeFactoryCollection();


  /**
   * It is asserted that a corresponding factory must be set prior to this
   * method call, otherwise the assertion will fail
   * \copydoc AbstractInstructionNodeFactory::createInstructionNode
   */
  Node createInstructionNode(const std::string &mnemonic) const;

  /**
   * It is asserted that a corresponding factory must be set prior to this
   * method call, otherwise the assertion will fail
   * \copydoc AbstractImmediateNodeFactory::createImmediateNode
   */
  Node createImmediateNode(const MemoryValue &numericalValue) const;

  /**
   * It is asserted that a corresponding factory must be set prior to this
   * method call, otherwise the assertion will fail
   * \copydoc AbstractRegisterAccessNodeFactory::createRegisterAccessNode
   */
  Node createRegisterAccessNode(const std::string &registerAddress);

  /**
   * It is asserted that a corresponding factory must be set prior to this
   * method call, otherwise the assertion will fail
   * \copydoc AbstractMemoryAccessNodeFactory::createMemoryAccessNode
   */
  Node createMemoryAccessNode() const;

  /**
   * It is asserted that a corresponding factory must be set prior to this
   * method call, otherwise the assertion will fail
   * \copydoc AbstractArithmeticNodeFactory::createArithmeticerationNode
   */
  Node createArithmeticNode(const int opType) const;

 private:
  AbstractNodeFactoryCollection(
      std::unique_ptr<AbstractInstructionNodeFactory> &&instructionFactory,
      std::unique_ptr<AbstractImmediateNodeFactory> &&immediateFactory,
      std::unique_ptr<AbstractMemoryAccessNodeFactory> &&memoryAccessFactory,
      std::unique_ptr<AbstractRegisterAccessNodeFactory>
          &&registerAccessFactory,
      std::unique_ptr<AbstractArithmeticNodeFactory> &&arithmeticFactory);

  std::unique_ptr<AbstractInstructionNodeFactory> _instructionFactory;
  std::unique_ptr<AbstractImmediateNodeFactory> _immediateFactory;
  std::unique_ptr<AbstractRegisterAccessNodeFactory> _registerAccessFactory;
  std::unique_ptr<AbstractMemoryAccessNodeFactory> _memoryAccessFactory;
  std::unique_ptr<AbstractArithmeticNodeFactory> _arithmeticFactory;
};

#endif /* ERAGPSIM_ARCH_ABSTRACT_NODE_FACTORY_COLLECTION_HPP */
