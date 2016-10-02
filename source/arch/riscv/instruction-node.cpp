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
  AssemblerFunction assembler;
  InstructionKey instructionKey = _instructionInformation.getKey();
  const char* format            = _instructionInformation.getFormat().c_str();

  switch (str2int(format)) {
    case str2int("R"): assembler  = RFormat(); break;
    case str2int("I"): assembler  = IFormat(); break;
    case str2int("S"): assembler  = SFormat(); break;
    case str2int("U"): assembler  = UFormat(); break;
    case str2int("SB"): assembler = SBFormat(); break;
    case str2int("UJ"): assembler = UJFormat(); break;
    default: assembler            = RFormat(); break;
  }

  std::vector<MemoryValue> args;

  for (int i = 0; i < _children.size(); i++) {
    args.push_back(_children.at(i)->assemble());
  }

  auto boolResult = assembler(instructionKey, args);

  MemoryValue result(boolResult.size() / RISCV_BITS_PER_BYTE,
                     RISCV_BITS_PER_BYTE);

  for (int i = 0; i < boolResult.size(); i++) result.put(i, boolResult.at(i));

  return result;
}
