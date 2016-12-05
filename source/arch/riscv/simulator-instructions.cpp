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

#include "arch/riscv/simulator-instructions.hpp"
#include "arch/riscv/utility.hpp"

using namespace riscv;

MemoryValue SleepInstruction::assemble() const {
    constexpr uint32_t OPCODE = 0x72657374;
    return riscv::convert(OPCODE);
}

const Translateable &SleepInstruction::getInstructionDocumentation() const {
    assert::that(_documentation && _documentation->isContextInformationAvailable(getInstructionInformation()));
    return _documentation->getContextInformation(getInstructionInformation());
}

MemoryValue CrashInstruction::assemble() const {
      constexpr riscv::unsigned32_t CRASH_OPCODE = 0x626f6f6d;
      return riscv::convert(CRASH_OPCODE);
}

const Translateable& CrashInstruction::getInstructionDocumentation() const {
    assert::that(_documentation && _documentation->isContextInformationAvailable(getInstructionInformation()));
    return _documentation->getContextInformation(getInstructionInformation());
}

void SleepInstruction::setDocumentation(const std::shared_ptr<InstructionContextInformation> &documentation) {
    _documentation = documentation;
}

void CrashInstruction::setDocumentation(const std::shared_ptr<InstructionContextInformation> &documentation) {
    _documentation = documentation;
}
