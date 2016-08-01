#include "arch/riscv/integer-instructions.hpp"

namespace riscv {

// TODO
// Check if the immediate values are representable by 20 bits

bool IntegerInstructionNode::validate() const {
  if (_immediate) {
    // If this is an immediate integer instruction, two registers and one
    // immmediate node is required (in that order)
    return requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 2) &&
           requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 2, 1);
  } else {
    // If this is not an immediate integer instruction, three registers are
    // required
    return requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 3);
  }
}

MemoryValue
IntegerInstructionNode::getValue(DummyMemoryAccess& memoryAccess) const {
  // Get dest register
  std::string dest = _children.at(0)->getIdentifier();

  // Eval the operands
  MemoryValue op1 = _children.at(1)->getValue(memoryAccess);
  MemoryValue op2 = _children.at(2)->getValue(memoryAccess);

  // TODO To the magic computation

  return MemoryValue{};
}
}
