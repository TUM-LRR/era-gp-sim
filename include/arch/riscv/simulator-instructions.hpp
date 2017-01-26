/* ERASIM Assembler Interpreter
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
#ifndef ERASIM_ARCH_RISCV_SIMULATORINSTRUCTIONS_HPP
#define ERASIM_ARCH_RISCV_SIMULATORINSTRUCTIONS_HPP

#include "arch/common/sleep-instruction-node.hpp"
#include "arch/common/crash-instruction-node.hpp"
#include "arch/riscv/instruction-context-information.hpp"



namespace riscv {

/**
 * The RISCV adaption of SimulatorSleepInstructionNode
 */
class SleepInstruction : public SimulatorSleepInstructionNode {
public:

    SleepInstruction(const InstructionInformation& information,
                     const PCIncrementer& pcIncrementFunction) : SimulatorSleepInstructionNode(information, pcIncrementFunction) {}

    MemoryValue assemble() const override;

    const Translateable& getInstructionDocumentation() const override;
    /** \copydoc InstructionNode::getInstructionDocumentation */
    void setDocumentation(const std::shared_ptr<InstructionContextInformation>& documentation);

private:
    /** A pointer to the instruction documentation collection*/
    std::shared_ptr<InstructionContextInformation> _documentation;
};

/**
 * The RISCV adaption of SimulatorCrashInstructionNode
 */
class CrashInstruction : public SimulatorCrashInstructionNode {
public:
    CrashInstruction(const InstructionInformation& info) : SimulatorCrashInstructionNode(info) {}

    MemoryValue assemble() const override;

    const Translateable& getInstructionDocumentation() const override;
    /** \copydoc InstructionNode::getInstructionDocumentation */
    void setDocumentation(const std::shared_ptr<InstructionContextInformation>& documentation);

private:
    /** A pointer to the instruction documentation collection*/
    std::shared_ptr<InstructionContextInformation> _documentation;
};
}
#endif // ERASIM_ARCH_RISCV_SIMULATORINSTRUCTIONS_HPP
