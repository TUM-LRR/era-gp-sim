#include <cassert>
#include <string>

#include "arch/common/instruction-assembler.hpp"
#include "arch/common/instruction-key.hpp"
#include "arch/riscv/formats.hpp"
#include "arch/riscv/instruction-node.hpp"

using namespace riscv;

constexpr unsigned int str2int(const char* str, int h = 0) {
  return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

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
