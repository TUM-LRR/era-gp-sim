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

#include <algorithm>
#include <string>

#include "arch/common/abstract-arithmetic-node-factory.hpp"
#include "arch/common/abstract-immediate-node-factory.hpp"
#include "arch/common/abstract-instruction-node-factory.hpp"
#include "arch/common/abstract-memory-access-node-factory.hpp"
#include "arch/common/abstract-register-access-node-factory.hpp"
#include "arch/common/abstract-syntax-tree-node.hpp"
#include "arch/common/node-factory-collection.hpp"
#include "common/utility.hpp"

NodeFactoryCollection::NodeFactoryCollection() = default;

NodeFactoryCollection::NodeFactoryCollection(
    const std::shared_ptr<AbstractInstructionNodeFactory> &instructionFactory,
    const std::shared_ptr<AbstractImmediateNodeFactory> &immediateFactory,
    const std::shared_ptr<AbstractMemoryAccessNodeFactory> &memoryAccessFactory,
    const std::shared_ptr<AbstractRegisterAccessNodeFactory>
        &registerAccessFactory,
    const std::shared_ptr<AbstractArithmeticNodeFactory> &arithmeticFactory)
: _instructionFactory(std::move(instructionFactory))
, _immediateFactory(std::move(immediateFactory))
, _registerAccessFactory(std::move(registerAccessFactory))
, _memoryAccessFactory(std::move(memoryAccessFactory))
, _arithmeticFactory(std::move(arithmeticFactory)) {
  // We should at least have these factories
  assert(static_cast<bool>(instructionFactory));
  assert(static_cast<bool>(registerAccessFactory));
  assert(static_cast<bool>(immediateFactory));
}

/**
 * It is asserted that a corresponding factory must be set prior to this
 * method call, otherwise the assertion will fail
 * \copydoc AbstractInstructionNodeFactory::createInstructionNode
 */
NodeFactoryCollection::Node NodeFactoryCollection::createInstructionNode(
    const std::string &mnemonic) const {
  assert(static_cast<bool>(_instructionFactory));
  return _instructionFactory->createInstructionNode(mnemonic);
}

/**
 * It is asserted that a corresponding factory must be set prior to this
 * method call, otherwise the assertion will fail
 * \copydoc AbstractImmediateNodeFactory::createImmediateNode
 */
NodeFactoryCollection::Node NodeFactoryCollection::createImmediateNode(
    const MemoryValue &numericalValue) const {
  assert(static_cast<bool>(_immediateFactory));
  return _immediateFactory->createImmediateNode(numericalValue);
}

/**
 * It is asserted that a corresponding factory must be set prior to this
 * method call, otherwise the assertion will fail
 * \copydoc AbstractRegisterAccessNodeFactory::createRegisterAccessNode
 */
NodeFactoryCollection::Node NodeFactoryCollection::createRegisterAccessNode(
    const std::string &registerName) const {
  assert(static_cast<bool>(_registerAccessFactory));
  return _registerAccessFactory->createRegisterAccessNode(registerName);
}

/**
 * It is asserted that a corresponding factory must be set prior to this
 * method call, otherwise the assertion will fail
 * \copydoc AbstractMemoryAccessNodeFactory::createMemoryAccessNode
 */
NodeFactoryCollection::Node
NodeFactoryCollection::createMemoryAccessNode() const {
  assert(static_cast<bool>(_memoryAccessFactory));
  return _memoryAccessFactory->createMemoryAccessNode();
}

/**
 * It is asserted that a corresponding factory must be set prior to this
 * method call, otherwise the assertion will fail
 * \copydoc AbstractArithmeticNodeFactory::createArithmeticerationNode
 */
NodeFactoryCollection::Node NodeFactoryCollection::createArithmeticNode(
    AbstractArithmeticNodeFactory::Operation operation) const {
  assert(static_cast<bool>(_arithmeticFactory));
  return _arithmeticFactory->createArithmeticNode(operation);
}
