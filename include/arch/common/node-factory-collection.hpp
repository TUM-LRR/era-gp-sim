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

#ifndef ERAGPSIM_ARCH_NODE_FACTORY_COLLECTION_HPP
#define ERAGPSIM_ARCH_NODE_FACTORY_COLLECTION_HPP

#include <memory>
#include <string>

#include "arch/common/abstract-arithmetic-node-factory.hpp"
#include "arch/common/instruction-set.hpp"
#include "core/memory-value.hpp"

class Architecture;
class AbstractSyntaxTreeNode;
class AbstractImmediateNodeFactory;
class AbstractInstructionNodeFactory;
class AbstractMemoryAccessNodeFactory;
class AbstractRegisterNodeFactory;

/**
 * \brief The NodeFactoryCollection class is a convienience class
 * containing an instance of each AbstractNodeFactory.
 * The interface ofeach AbstractNodeFactory is copied for convienient use.
 */
class NodeFactoryCollection {
 public:
  using Node = std::unique_ptr<AbstractSyntaxTreeNode>;

  /**
   * Default-constructs a NodeFactoryCollection.
   */
  NodeFactoryCollection() = default;

  template <typename FactoryTypes>
  static NodeFactoryCollection Create(const InstructionSet &instructionSet,
                                      const Architecture &architecture) {
    // clang-format off
    return NodeFactoryCollection(
        FactoryTypes::instructionFactory(instructionSet, architecture),
        FactoryTypes::immediateFactory(),
        FactoryTypes::memoryAccessFactory(),
        FactoryTypes::registerFactory(),
        FactoryTypes::arithmeticFactory()
    );
    // clang-format on
  }

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
   * \copydoc AbstractRegisterNodeFactory::createRegisterNode
   */
  Node createRegisterNode(const std::string &registerName) const;

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
  Node createArithmeticNode(
      AbstractArithmeticNodeFactory::Operation operation) const;

 private:
  NodeFactoryCollection(
      std::shared_ptr<AbstractInstructionNodeFactory> &&instructionFactory,
      std::shared_ptr<AbstractImmediateNodeFactory> &&immediateFactory,
      std::shared_ptr<AbstractMemoryAccessNodeFactory> &&memoryAccessFactory,
      std::shared_ptr<AbstractRegisterNodeFactory> &&registerFactory,
      std::shared_ptr<AbstractArithmeticNodeFactory> &&arithmeticFactory);

  std::shared_ptr<AbstractInstructionNodeFactory> _instructionFactory;
  std::shared_ptr<AbstractImmediateNodeFactory> _immediateFactory;
  std::shared_ptr<AbstractRegisterNodeFactory> _registerFactory;
  std::shared_ptr<AbstractMemoryAccessNodeFactory> _memoryAccessFactory;
  std::shared_ptr<AbstractArithmeticNodeFactory> _arithmeticFactory;
};

#endif /* ERAGPSIM_ARCH_NODE_FACTORY_COLLECTION_HPP */
