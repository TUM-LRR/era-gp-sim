#include "arch/riscv/instruction-node.hpp"

<<<<<<< HEAD
bool InstructionNode::requireChildren(NodeType type, size_t startIndex,
=======
using namespace riscv;

bool InstructionNode::requireChildren(Type type, size_t startIndex,
>>>>>>> master
                                      size_t amount) {
  if (_children.size() - startIndex < amount)
    return false;

  for (size_t i = startIndex; i < startIndex + amount; i++) {
    if (_children.at(i)->getType() != type) {
      return false;
    }
  }
  return true;
}
