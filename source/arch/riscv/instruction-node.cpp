#include <cassert>

#include "arch/common/instruction-key.hpp"
#include "arch/riscv/instruction-node.hpp"

using namespace riscv;

const std::string& InstructionNode::getIdentifier() const {
  assert(_instructionInformation.isValid() &&
         _instructionInformation.hasMnemonic());
  return _instructionInformation.getMnemonic();
}

bool InstructionNode::requireChildren(Type type,
                                      size_t startIndex,
                                      size_t amount) const {
  if (_children.size() - startIndex < amount) return false;

  for (size_t i = startIndex; i < startIndex + amount; i++) {
    if (_children.at(i)->getType() != type) {
      return false;
    }
  }
  return true;
}

MemoryValue InstructionNode::assemble() const {
  AssemblerFunction assembler;

  InstructionKey instructionKey = _instructionInformation.getKey();

  switch (instructionKey["format"]) {
    case "R": assembler  = RFormat(); break;
    case "I": assembler  = IFormat(); break;
    case "S": assembler  = SFormat(); break;
    case "U": assembler  = UFormat(); break;
    case "SB": assembler = SBFormat(); break;
    case "UJ": assembler = UJFormat(); break;
    default: assembler   = RFormat(); break;
  }

  std::vector<MemoryValue> args;

  for (int i = 0; i < _children.size(); i++) {
    args.push_back(_children.at(i).assemble());
  }

  auto boolResult = assembler(instructionKey, args);

  MemoryValue result(boolResult.size() / RISCV_BITS_PER_BYTE,
                     RISCV_BITS_PER_BYTE);

  for (int i = 0; i < boolResult.size()) result.put(i, boolResult.at(i));

  return result;
}
