/* C++ Assembly Interpreter
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

#include <string>
#include <unordered_map>

#include "arch/common/instruction-key.hpp"
#include "arch/riscv/format.hpp"
#include "arch/riscv/properties.hpp"
#include "arch/riscv/utility.hpp"
#include "common/bit-builder.hpp"
#include "common/utility.hpp"
#include "core/memory-value.hpp"

namespace riscv {
namespace Format {
namespace Detail {
riscv::unsigned32_t convert(const MemoryValue& memoryValue) {
  return riscv::convert<riscv::unsigned32_t>(memoryValue);
}
}

AssemblerFunction getAssembler(const std::string& format) {
  // clang-format off
  using AssemblyMap = std::unordered_map<std::string, AssemblerFunction>;
  static const AssemblyMap assemblers = {
      {"R", Format::R},
      {"I", Format::I},
      {"S", Format::S},
      {"U", Format::U},
      {"SB", Format::SB},
      {"UJ", Format::UJ}
  };
  // clang-format on

  assert::that(assemblers.count(format) > 0);

  return assemblers.at(format);
}

MemoryValue assemble(const InstructionInformation& instructionInformation,
                     const Operands& operands) {
  const auto assembler = getAssembler(instructionInformation.getFormat());
  return assembler(instructionInformation.getKey(), operands);
}

MemoryValue R(const InstructionKey& key, const Operands& operands) {
  using Detail::convert;

  auto bits = BitBuilder<riscv::unsigned32_t>()
                  .addUpTo(key["opcode"], 6)
                  .addUpTo(convert(operands[0]), 4)
                  .addUpTo(key["funct3"], 2)
                  .addUpTo(convert(operands[1]), 4)
                  .addUpTo(convert(operands[2]), 4)
                  .addUpTo(key["funct7"], 6)
                  .build();

  return riscv::convert(bits, 32);
}

MemoryValue I(const InstructionKey& key, const Operands& operands) {
  using Detail::convert;

  auto bits = BitBuilder<riscv::unsigned32_t>()
                  .addUpTo(key["opcode"], 6)
                  .addUpTo(convert(operands[0]), 4)
                  .addUpTo(key["funct3"], 2)
                  .addUpTo(convert(operands[1]), 4)
                  .addUpTo(convert(operands[2]), 11)
                  .build();

  return riscv::convert(bits, 32);
}

MemoryValue S(const InstructionKey& key, const Operands& operands) {
  using Detail::convert;

  const auto immediate = convert(operands[2]);

  auto bits = BitBuilder<riscv::unsigned32_t>()
                  .addUpTo(key["opcode"], 6)
                  .addUpTo(immediate, 4)
                  .addUpTo(key["funct3"], 2)
                  .addUpTo(convert(operands[0]), 4)
                  .addUpTo(convert(operands[1]), 4)
                  .addRange(immediate, 5, 11)
                  .build();

  return riscv::convert(bits, 32);
}

MemoryValue SB(const InstructionKey& key, const Operands& operands) {
  using Detail::convert;

  const auto immediate = convert(operands[2]);

  auto bits = BitBuilder<riscv::unsigned32_t>()
                  .addUpTo(key["opcode"], 6)
                  .addBit(immediate, 10)
                  .addRange(immediate, 0, 3)
                  .addUpTo(key["funct3"], 2)
                  .addUpTo(convert(operands[0]), 4)
                  .addUpTo(convert(operands[1]), 4)
                  .addRange(immediate, 4, 9)
                  .addBit(immediate, 11)
                  .build();

  return riscv::convert(bits, 32);
}

MemoryValue U(const InstructionKey& key, const Operands& operands) {
  using Detail::convert;

  auto bits = BitBuilder<riscv::unsigned32_t>()
                  .addUpTo(key["opcode"], 6)
                  .addUpTo(convert(operands[0]), 4)
                  .addUpTo(convert(operands[1]), 19)
                  .build();

  return riscv::convert(bits, 32);
}

MemoryValue UJ(const InstructionKey& key, const Operands& operands) {
  using Detail::convert;

  const auto immediate = convert(operands[1]);

  auto bits = BitBuilder<riscv::unsigned32_t>()
                  .addUpTo(key["opcode"], 6)
                  .addUpTo(convert(operands[0]), 4)
                  .addRange(immediate, 11, 18)
                  .addBit(immediate, 10)
                  .addRange(immediate, 0, 9)
                  .addBit(immediate, 19)
                  .build();

  return riscv::convert(bits, 32);
}
}
}
