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

  // immediate[11:5]
  for (int i = 11; i > 4; i--) res.push_back(imm[i]);
  // rs1
  for (auto bit : args.at(1)) res.push_back(bit);
  // funct3 - 3 bits long
  tmp.clear();
  Utility::convertToBin(tmp, key["funct3"]);
  Utility::push_back_from_end(res, tmp, 3);

  // imm[4:0]
  for (int i = 4; i >= 0; i--) res.push_back(imm[i]);

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

  // immediate[11:5]
  for (int i = 31; i >= 12; i--) res.push_back(imm[i]);

  // rd
  for (auto bit : args.at(0)) res.push_back(bit);

  Utility::convertToBin(tmp, key["opcode"]);
  Utility::push_back_from_end(res, tmp, 7);

  return res;
}
