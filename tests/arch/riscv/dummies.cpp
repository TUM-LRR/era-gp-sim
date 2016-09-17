#include "dummies.hpp"

MemoryValue to32BitMemoryValue(uint32_t value) {
  return convertToMem<uint32_t>(value);
}

MemoryValue to64BitMemoryValue(uint64_t value) {
  return convertToMem<uint64_t>(value);
}

InstructionNodeFactory
setUpFactory(ArchitectureFormula::InitializerList modules =
                 ArchitectureFormula::InitializerList()) {
  auto formula = ArchitectureFormula("riscv", modules);
  auto riscv   = Architecture::Brew(formula);
  return InstructionNodeFactory(riscv.getInstructions(), riscv);
}
