#include "arch/riscv/instruction-node.hpp"

using namespace riscv;

bool InstructionNode::requireChildren(Type type, size_t startIndex,
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

size_t InstructionNode::getChildrenCount() const{
    return _children.size();
}

bool InstructionNode::validateAllChildren() {
    for(auto &child : _children) {
        if(!child->validate()) {
            return false;
        }
    }
    return true;
}
