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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

// There should be a HashMap of instructions to functions for the assembly
// in the abstract factory.

#ifndef ERAGPSIM_ARCH_COMMON_INSTRUCTION_ASSEMBLER_HPP
#define ERAGPSIM_ARCH_COMMON_INSTRUCTION_ASSEMBLER_HPP

#include <functional>
#include <vector>

#include "arch/common/instruction-information.hpp"
#include "arch/common/instruction-node.hpp"

using InstructionKey = InstructionInformation::InstructionKey;

using AssemblerFunction =
    std::function<std::vector<bool>(InstructionKey, InstructionNode)>;

#endif /* ERAGPSIM_ARCH_COMMON_INSTRUCTION_ASSEMBLER_HPP */
