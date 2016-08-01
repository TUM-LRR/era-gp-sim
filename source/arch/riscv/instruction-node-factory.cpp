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
#include "arch/riscv/integer-instructions.hpp"
#include "arch/riscv/load-store-instructions.hpp"

namespace riscv {

//unnamed namespace for this helper function in order not to pollute riscv namespace
namespace {
    template<typename WordSize>
    void initializeIntegerInstructions(InstructionNodeFactory::InstructionMap& _instructionMap) {
        _instructionMap.emplace(
            "add", [](InstructionInformation info){ return std::make_unique<AddInstructionNode<WordSize>>(info, false);});
        _instructionMap.emplace(
            "addi", [](InstructionInformation info) { return std::make_unique<AddInstructionNode<WordSize>>(info, true); });
    }
}

void InstructionNodeFactory::initializeInstructionMap(const Architecture& architecture) {
    assert(architecture.isValid());


    Architecture::word_size_t wordSize = architecture.getWordSize();
  // Integer Instructions
    if(wordSize == InstructionNodeFactory::RV32) {
        initializeIntegerInstructions<InstructionNodeFactory::RV32_integral_t>(_instructionMap);
    }else if(wordSize == InstructionNodeFactory::RV64) {
        initializeIntegerInstructions<InstructionNodeFactory::RV64_integral_t>(_instructionMap);
    }else{
        //The given architecture does not define a valid word_size to create IntegerInstructions
        assert(false);
    }
  _instructionMap.emplace(
      "sub", [](InstructionInformation info) { return std::make_unique<SubInstructionNode>(false); });
  _instructionMap.emplace(
      "subi", [](InstructionInformation info) { return std::make_unique<SubInstructionNode>(true); });

  // Load/Store Instructions
  _instructionMap.emplace("lw", [](InstructionInformation info) {
    return std::make_unique<LoadInstructionNode>(LoadType::WORD);
  });
  _instructionMap.emplace("lh", [](InstructionInformation info) {
    return std::make_unique<LoadInstructionNode>(LoadType::HALF_WORD);
  });
  _instructionMap.emplace("lhu", [](InstructionInformation info) {
    return std::make_unique<LoadInstructionNode>(LoadType::HALF_WORD_UNSIGNED);
  });
  _instructionMap.emplace("lb", [](InstructionInformation info) {
    return std::make_unique<LoadInstructionNode>(LoadType::BYTE);
  });
  _instructionMap.emplace("lbu", [](InstructionInformation info) {
    return std::make_unique<LoadInstructionNode>(LoadType::BYTE_UNSIGNED);
  });
  _instructionMap.emplace("sw", [](InstructionInformation info) {
    return std::make_unique<StoreInstructionNode>(StoreType::WORD);
  });
  _instructionMap.emplace("sh", [](InstructionInformation info) {
    return std::make_unique<StoreInstructionNode>(StoreType::HALF_WORD);
  });
  _instructionMap.emplace("sb", [](InstructionInformation info) {
    return std::make_unique<StoreInstructionNode>(StoreType::BYTE);
  });
}


std::unique_ptr<AbstractSyntaxTreeNode>
InstructionNodeFactory::createInstructionNode(const std::string &token) const {
  using std::begin;
  using std::end;

  // transform token to lowercase
  std::string lower = token;
  std::transform(begin(lower), end(lower), begin(lower), tolower);

  if(!_instrSet.hasInstruction(lower)) {
      return nullptr;// return nullptr as the lowercase token could not be found
  }

  auto it = _instructionMap.find(lower);// lookup the token
  assert(it != end(_instructionMap));
  return it->second(_instrSet.getInstruction(lower));// dereference iterator to the key-value pair and call
                        // the function providing the correct InstructionInformation for the instruction

}
}
