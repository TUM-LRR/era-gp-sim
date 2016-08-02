#include "arch/riscv/integer-instructions.hpp"

namespace riscv {

// TODO
// Check if the immediate values are representable by 20 bits
bool validateIntegerInstruction(InstructionNode &node, bool immediate) {
    //a integer instruction needs exactly 3 operands
    if(node.getChildrenCount() != 3) {
        return false;
    }
    //check if all operands are valid themselves
    if(!node.validateAllChildren()) {
        return false;
    }
    //a immediate integer instruction needs two register operands followed by one immediate operand
    if(immediate) {
        return node.requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 2) &&node.requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 2, 1);
    }else {
        //a register integer instruction needs three register operands
        return node.requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 3);
    }
}

bool SubInstructionNode::validate() {
  return validateIntegerInstruction(*this, _immediate);
}

MemoryValue SubInstructionNode::getValue(DummyMemoryAccess &memory_access) {
  // Get the destination register
  std::string dest = _children.at(0)->getIdentifier();

  // Evaluate the operands
  MemoryValue op1 = _children.at(1)->getValue(memory_access);
  MemoryValue op2 = _children.at(2)->getValue(memory_access);

  // TODO Replace this pseudo code by actual implementation
  // MemoryValue sub = op1 - op2;
  // memory_access.storeRegister(dest, sub);

  return MemoryValue{};
}
}
