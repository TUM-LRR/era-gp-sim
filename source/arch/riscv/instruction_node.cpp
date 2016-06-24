#include "arch/riscv/instruction_node.hpp"

bool InstructionNode::requireChildren(NodeType type, size_t startIndex,
                                      size_t amount) {
  if (_children.size() - startIndex < amount)
    return false;

  for (size_t i = startIndex; i < amount; i++) {
    if (_children.at(i)->getType() != type) {
      return false;
    }
  }
  return true;
}
