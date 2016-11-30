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
#include "include/arch/common/crash-instruction-node.hpp"
#include <QtCore/qglobal.h>

SimulatorCrashInstructionNode::SimulatorCrashInstructionNode(const InstructionInformation &info)
    : AbstractInstructionNode(info) {}

ValidationResult SimulatorCrashInstructionNode::validate(
    MemoryAccess &memoryAccess) const {
  if (_children.size() != 1) {
    return ValidationResult::fail(
        QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                          "%1 may only have 1 operand"),
        getInstructionInformation().getMnemonic());
  }
  if (_children.at(0)->getType() != Type::OTHER) {
    return ValidationResult::fail(
        QT_TRANSLATE_NOOP("Syntax-Tree-Validation", "%1 invalid operand type"),
        getInstructionInformation().getMnemonic());
  }
  return ValidationResult::success();
}

ValidationResult SimulatorCrashInstructionNode::validateRuntime(
    MemoryAccess &memoryAccess) const {
  auto operandText = _children.at(0)->getIdentifier();
  std::string customMsg = std::string(operandText.begin(), operandText.end());

  return ValidationResult::fail(
      QT_TRANSLATE_NOOP("Simulator-Debug-Crash-Message",
                        "Program terminated.\nCause:\n%1"),
      customMsg);
}

MemoryValue SimulatorCrashInstructionNode::getValue(
    MemoryAccess &memoryAccess) const {
  assert::that(false);  // this should never be called
  return MemoryValue();
}
