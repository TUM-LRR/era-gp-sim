#include <cassert>
#include <string>

#include "arch/common/instruction-assembler.hpp"
#include "arch/common/instruction-key.hpp"
#include "arch/riscv/formats.hpp"
#include "arch/riscv/instruction-node.hpp"

namespace riscv {

AssemblerFunction getAssemblerFromFormat(const std::string& format) {
  AssemblerFunction assemblerFunction;
  if (format == "R") {
    assemblerFunction = RFormat();
  } else if (format == "I") {
    assemblerFunction = IFormat();
  } else if (format == "S") {
    assemblerFunction = SFormat();
  } else if (format == "U") {
    assemblerFunction = UFormat();
  } else if (format == "SB") {
    assemblerFunction = SBFormat();
  } else if (format == "UJ") {
    assemblerFunction = UJFormat();
  } else {
    assemblerFunction = RFormat();
  }
  return assemblerFunction;
}

InstructionNode::InstructionNode(const InstructionInformation& information)
    : super(information) {}

bool InstructionNode::_requireChildren(Type type, size_t startIndex,
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
  auto& _information = getInstructionInformation();
  InstructionKey instructionKey = _information.getKey();
  const char* format = _information.getFormat().c_str();

  assembler = getAssemblerFromFormat(format);

  std::vector<MemoryValue> args;

  for (int i = 0; i < _children.size(); i++) {
    args.push_back(_children.at(i)->assemble());
  }

  return assembler(instructionKey, args);
}

const Translateable& InstructionNode::getInstructionDocumentation() const {
    assert::that(_documentation && _documentation->isContextInformationAvailable(getInstructionInformation().getMnemonic()));
    return _documentation->getContextInformation(getInstructionInformation());
}

void InstructionNode::setDocumentation(const std::shared_ptr<InstructionContextInformation> &documentation) {
    _documentation = documentation;
}
}
