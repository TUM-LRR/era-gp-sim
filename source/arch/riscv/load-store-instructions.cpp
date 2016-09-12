#include "arch/riscv/load-store-instructions.hpp"

using namespace riscv;

const ValidationResult LoadInstructionNode::validate() const {
  if (!requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 2) ||
      !requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 2, 1)) {
    return ValidationResult::fail(QT_TRANSLATE_NOOP(
        "Syntax-Tree-Validation",
        "Load instructions must have 2 registers and 1 immediate"));
  }

  // TODO Switch-case through the types and validate
  // the size of the offset respectively

  return ValidationResult::success();
}

const ValidationResult StoreInstructionNode::validate() const {
  if (!requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 1, 2) ||
      !requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 3, 1)) {
    return ValidationResult::fail(QT_TRANSLATE_NOOP(
        "Syntax-Tree-Validation",
        "Store instructions must have 2 registers and 1 immediate"));
  }

  // TODO Switch-case through the types and validate
  // the size of the offset respectively

  return ValidationResult::success();
}

MemoryValue
LoadInstructionNode::getValue(DummyMemoryAccess &memory_access) const {
  std::string dest   = _children.at(0)->getIdentifier();
  MemoryValue base   = _children.at(1)->getValue(memory_access);
  MemoryValue offset = _children.at(2)->getValue(memory_access);

  // TODO Replace this with actual implementation
  // TODO Ensure the correct amount of bytes are loaded from memory
  return MemoryValue{};
}

MemoryValue
StoreInstructionNode::getValue(DummyMemoryAccess &memory_access) const {
  MemoryValue base   = _children.at(0)->getValue(memory_access);
  MemoryValue src    = _children.at(1)->getValue(memory_access);
  MemoryValue offset = _children.at(2)->getValue(memory_access);

  // TODO Replace this with actual implementation
  // memory_access.putMemory(base + offset, src);
  // TODO Ensure the correct amount of bytes are written into memory
  return MemoryValue{};
}
