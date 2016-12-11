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
#include "common/utility.hpp"
#include "core/memory-value.hpp"

namespace riscv {
namespace Format {

AssemblyFunction getAssembler(const std::string& format) {
  // clang-format off
  using AssemblyMap = std::unordered_map<std::string, AssemblyFunction>;
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

  return assemblers[format];
}

MemoryValue assemble(const InstructionInformation& instructionInformation,
                     const Arguments& arguments) {
  const auto assembler = getAssembler(instructionInformation.getFormat());
  return assembler(instructionInformation.getKey(), arguments);
}


MemoryValue R(const InstructionKey& key, const Arguments& arguments) {
  auto bits = key["funct7"];
  bits = Utility::appendBits<7>(bits, key["funct7"]);
  bits = Utility::appendBits<5>(bits, arguments[2]);
  bits = Utility::appendBits<5>(bits, arguments[1]);
  bits = Utility::appendBits<3>(bits, key["funct3"]);
  bits = Utility::appendBits<5>(bits, arguments[0]);
  bits = Utility::appendBits<7>(bits, key["opcode"]);

  return riscv::convert(bits, 32);
}

MemoryValue I(const InstructionKey& key, const Arguments& arguments) {
  riscv::unsigned32_t bits = 0;
  bits = Utility::appendBits<12>(bits, arguments[2]);
  bits = Utility::appendBits<5>(bits, arguments[1]);
  bits = Utility::appendBits<3>(bits, key["funct3"]);
  bits = Utility::appendBits<5>(bits, arguments[0]);
  bits = Utility::appendBits<7>(bits, key["opcode"]);

  return riscv::convert(bits, 32);
}

MemoryValue S(const InstructionKey& key, const Arguments& arguments) {
  riscv::unsigned32_t bits = 0;
  bits = Utility::appendBitSlice<5, 11>(bits, arguments[2]);
  bits = Utility::appendBitInterval<1, 3>(bits, arguments[1]);
  bits = Utility::appendBits<5>(bits, sliceBits<1, 3>(arguments));
  bits = Utility::appendBits<5>(bits, arguments[1]);
  bits = Utility::appendBits<5>(bits, arguments[0]);
  bits = Utility::appendBits<3>(bits, key["funct3"]);
  bits = Utility::appendBitSlice<0, 4>(bits, arguments[2]);
  bits = Utility::appendBits<7>(bits, key["opcode"]);

  return riscv::convert(bits, 32);
}

MemoryValue SB(const InstructionKey& key, const Arguments& arguments) {
  riscv::unsigned32_t bits = 0;
  bits = Utility::appendBitSlice<12, 12>(bits, arguments[2]);
  bits = Utility::appendBitSlice<5, 10>(bits, arguments[2]);
  bits = Utility::appendBits<5>(bits, arguments[1]);
  bits = Utility::appendBits<5>(bits, arguments[0]);
  bits = Utility::appendBits<3>(bits, key["funct3"]);
  bits = Utility::appendBitSlice<1, 4>(bits, arguments[2]);
  bits = Utility::appendBitSlice<11, 11>(bits, arguments[2]);
  bits = Utility::appendBits<7>(bits, key["opcode"]);


  return riscv::convert(bits, 32);
}

MemoryValue U(const InstructionKey& key, const Arguments& arguments) {
  riscv::unsigned32_t bits = 0;
  bits = Utility::appendBitSlice<12, 31>(bits, arguments[1]);
  bits = Utility::appendBits<5>(bits, arguments[0]);
  bits = Utility::appendBits<7>(bits, key["opcode"]);

  return riscv::convert(bits, 32);
}

MemoryValue UJ(const InstructionKey& key, const Arguments& arguments) {
  riscv::unsigned32_t bits = 0;
  bits = Utility::appendBitSlice<20, 20>(bits, arguments[1]);
  bits = Utility::appendBitSlice<1, 10>(bits, arguments[1]);
  bits = Utility::appendBitSlice<11, 11>(bits, arguments[1]);
  bits = Utility::appendBitSlice<12, 19>(bits, arguments[1]);
  bits = Utility::appendBits<5>(bits, arguments[0]);
  bits = Utility::appendBits<7>(bits, key["opcode"]);

  return riscv::convert(bits, 32);
}
}
}
