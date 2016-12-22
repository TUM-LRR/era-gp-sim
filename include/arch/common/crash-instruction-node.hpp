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
#ifndef ERA_SIM_ARCH_COMMON_CRASH_INSTRUCTION_NODE_HPP
#define ERA_SIM_ARCH_COMMON_CRASH_INSTRUCTION_NODE_HPP

#include "arch/common/abstract-instruction-node.hpp"
#include "arch/common/validation-result.hpp"
#include "core/memory-value.hpp"

/**
 * A instruction that will terminate the execution and display a message defined
 * by the user (via the operands of this instruction). This can be used e.g. for
 * debug, crash or assertions in the code.
 * This instruction is not automatically available in a architecture
 * implementation. In order to use it, this instruction must be
 * returned in the architecture specific implementation of
 * InstructionNodeFactory.
 * By this, the factory-implementor can choose the mnemonic for this instruction
 * and can customize it to the special needs of the implemented architecture.
 */
class SimulatorCrashInstructionNode : public AbstractInstructionNode {
 public:
  /**
 * Creates a new instruction that will terminate the execution and
 * \param info The information object associated with the instruction
 * \param assembled The assembled format of this instruction. As this
 * instruction likely holds operands that cannot be assembled into a fixed bit
 * value (like a custom message text) the assembled format may not depend on the
 * operands assembled format.
 */
  SimulatorCrashInstructionNode(const InstructionInformation &info);

  /** \copydoc AbstractInstructionNode::validate() */
  ValidationResult validate(MemoryAccess &memoryAccess) const override;

  /**
   * This will always fail and generate a RunTimeError containing the custom error message
   * \param memoryAccess
   * \return A failed ValidationResult containing the custom error message
   */
  ValidationResult validateRuntime(MemoryAccess &memoryAccess) const override;

  /**
   * It is not intended to call this function as execution should stop when validateRuntime() fails. This
   * is asserted
   */
  MemoryValue getValue(MemoryAccess &memoryAccess) const override;
};

#endif  // ERA_SIM_ARCH_COMMON_CRASH_INSTRUCTION_NODE_HPP
