#include "arch/riscv/integer-instructions.hpp"

using namespace riscv;

// TODO
// Check if the immediate values are representable by 20 bits

bool riscv::validateIntegerInstruction(InstructionNode &node,
                                       bool immediate) {
  return immediate
             ? node.requireChildren(NodeType::REGISTER, 0, 2) &&
                   node.requireChildren(NodeType::IMMEDIATE, 2, 1)
             : node.requireChildren(NodeType::REGISTER, 0, 3);
}

// validate functions
bool AddInstructionNode::validate() {
  return validateIntegerInstruction(*this, _immediate);
}

bool SubInstructionNode::validate() {
  return validateIntegerInstruction(*this, _immediate);
}

// getValue functions
MemoryValue AddInstructionNode::getValue(DummyMemoryAccess &memory_access) {
  // Get the destination register
  std::string dest = _children.at(0)->getIdentifier();

  // Evaluate the operands
  MemoryValue op1 = _children.at(1)->getValue(memory_access);
  MemoryValue op2 = _children.at(2)->getValue(memory_access);

  // TODO Replace this pseudo code by actual implementation
  // MemoryValue sum = op1 + op2;
  // memory_access.storeRegister(dest, sum);

  return MemoryValue{};
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
