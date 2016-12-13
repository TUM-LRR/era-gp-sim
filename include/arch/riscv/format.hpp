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

#ifndef ERAGPSIM_ARCH_RISCV_FORMATS_HPP
#define ERAGPSIM_ARCH_RISCV_FORMATS_HPP

#include <functional>
#include <string>
#include <vector>

#include "core/memory-value.hpp"

class InstructionInformation;
class InstructionKey;

namespace riscv {

/**
 * Holds assembler functions for RISC-V instruction formats.
 *
 * The `Format` namespace hosts functions that can produce a binary
 * representation of an instruction, given its static information and dynamic
 * operands. The *static* information refers to the `InstructionInformation`
 * object, which stores information about the instruction format for an
 * instruction, as well as the instruction-specific values that are encoded for
 * each instruction, irrespective of its operands (such as the function bits or
 * opcode). The *dynamic* argumetns are the registers or operands that are
 * supplied to the instruction at run-time.
 *
 * The 6 formats implemented here are (as specified by the RISC-V standard):
 *
 * - R (register-register instructions, like `add`)
 * - I (register-integer instructions, like `xori`)
 * - S (store instructions)
 * - SB (branch instructions)
 * - U (LUI, AUIPC)
 * - UJ (JAL)
 */
namespace Format {

using Operands = std::vector<MemoryValue>;
using AssemblerFunction =
    std::function<MemoryValue(InstructionKey, const Operands&)>;

/**
 * Returns the appropriate assembly function for the given format string.
 * \param  format The name of the format (e.g. "R").
 * \return The assembler function for the given format.
 */
AssemblerFunction getAssembler(const std::string& format);

/**
 * Assembles the operands for the given instruction information.
 *
 * The format is extracted from the information object.
 *
 * \param  instructionInformation The instruction information
 *                                to assemble for.
 * \param  operands The operands to assemble.
 * \return The assembled memory value.
 */
MemoryValue assemble(const InstructionInformation& instructionInformation,
                     const Operands& operands);

/*
 * Assembles instructions with the `R` format.
 *
 * `R` is the format for all *register-register* operations, such as most
 * arithmetic operations that combine two input registers and store their result
 * in an output register. Its layout is:
 *
 * `funct7 | rs2 | rs1 | funct3 | rd | opcode`
 *
 * This format is characterized by first encoding ten function bits, `funct7`
 * and `funct3`, that further specialize the `opcode` of the instruction. It
 * then takes two input registers, `rs1` and `rs2` and one output register
 * `rs3`.
 *
 * \param key The key of the instruction.
 * \param operands The operands of the instruction.
 * \return A `MemoryValue` holding the result of assembling the instruction.
 */
MemoryValue R(const InstructionKey& key, const Operands& operands);

/*
 * Assembles instructions with the `I` format.
 *
 * `I` is the format for all *register-immediate* operations, i.e. all
 * operations that combine an immediate (always 12 bit) with the contents of an
 * other source register, and that store the result in a destination register.
 * The layout of the `I` format is:
 *
 * `imm[11:0] | rs1 | funct3 | rd | opcode`
 *
 * Next two the source and destination register (5 bit each), the instruction
 * takes a 12-bit immediate as the third operand. Then, there are further three
 * bits to specialize the 7-bit opcode.
 *
 * \param key The key of the instruction.
 * \param operands The operands of the instruction.
 * \return A `MemoryValue` holding the result of assembling the instruction.
 */
MemoryValue I(const InstructionKey& key, const Operands& operands);

/*
 * imm[11:5] | rs2 | rs1 | funct3 | imm[4:0] | opcode
 */
MemoryValue S(const InstructionKey& key, const Operands& operands);

/*
 * imm[31:12] | rd | opcode
 */
MemoryValue U(const InstructionKey& key, const Operands& operands);

/*
 * The same as SFormat except the immediate encoding.
 */
MemoryValue SB(const InstructionKey& key, const Operands& operands);

/*
 * The same as UFormat except the immediate encoding.
 */
MemoryValue UJ(const InstructionKey& key, const Operands& operands);
}
}

#endif /* ERAGPSIM_ARCH_RISCV_FORMATS_HPP */
