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
* along with this program. If not, see <http://www.gnu.org/licenses/>.*/

#include <algorithm>
#include <cctype>

#include "arch/riscv/instruction-node-factory.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "arch/riscv/integer-instructions.hpp"
#include "arch/riscv/load-store-instructions.hpp"

namespace riscv {

void InstructionNodeFactory::initializeInstructionMap() {
  // Integer Instructions
  _instructionMap.emplace(
      "ADD", []() { return std::make_unique<AddInstructionNode>(false); });
  _instructionMap.emplace(
      "ADDI", []() { return std::make_unique<AddInstructionNode>(true); });
  _instructionMap.emplace(
      "SUB", []() { return std::make_unique<SubInstructionNode>(false); });
  _instructionMap.emplace(
      "SUBI", []() { return std::make_unique<SubInstructionNode>(true); });

  // Load/Store Instructions
  _instructionMap.emplace("LW", []() {
    return std::make_unique<LoadInstructionNode>(LoadType::WORD);
  });
  _instructionMap.emplace("LH", []() {
    return std::make_unique<LoadInstructionNode>(LoadType::HALF_WORD);
  });
  _instructionMap.emplace("LHU", []() {
    return std::make_unique<LoadInstructionNode>(LoadType::HALF_WORD_UNSIGNED);
  });
  _instructionMap.emplace("LB", []() {
    return std::make_unique<LoadInstructionNode>(LoadType::BYTE);
  });
  _instructionMap.emplace("LBU", []() {
    return std::make_unique<LoadInstructionNode>(LoadType::BYTE_UNSIGNED);
  });
  _instructionMap.emplace("SW", []() {
    return std::make_unique<StoreInstructionNode>(StoreType::WORD);
  });
  _instructionMap.emplace("SH", []() {
    return std::make_unique<StoreInstructionNode>(StoreType::HALF_WORD);
  });
  _instructionMap.emplace("SB", []() {
    return std::make_unique<StoreInstructionNode>(StoreType::BYTE);
  });
}

std::unique_ptr<AbstractSyntaxTreeNode>
InstructionNodeFactory::createInstructionNode(const std::string &token) const {
  using std::begin;
  using std::end;

  // transform token to uppercase
  std::string upper = token;
  std::transform(begin(upper), end(upper), begin(upper), toupper);

  auto it = _instructionMap.find(upper);// lookup the uppercase token
  if (it != end(_instructionMap)) {
    return it->second();// dereference iterator to the key-value pair and call
                        // the function
  } else {
    return nullptr;// return nullptr as the uppercase token could not be found
  }
}
}
