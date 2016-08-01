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

#include "arch/common/instruction-format.hpp"

namespace riscv {
/*
 * funct7 | rs2 | rs1 | funct3 | rd | opcode
 */
struct RFormat : public Format {};

/*
 * imm[11:0] | rs1 | funct3 | rd | opcode
 */
struct IFormat : public Format {};

/*
 * imm[11:5] | rs2 | rs1 | funct3 | imm[4:0] | opcode
 */
struct SFormat : public Format {};

/*
 * imm[31:12] | rd | opcode
 */
struct UFormat : public Format {};

/*
 * The same as SFormat except the immediate encoding.
 */
struct SBFormat : public Format {};

/*
 * The same as UFormat except the immediate encoding.
 */
struct UJFormat : public Format {};
}

#endif /* ERAGPSIM_ARCH_RISCV_FORMATS_HPP */
