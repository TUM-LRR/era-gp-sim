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

#include <vector>

#include "arch/common/instrction-information.hpp"
#include "arch/common/instruction-node.hpp"
#include "arch/riscv/formats.hpp"
#include "common/utility.hpp"
#include "core/memoryvalue.hpp"

namespace {
// methods to transform immediates into their specified formats
// I should possibly reverse the memory values before the following operations
void immToI(MemoryValue& vec) {
  for (int i = 10; i >= 0; i--) vec[i] = vec[20 + i];
  for (int i = 31; i > 10; i--) vec[i] = vec[31];
}

void immToS(MemoryValue& vec) {
  vec[0] = vec[7];
  for (int i = 4; i >= 1; i--) vec[i] = vec[7 + i];
  for (int i = 10; i >= 5; i--) vec[i] = vec[20 + i];
  for (int i = 31; i > 10; i--) vec[i] = vec[31];
}

void immToB(MemoryValue& vec) {
  immToS(vec);
  vec[11] = vec[0];
  vec[0]  = false;
}

void immToU(MemoryValue& vec) {
  for (int i = 12; i >= 0; i--) vec[i] = false;
}

void immToJ(MemoryValue& vec) {
  // clang-format off
  for (int i = 4; i >= 1; i--) vec[i] = vec[20 + i];
  vec[0] = false;
  for (int i = 10; i >= 5; i--) vec[i] = vec[20 + i];
  vec[11] = vec[20];
  for (int i = 31; i >= 20; i--) vec[i] = vec[31];
  // clang-format on
}
}

std::vector<bool> RFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const std::vector<MemoryValue> args) {
  std::vector<bool> res;

  // funct7 - 6 bits long
  std::vector<bool> tmp;
  Utility::convertToBin(tmp, key["funct7"]);
  // push the last 6 bits
  Utility::push_back_from_end(res, tmp, 7);

  // rs2
  for (auto bit : args.at(2)) res.push_back(bit);
  // rs1
  for (auto bit : args.at(1)) res.push_back(bit);
  // funct3 - 3 bits long
  tmp.clear();
  Utility::convertToBin(tmp, key["funct3"]);
  Utility::push_back_from_end(res, tmp, 3);

  // destination
  for (auto bit : args.at(0)) res.push_back(bit);

  tmp.clear();
  Utility::convertToBin(tmp, key["opcode"]);
  Utility::push_back_from_end(res, tmp, 7);

  return res;
}


std::vector<bool> IFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const std::vector<MemoryValue> args) {
  std::vector<bool> res;
  std::vector<bool> tmp;

  auto imm = args.at(2);

  immToI(imm);

  // immediate - 12 bits long
  for (int i = 11; i >= 0; i--) res.push_back(imm[i]);
  // rs1
  for (auto bit : args.at(1)) res.push_back(bit);
  // funct3 - 3 bits long
  tmp.clear();
  Utility::convertToBin(tmp, key["funct3"]);
  Utility::push_back_from_end(res, tmp, 3);

  // destination
  for (auto bit : args.at(0)) res.push_back(bit);

  tmp.clear();
  Utility::convertToBin(tmp, key["opcode"]);
  Utility::push_back_from_end(res, tmp, 7);

  return res;
}

std::vector<bool> SFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const std::vector<MemoryValue> args) {
  std::vector<bool> res;
  std::vector<bool> tmp;

  auto imm = args.at(2);

  immToS(imm);

  // immediate[11:5]
  for (int i = 11; i > 4; i--) res.push_back(imm[i]);
  // rs1
  for (auto bit : args.at(1)) res.push_back(bit);
  // funct3 - 3 bits long
  Utility::convertToBin(tmp, key["funct3"]);
  Utility::push_back_from_end(res, tmp, 3);

  // imm[4:0]
  for (int i = 4; i >= 0; i--) res.push_back(imm[i]);

  tmp.clear();
  Utility::convertToBin(tmp, key["opcode"]);
  Utility::push_back_from_end(res, tmp, 7);

  return res;
}

std::vector<bool> SBFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const std::vector<MemoryValue> args) {
  std::vector<bool> res;
  std::vector<bool> tmp;

  auto imm = args.at(2);

  immToB(imm);

  // imm[12]
  res.push_back(imm[12]);
  // immediate[10:5]
  for (int i = 10; i > 4; i--) res.push_back(imm[i]);
  // rs1
  for (auto bit : args.at(1)) res.push_back(bit);
  // funct3 - 3 bits long
  Utility::convertToBin(tmp, key["funct3"]);
  Utility::push_back_from_end(res, tmp, 3);

  // imm[4:1]
  for (int i = 4; i >= 1; i--) res.push_back(imm[i]);
  // imm[0]
  res.push_back(imm[11]);

  tmp.clear();
  Utility::convertToBin(tmp, key["opcode"]);
  Utility::push_back_from_end(res, tmp, 7);

  return res;
}

std::vector<bool> UFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const std::vector<MemoryValue> args) {
  std::vector<bool> res;
  std::vector<bool> tmp;

  auto imm = args.at(1);

  immToU(imm);

  // immediate[31:12]
  for (int i = 31; i >= 12; i--) res.push_back(imm[i]);

  // rd
  for (auto bit : args.at(0)) res.push_back(bit);

  Utility::convertToBin(tmp, key["opcode"]);
  Utility::push_back_from_end(res, tmp, 7);

  return res;
}

std::vector<bool> UJFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const std::vector<MemoryValue> args) {
  std::vector<bool> res;
  std::vector<bool> tmp;

  auto imm = args.at(1);

  immToJ(imm);

  // immediate[20]
  res.push_back(imm[20]);
  // imm[10:1]
  for (int i = 10; i >= 1; i--) res.push_back(imm[i]);
  // imm[11]
  res.push_back(imm[11]);
  // imm[19:12]
  for (int i = 19; i >= 12; i--) res.push_back(imm[i]);

  // rd
  for (auto bit : args.at(0)) res.push_back(bit);

  Utility::convertToBin(tmp, key["opcode"]);
  Utility::push_back_from_end(res, tmp, 7);

  return res;
}
