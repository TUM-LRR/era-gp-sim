#include "arch/riscv/instruction_node.hpp"

bool InstructionNode::requireThreeRegisters() {
  if (_children.size() != 3)
    return false;

  for (auto &child : _children) {
    if (child->getType() != NodeType::REGISTER)
      return false;
  }

  return true;
}

bool InstructionNode::requireTwoRegistersAndImmediate() {
  if (_children.size() != 3)
    return false;

  if (_children.at(0)->getType() != NodeType::REGISTER ||
      _children.at(1)->getType() != NodeType::REGISTER ||
      _children.at(2)->getType() != NodeType::IMMEDIATE) {
    return false;
  }

  return true;
}
