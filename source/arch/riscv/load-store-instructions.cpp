#include "arch/riscv/load-store-instructions.hpp"

using namespace riscv;

ValidationResult LoadInstructionNode::validate() const {
  if (!_requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 0, 2) ||
      !_requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 2, 1)) {
    return ValidationResult::fail(QT_TRANSLATE_NOOP(
        "Syntax-Tree-Validation",
        "Load instructions must have 2 registers and 1 immediate"));
  }

  // TODO Switch-case through the types and validate
  // the size of the offset respectively

  return ValidationResult::success();
}

ValidationResult StoreInstructionNode::validate() const {
  if (!_requireChildren(AbstractSyntaxTreeNode::Type::REGISTER, 1, 2) ||
      !_requireChildren(AbstractSyntaxTreeNode::Type::IMMEDIATE, 3, 1)) {
    return ValidationResult::fail(QT_TRANSLATE_NOOP(
        "Syntax-Tree-Validation",
        "Store instructions must have 2 registers and 1 immediate"));
  }

  // TODO Switch-case through the types and validate
  // the size of the offset respectively

  return ValidationResult::success();
}

MemoryValue
LoadInstructionNode::getValue(MemoryAccess &MemoryAccess) const {
  // auto
  // []
  std::string dest   = _children.at(0)->getIdentifier();
  MemoryValue base   = _children.at(1)->getValue(MemoryAccess);
  MemoryValue offset = _children.at(2)->getValue(MemoryAccess);

  // TODO Replace this with actual implementation
  // TODO Ensure the correct amount of bytes are loaded from memory
  return MemoryValue{};
}

MemoryValue
StoreInstructionNode::getValue(MemoryAccess &MemoryAccess) const {
  MemoryValue base   = _children.at(0)->getValue(MemoryAccess);
  MemoryValue src    = _children.at(1)->getValue(MemoryAccess);
  MemoryValue offset = _children.at(2)->getValue(MemoryAccess);

  // TODO Replace this with actual implementation
  // MemoryAccess.putMemory(base + offset, src);
  // TODO Ensure the correct amount of bytes are written into memory
  return MemoryValue{};
}
