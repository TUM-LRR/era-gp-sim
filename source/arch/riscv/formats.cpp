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

#include <iostream>
#include <vector>

#include "arch/common/instruction-key.hpp"
#include "arch/riscv/formats.hpp"
#include "arch/riscv/utility.hpp"
#include "common/utility.hpp"
#include "core/memory-value.hpp"

namespace {

void pushBack(uint32_t& base, uint32_t value, std::size_t size) {
  base <<= size;
  base |= value;
}

void pushBack(uint32_t& base, const MemoryValue& value, std::size_t size) {
  uint32_t numericalValue = riscv::convert<uint32_t>(value);
  pushBack(base, numericalValue, size);
}

/**
 * @brief pushBackInterval
 * @param base
 * @param value
 * @param beginBit incl
 * @param endBit incl
 */
void pushBackInterval(uint32_t& base, const MemoryValue& value, std::size_t beginBit, std::size_t endBit) {
    uint32_t valueNum = riscv::convert<uint32_t>(value);
    valueNum <<= 32-endBit-1;
    valueNum >>= 32-endBit-1+beginBit;
    pushBack(base, valueNum, endBit-beginBit+1);
}
}

namespace riscv {

MemoryValue RFormat::operator()(const InstructionKey& key,
                                const std::vector<MemoryValue>& args) {
  // put values backwards to allow shifting
  uint32_t assembled = key["funct7"];
  pushBack(assembled, key["funct7"], 7);
  pushBack(assembled, args.at(2), 5);
  pushBack(assembled, args.at(1), 5);
  pushBack(assembled, key["funct3"], 3);
  pushBack(assembled, args.at(0), 5);
  pushBack(assembled, key["opcode"], 7);
  return riscv::convert(assembled, 32);
}

MemoryValue IFormat::operator()(const InstructionKey& key,
                                const std::vector<MemoryValue>& args) {
    uint32_t assembled = 0;
    pushBack(assembled, args.at(2), 12);
    pushBack(assembled, args.at(1), 5);
    pushBack(assembled, key["funct3"], 3);
    pushBack(assembled, args.at(0), 5);
    pushBack(assembled, key["opcode"], 7);
    return riscv::convert(assembled, 32);
}

MemoryValue SFormat::operator()(const InstructionKey& key,
                                const std::vector<MemoryValue>& args) {
  uint32_t assembled = 0;
  pushBackInterval(assembled, args.at(2), 5, 11);
  pushBack(assembled, args.at(1), 5);
  pushBack(assembled, args.at(0), 5);
  pushBack(assembled, key["funct3"], 3);
  pushBackInterval(assembled, args.at(2), 0, 4);
  pushBack(assembled, key["opcode"], 7);
  return riscv::convert(assembled, 32);
}

MemoryValue SBFormat::operator()(const InstructionKey& key,
                                 const std::vector<MemoryValue>& args) {
  uint32_t assembled = 0;
  pushBackInterval(assembled, args.at(2), 12, 12);
  pushBackInterval(assembled, args.at(2), 5, 10);
  pushBack(assembled, args.at(1), 5);
  pushBack(assembled, args.at(0), 5);
  pushBack(assembled, key["funct3"], 3);
  pushBackInterval(assembled, args.at(2), 1, 4);
  pushBackInterval(assembled, args.at(2), 11, 11);
  pushBack(assembled, key["opcode"], 7);
  return riscv::convert(assembled, 32);
}

MemoryValue UFormat::operator()(const InstructionKey& key,
                                const std::vector<MemoryValue>& args) {
  uint32_t assembled = 0;
  pushBackInterval(assembled, args.at(1), 12, 31);
  pushBack(assembled, args.at(0), 5);
  pushBack(assembled, key["opcode"], 7);
  return riscv::convert(assembled, 32);
}

MemoryValue UJFormat::operator()(const InstructionKey& key,
                                 const std::vector<MemoryValue>& args) {
    uint32_t assembled = 0;
    pushBackInterval(assembled, args.at(1), 20, 20);
    pushBackInterval(assembled, args.at(1), 1, 10);
    pushBackInterval(assembled, args.at(1), 11, 11);
    pushBackInterval(assembled, args.at(1), 12, 19);
    pushBack(assembled, args.at(0), 5);
    pushBack(assembled, key["opcode"], 7);
    return riscv::convert(assembled, 32);
}
}
