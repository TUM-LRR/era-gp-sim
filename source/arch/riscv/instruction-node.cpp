#include <cassert>
#include <string>

#include "arch/common/instruction-assembler.hpp"
#include "arch/common/instruction-key.hpp"
#include "arch/riscv/formats.hpp"
#include "arch/riscv/instruction-node.hpp"

namespace riscv {

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
  InstructionKey instructionKey = _information.getKey();

  int format = 2;

  // switch (instructionKey["format"]) {
  // jut for now
  // switch (format) {
  //   case "R": assembler  = RFormat(); break;
  //   case "I": assembler  = IFormat(); break;
  //   case "S": assembler  = SFormat(); break;
  //   case "U": assembler  = UFormat(); break;
  //   case "SB": assembler = SBFormat(); break;
  //   case "UJ": assembler = UJFormat(); break;
  //   default: assembler   = RFormat(); break;
  // }

  switch (format) {
    case 1: assembler  = RFormat(); break;
    case 2: assembler  = IFormat(); break;
    case 3: assembler  = SFormat(); break;
    case 4: assembler  = UFormat(); break;
    case 5: assembler  = SBFormat(); break;
    case 6: assembler  = UJFormat(); break;
    default: assembler = RFormat(); break;
  }

  std::vector<MemoryValue> args;

  for (int i = 0; i < _children.size(); i++) {
    args.push_back(_children.at(i)->assemble());
  }

  auto boolResult = assembler(instructionKey, args);

  MemoryValue result(boolResult.size() / riscv::BITS_PER_BYTE,
                     riscv::BITS_PER_BYTE);

  int k = boolResult.size();
  for (int i = 0; i < boolResult.size(); i++) result.put(--k, boolResult.at(i));

  return result;
}
}
