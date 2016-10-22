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

#include "include/arch/common/sleep-instruction-node.hpp"
#include <QtCore/qglobal.h>

#include "common/assert.hpp"
#include "common/utility.hpp"
#include "core/conversions.hpp"

SimulatorSleepInstructionNode::SimulatorSleepInstructionNode(
    const InstructionInformation &information,
    const PCIncrementer &pcIncrementFunction, const Assembler &assembleFunction)
    : AbstractInstructionNode(information),
      _pcIncFunction(pcIncrementFunction),
      _assembleFunction(assembleFunction) {}

ValidationResult SimulatorSleepInstructionNode::validate(
    MemoryAccess &memoryAccess) const {
  if (_children.size() != 1) {
    return ValidationResult::fail(
        QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                          "%1 must have exaclty one operand"),
        getInstructionInformation().getMnemonic());
  }
  auto &operand = _children.at(0);
  auto type = operand->getType();
  if (type != Type::IMMEDIATE && type != Type::ARITHMETIC &&
      type != Type::REGISTER && type != Type::MEMORY_ACCESS) {
    return ValidationResult::fail(
        QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                          "%1 may only have an immediate or register operand"),
        getInstructionInformation().getMnemonic());
  }

  if (type == Type::IMMEDIATE &&
      Utility::occupiesMoreBitsThan(operand->getValue(memoryAccess), 12)) {
    return ValidationResult::fail(
        QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                          "%1 the immediate operand is limited to 12 bit"),
        getInstructionInformation().getMnemonic());
  }
  return ValidationResult::success();
}

ValidationResult SimulatorSleepInstructionNode::validateRuntime(
    MemoryAccess &memoryAccess) const {
  return validate(memoryAccess);
}

MemoryValue SimulatorSleepInstructionNode::assemble() const {
    MemoryValue timeOperand = _children.at(0)->assemble();
    return _assembleFunction(timeOperand);
}

MemoryValue SimulatorSleepInstructionNode::getValue(
    MemoryAccess &memoryAccess) const {
  assert::that(validate(memoryAccess));
  MemoryValue sleeptime = _children.at(0)->getValue(memoryAccess);
  uint32_t mstime = conversions::convert<uint32_t>(sleeptime, 32);
  // TODO sleep
  return _pcIncFunction(memoryAccess);
}
