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

    if(immediate && node.getChild(2).getType() == AbstractSyntaxTreeNode::Type::IMMEDIATE) {
        //check if immediate operand is represented by only 20 bits
        DummyMemoryAccessStub stub;
        MemoryValue value = node.getChild(2).getValue(stub);//no memory access is needed for a immediate node
        auto bits20 = value.getValue() & (~2097151); //2097151 = 0b11111...1 (20 times a 1) -> erase lower 20 bits
        if(bits20 != 0) {
            //there is a 1 somewhere in bit 20 to x => the value is not represented by only bit 0...19
            return false;
        }
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
