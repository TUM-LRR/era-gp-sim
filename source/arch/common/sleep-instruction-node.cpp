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
#include <QtCore/qglobal.h>
#include <chrono>

#include "arch/common/sleep-instruction-node.hpp"
#include "common/assert.hpp"
#include "common/utility.hpp"
#include "core/conversions.hpp"
#include "core/memory-access.hpp"

SimulatorSleepInstructionNode::SimulatorSleepInstructionNode(const InstructionInformation &information,
    const PCIncrementer &pcIncrementFunction)
    : AbstractInstructionNode(information),
      _pcIncFunction(pcIncrementFunction) {}

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

  auto opValue = operand->getValue(memoryAccess);
  if (Utility::occupiesMoreBitsThan(opValue, 32)) {
    return ValidationResult::fail(
        QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
        "Immediate value of %1 must be representable in 32bit"), opValue.toHexString(true, false));
  }

  // if operand is negative
  if (opValue.get(31)) {
    return ValidationResult::fail(
        QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
        "Would't a negative sleep time speed up the execution?"));
  }
  return ValidationResult::success();
}

ValidationResult SimulatorSleepInstructionNode::validateRuntime(
    MemoryAccess &memoryAccess) const {
  return validate(memoryAccess);
}

MemoryValue SimulatorSleepInstructionNode::getValue(
    MemoryAccess &memoryAccess) const {
  assert::that(validate(memoryAccess));
  MemoryValue sleeptime = _children.at(0)->getValue(memoryAccess);
  uint32_t mstime = conversions::convert<uint32_t>(sleeptime, 32);
  memoryAccess.sleep(std::chrono::milliseconds(mstime));
  return _pcIncFunction(memoryAccess);
}
