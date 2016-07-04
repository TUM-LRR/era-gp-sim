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

#include "arch/riscv/formats.hpp"


std::vector<bool> RFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const InstructonNode& node) {
  std::vector res(32);

  for (auto bit : key["funct7"]) res.push_back(bit);

  // This is where we need access to a protected member.
  for (auto bit : node._children.at(2)->getValue()) res.push_back(bit);
  // r2
  for (auto bit : node._children.at(1)->getValue()) res.push_back(bit);
  // r1
  for (auto bit : key["funct3"]) res.push_back(bit);

  // destination
  for (auto bit : node._children.at(0)->getValue()) res.push_back(bit);

  for (auto bit : key["opcode"]) res.push_back(bit);

  return res;
}


std::vector<bool> IFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const InstructonNode& node) {
  // TODO
}

std::vector<bool> SFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const InstructonNode& node) {
  // TODO
}

std::vector<bool> UFormat::
operator()(const InstructionInformation::InstructionKey& key,
           const InstructonNode& node) {
  // TODO
}
