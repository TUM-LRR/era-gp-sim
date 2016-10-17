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
#include "common/utility.hpp"
#include "core/memory-value.hpp"

namespace riscv {

const std::size_t REGISTER_SIZE = 4;

// push the last n bits from the src vector to the dest vector
void pushBackFromEnd(std::vector<bool>& dest,
                     const std::vector<bool>& src,
                     size_t n) {
  int i = src.size() - n - 1;
  while (++i < 0) dest.push_back(false);
  for (; i < src.size(); i++) dest.push_back(src.at(i));
}

// each format has it's own immediate format
void immediateToIFormat(MemoryValue& vec) {
  for (int i = 10; i >= 0; i--) vec.put(i, vec.get(20 + i));
  for (std::size_t i = 31; i > 10; i--) vec.put(i, vec.get(31));
}

void immediateToSFormat(MemoryValue& vec) {
  vec.put(0, vec.get(7));
  for (std::size_t i = 4; i >= 1; i--) vec.put(i, vec.get(7 + i));
  for (std::size_t i = 10; i >= 5; i--) vec.put(i, vec.get(20 + i));
  for (std::size_t i = 31; i > 10; i--) vec.put(i, vec.get(31));
}

void immediateToBFormat(MemoryValue& vec) {
  immediateToSFormat(vec);
  vec.put(11, vec.get(0));
  vec.put(0, false);
}

void immediateToUFormat(MemoryValue& vec) {
  for (int i = 12; i >= 0; i--) vec.put(i, false);
}

void immediateToJFormat(MemoryValue& vec) {
  // clang-format off
      for (std::size_t i = 4; i >= 1; i--) vec.put(i, vec.get(20 + i));
      vec.put(0, false);
      for (std::size_t i = 10; i >= 5; i--) vec.put(i, vec.get(20 + i));
      vec.put(11, vec.get(20));
      for (std::size_t i = 31; i >= 20; i--) vec.put(i, vec.get(31));
  // clang-format on
}

std::vector<bool> RFormat::
operator()(const InstructionKey& key, const std::vector<MemoryValue>& args) {
  std::vector<bool> res;

  // funct7 - 6 bits long
  std::vector<bool> tmp;

  tmp = Utility::convertToBinary(key["function"] >> 3);
  // push the last 7 bits
  pushBackFromEnd(res, tmp, 7);

  auto argument = args.at(2);

  // rs2
  for (int i = REGISTER_SIZE; i >= 0; i--) res.push_back(argument.get(i));
  // rs1
  argument = args.at(1);
  for (int i = REGISTER_SIZE; i >= 0; i--) res.push_back(argument.get(i));
  // funct3 - 3 bits long
  tmp.clear();

  tmp = Utility::convertToBinary(key["function"]);
  pushBackFromEnd(res, tmp, 3);

  // destination
  argument = args.at(0);
  for (int i = REGISTER_SIZE; i >= 0; i--) res.push_back(argument.get(i));

  tmp.clear();
  tmp = Utility::convertToBinary(key["opcode"]);
  pushBackFromEnd(res, tmp, 7);

  return res;
}


std::vector<bool> IFormat::
operator()(const InstructionKey& key, const std::vector<MemoryValue>& args) {
  std::vector<bool> res;
  std::vector<bool> tmp;

  auto imm = args.at(2);

  immediateToIFormat(imm);

  // immediate - 12 bits long
  for (int i = 11; i >= 0; i--) res.push_back(imm.get(i));
  // rs1
  auto argument = args.at(1);
  for (int i = REGISTER_SIZE; i >= 0; i--) res.push_back(argument.get(i));
  // funct3 - 3 bits long
  tmp.clear();

  tmp = Utility::convertToBinary(key["function"]);
  pushBackFromEnd(res, tmp, 3);

  // destination
  argument = args.at(0);
  for (int i = REGISTER_SIZE; i >= 0; i--) res.push_back(argument.get(i));

  tmp.clear();
  tmp = Utility::convertToBinary(key["opcode"]);
  pushBackFromEnd(res, tmp, 7);

  return res;
}

std::vector<bool> SFormat::
operator()(const InstructionKey& key, const std::vector<MemoryValue>& args) {
  std::vector<bool> res;
  std::vector<bool> tmp;

  auto imm = args.at(2);

  immediateToSFormat(imm);

  // immediate[11:5]
  for (std::size_t i = 11; i > 4; i--) res.push_back(imm.get(i));
  // rs2
  auto argument = args.at(1);
  for (int i = REGISTER_SIZE; i >= 0; i--) res.push_back(argument.get(i));
  // rs1
  argument = args.at(0);
  for (int i = REGISTER_SIZE; i >= 0; i--) res.push_back(argument.get(i));
  // funct3 - 3 bits long

  tmp = Utility::convertToBinary(key["function"]);
  pushBackFromEnd(res, tmp, 3);

  // imm[4:0]
  for (int i = 4; i >= 0; i--) res.push_back(imm.get(i));

  tmp.clear();
  tmp = Utility::convertToBinary(key["opcode"]);
  pushBackFromEnd(res, tmp, 7);

  return res;
}

std::vector<bool> SBFormat::
operator()(const InstructionKey& key, const std::vector<MemoryValue>& args) {
  std::vector<bool> res;
  std::vector<bool> tmp;

  auto imm = args.at(2);

  immediateToBFormat(imm);

  // imm[12]
  res.push_back(imm.get(12));
  // immediate[10:5]
  for (std::size_t i = 10; i > 4; i--) res.push_back(imm.get(i));
  // rs2
  auto argument = args.at(1);
  for (int i = REGISTER_SIZE; i >= 0; i--) res.push_back(argument.get(i));
  // rs1
  argument = args.at(0);
  for (int i = REGISTER_SIZE; i >= 0; i--) res.push_back(argument.get(i));
  // funct3 - 3 bits long

  tmp = Utility::convertToBinary(key["function"]);
  pushBackFromEnd(res, tmp, 3);

  // imm[4:1]
  for (std::size_t i = 4; i >= 1; i--) res.push_back(imm.get(i));
  // imm[11]
  res.push_back(imm.get(11));

  tmp.clear();
  tmp = Utility::convertToBinary(key["opcode"]);
  pushBackFromEnd(res, tmp, 7);

  return res;
}

std::vector<bool> UFormat::
operator()(const InstructionKey& key, const std::vector<MemoryValue>& args) {
  std::vector<bool> res;
  std::vector<bool> tmp;

  auto imm = args.at(1);

  immediateToUFormat(imm);

  // immediate[31:12]
  for (std::size_t i = 31; i >= 12; i--) res.push_back(imm.get(i));

  // rd
  auto argument = args.at(0);
  for (int i = REGISTER_SIZE; i >= 0; i--) res.push_back(argument.get(i));

  tmp = Utility::convertToBinary(key["opcode"]);
  pushBackFromEnd(res, tmp, 7);

  return res;
}

std::vector<bool> UJFormat::
operator()(const InstructionKey& key, const std::vector<MemoryValue>& args) {
  std::vector<bool> res;
  std::vector<bool> tmp;

  auto imm = args.at(1);

  immediateToJFormat(imm);

  // immediate[20]
  res.push_back(imm.get(21));
  // imm[10:1]
  for (std::size_t i = 10; i >= 1; i--) res.push_back(imm.get(i));
  // imm[11]
  res.push_back(imm.get(11));
  // imm[19:12]
  for (std::size_t i = 19; i >= 12; i--) res.push_back(imm.get(i));

  // rd
  auto argument = args.at(0);
  for (int i = REGISTER_SIZE; i >= 0; i--) res.push_back(argument.get(i));

  tmp = Utility::convertToBinary(key["opcode"]);
  pushBackFromEnd(res, tmp, 7);

  return res;
}
}
