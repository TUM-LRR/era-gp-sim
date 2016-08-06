#include "arch/riscv/instruction-node.hpp"
#include <cassert>

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
    return MemoryValue{};
}
