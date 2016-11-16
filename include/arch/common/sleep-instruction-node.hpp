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
#ifndef ERAGPSIM_ARCH_COMMON_SLEEP_INSTRUCTION_NODE_HPP
#define ERAGPSIM_ARCH_COMMON_SLEEP_INSTRUCTION_NODE_HPP

#include "arch/common/abstract-instruction-node.hpp"
#include "arch/common/validation-result.hpp"

/**
 * A instruction to halt the execution of the next instruction (i.e. sleep) for
 * a certain amount of time.
 * This time in milliseconds will be the result of calling the only child's
 * getValue() and converting it into a number.
 * This instruction is not automatically available in a architecture
 * implementation. In order to use it, this instruction must be
 * returned in the architecture specific implementation of
 * InstructionNodeFactory.
 * By this, the factory-implementor can choose the mnemonic for this instruction
 * and can customize it to the special needs of the implemented architecture.
 * Such special needs include to ensure that getValue() returnes the adress to
 * the next instruction (by specifying PCIncrementer) and implement a
 * architecture specific assembly of this
 * instruction (using Assembler).
 */
class SimulatorSleepInstructionNode : public AbstractInstructionNode {
 public:
  using PCIncrementer = std::function<MemoryValue(MemoryAccess&)>;
  using Assembler = std::function<MemoryValue(const MemoryValue&)>;

  /**
 * Creates a instruction that holds the execution for a certain time
 * \param information The information object associated with the instruction
 * \param pcIncrementFunction function for returning the next instruction adress
 * \see PCIncrementer
 * \param assembleFunction function for architecture specific assembly of this
 * instruction \see Assembler
 */
  SimulatorSleepInstructionNode(const InstructionInformation& information,
                                const PCIncrementer& pcIncrementFunction,
                                const Assembler& assembleFunction);

  /**
   * Returnes a success only if this instruction has exactly one child either of
   * type INTEGER, ARITHMETIC, REGISTER or MEMORY_ACCESS.
   * \param memoryAccess
   * \return
   */
  ValidationResult validate(MemoryAccess& memoryAccess) const override;

  /**
   * \return The same as validate()
   */
  ValidationResult validateRuntime(MemoryAccess& memoryAccess) const override;

  MemoryValue assemble() const override;

  /**
   * Retrieves the numeric value of the operand and holds the execution of the
   * next instruction for that value in milliseconds
   * \param memoryAccess
   * \return Returnes the adress for the next instruction
   */
  MemoryValue getValue(MemoryAccess& memoryAccess) const override;

 private:
  /**
   * Function to ensure matching architecture specific behaviour without the
   * need to extend.
   * This function gets called after sleeping in getValue() and should return
   * the adress of the next instruction (see return value in getValue()
   * documentation)
   */
  const PCIncrementer _pcIncFunction;
  /**
 *Function to ensure matching architecture specific behaviour without the need
 *to extend.
 * This function should return a MemoryValue containing the assembly of this
 *instruction.
 * Note that a operand-specific assembly is possible as the parameter of this
 *function is
 * the assembly of the only operand and can therefore be used to assemble as
 *well.
 */
  const Assembler _assembleFunction;
};

#endif  // ERAGPSIM_ARCH_COMMON_SLEEP_INSTRUCTION_NODE_HPP
