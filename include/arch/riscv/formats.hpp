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

#include "arch/common/instruction-key.hpp"
#include "core/memory-value.hpp"

namespace riscv {

/*
 * funct7 | rs2 | rs1 | funct3 | rd | opcode
 */
struct RFormat{
  virtual MemoryValue
  operator()(const InstructionKey& key, const std::vector<MemoryValue>& args);
};

/*
 * imm[11:0] | rs1 | funct3 | rd | opcode
 */
struct IFormat {
  virtual MemoryValue
  operator()(const InstructionKey& key, const std::vector<MemoryValue>& args);
};

/*
 * imm[11:5] | rs2 | rs1 | funct3 | imm[4:0] | opcode
 */
struct SFormat {
  virtual MemoryValue
  operator()(const InstructionKey& key, const std::vector<MemoryValue>& args);
};

/*
 * imm[31:12] | rd | opcode
 */
struct UFormat  {
  virtual MemoryValue
  operator()(const InstructionKey& key, const std::vector<MemoryValue>& args);
};

/*
 * The same as SFormat except the immediate encoding.
 */
struct SBFormat  {
  virtual MemoryValue
  operator()(const InstructionKey& key, const std::vector<MemoryValue>& args);
};

/*
 * The same as UFormat except the immediate encoding.
 */
struct UJFormat {
  virtual MemoryValue
  operator()(const InstructionKey& key, const std::vector<MemoryValue>& args);
};
}

#endif /* ERAGPSIM_ARCH_RISCV_FORMATS_HPP */
