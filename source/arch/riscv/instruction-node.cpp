#include <cassert>
#include <string>

#include "arch/common/instruction-assembler.hpp"
#include "arch/common/instruction-key.hpp"
#include "arch/riscv/formats.hpp"
#include "arch/riscv/instruction-node.hpp"

namespace riscv {

constexpr unsigned int str2int(const char* str, int h = 0) {
  return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

InstructionNode::InstructionNode(const InstructionInformation& information)
: super(Type::INSTRUCTION), _information(information) {
}

const std::string& InstructionNode::getIdentifier() const {
  return _information.getMnemonic();
}

bool InstructionNode::_requireChildren(Type type,
                                       size_t startIndex,
                                       size_t amount) const {
  auto first = _children.begin();
  std::advance(first, startIndex);

  auto last = first;
  std::advance(last, amount);

  // clang-format off
  return std::all_of(first, last, [type] (const auto& child) {
    return child->getType() == type;
  });
  // clang-format on
}

bool InstructionNode::_compareChildTypes(TypeList list,
                                         size_t startIndex) const {
  auto view = Utility::viewFrom(_children, startIndex);

  // clang-format off
  return Utility::isEqual(list, view, [](auto type, const auto& child) {
    return type == child->getType();
  });
  // clang-format on
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

  MemoryValue result(boolResult.size() / riscv::BITS_PER_BYTE,
                     riscv::BITS_PER_BYTE);

  for (int i = 0; i < boolResult.size(); i++) result.put(i, boolResult.at(i));

  return result;
}
