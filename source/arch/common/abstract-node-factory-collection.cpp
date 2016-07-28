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
#include "arch/common/abstract-node-factory-collection.hpp"
#include "arch/common/abstract-register-access-node-factory.hpp"
#include "arch/common/abstract-syntax-tree-node.hpp"
#include "arch/common/architecture.hpp"
#include "arch/riscv/factory-types.hpp"
#include "common/utility.hpp"

AbstractNodeFactoryCollection
AbstractNodeFactoryCollection::CreateFor(const Architecture &architecture) {
  if (architecture.getName() == "risc-v") {
    return Create<riscv::FactoryTypes>(architecture.getInstructions());
  } else {
    assert(false);
  }
}

AbstractNodeFactoryCollection::AbstractNodeFactoryCollection() = default;

AbstractNodeFactoryCollection::AbstractNodeFactoryCollection(
    const AbstractNodeFactoryCollection &other)
: _instructionFactory(Utility::copyPointer(other._instructionFactory))
, _immediateFactory(Utility::copyPointer(other._immediateFactory))
, _memoryAccessFactory(Utility::copyPointer(other._memoryAccessFactory))
, _registerAccessFactory(Utility::copyPointer(other._registerAccessFactory))
, _arithmeticFactory(Utility::copyPointer(other._arithmeticFactory)) {
}

AbstractNodeFactoryCollection::AbstractNodeFactoryCollection(
    AbstractNodeFactoryCollection &&other) noexcept {
  swap(other);
}

AbstractNodeFactoryCollection &AbstractNodeFactoryCollection::
operator=(AbstractNodeFactoryCollection other) {
  swap(other);
  return *this;
}

void AbstractNodeFactoryCollection::swap(
    AbstractNodeFactoryCollection &other) noexcept {
  // Enable ADL
  using std::swap;

  swap(_instructionFactory, other._instructionFactory);
  swap(_immediateFactory, other._immediateFactory);
  swap(_memoryAccessFactory, other._memoryAccessFactory);
  swap(_registerAccessFactory, other._registerAccessFactory);
  swap(_arithmeticFactory, other._arithmeticFactory);
}
void swap(AbstractNodeFactoryCollection &first,
          AbstractNodeFactoryCollection &second) noexcept {
  first.swap(second);
}


AbstractNodeFactoryCollection::~AbstractNodeFactoryCollection() = default;


/**
 * It is asserted that a corresponding factory must be set prior to this
 * method call, otherwise the assertion will fail
 * \copydoc AbstractInstructionNodeFactory::createInstructionNode
 */
AbstractNodeFactoryCollection::Node
AbstractNodeFactoryCollection::createInstructionNode(
    const std::string &mnemonic) const {
  assert(_instructionFactory);
  return _instructionFactory->createInstructionNode(mnemonic);
}

/**
 * It is asserted that a corresponding factory must be set prior to this
 * method call, otherwise the assertion will fail
 * \copydoc AbstractImmediateNodeFactory::createImmediateNode
 */
AbstractNodeFactoryCollection::Node
AbstractNodeFactoryCollection::createImmediateNode(
    const MemoryValue &numericalValue) const {
  assert(_immediateFactory);
  return _immediateFactory->createImmediateNode(numericalValue);
}

/**
 * It is asserted that a corresponding factory must be set prior to this
 * method call, otherwise the assertion will fail
 * \copydoc AbstractRegisterAccessNodeFactory::createRegisterAccessNode
 */
AbstractNodeFactoryCollection::Node
AbstractNodeFactoryCollection::createRegisterAccessNode(
    const std::string &registerAddress) const {
  assert(_registerAccessFactory);
  return _registerAccessFactory->createRegisterAccessNode(registerAddress);
}

/**
 * It is asserted that a corresponding factory must be set prior to this
 * method call, otherwise the assertion will fail
 * \copydoc AbstractMemoryAccessNodeFactory::createMemoryAccessNode
 */
Node createMemoryAccessNode() const {
  assert(static_cast<bool>(_memoryAccessFactory));
  return _memoryAccessFactory->createMemoryAccessNode();
}

/**
 * It is asserted that a corresponding factory must be set prior to this
 * method call, otherwise the assertion will fail
 * \copydoc AbstractArithmeticNodeFactory::createArithmeticerationNode
 */
AbstractNodeFactoryCollection::Node
AbstractNodeFactoryCollection::createArithmeticNode(const int opType) const {
  assert(static_cast<bool>(_arithmeticFactory));
  return _arithmeticFactory->createArithmeticNode(opType);
}

AbstractNodeFactoryCollection::AbstractNodeFactoryCollection(
    std::unique_ptr<AbstractInstructionNodeFactory> &&instructionFactory,
    std::unique_ptr<AbstractImmediateNodeFactory> &&immediateFactory,
    std::unique_ptr<AbstractMemoryAccessNodeFactory> &&memoryAccessFactory,
    std::unique_ptr<AbstractRegisterAccessNodeFactory> &&registerAccessFactory,
    std::unique_ptr<AbstractArithmeticNodeFactory> &&arithmeticFactory)
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
