#include "gtest/gtest.h"

#include "parser/parser-factory.hpp"

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "core/memory-access.hpp"
#include "core/project-module.hpp"

class ParserFactoryFixture : public ::testing::Test {
 public:
  ParserFactoryFixture()
  : formula(ArchitectureFormula{"riscv", {"rv32i"}})
  , projectModule(ArchitectureFormula{"riscv", {"rv32i"}}, 4096, "riscv")
  , memoryAccess(projectModule.getMemoryAccess()) {
    ArchitectureAccess archAccess = projectModule.getArchitectureAccess();
    arch = archAccess.getArchitecture().get();
  }
  ArchitectureFormula formula;
  ProjectModule projectModule;
  MemoryAccess memoryAccess;
  Architecture arch;
};

static void createBadParser(MemoryAccess& memoryAccess) {
  ParserPtr parser{ParserFactory::createParser(
      Architecture{}, memoryAccess, "doesnt_exist")};
}

TEST_F(ParserFactoryFixture, CreateBadParser) {
  ASSERT_THROW(createBadParser(memoryAccess), assert::AssertionError);
}

TEST_F(ParserFactoryFixture, CreateRiscVParser) {
  Architecture arch{
      Architecture::Brew(ArchitectureFormula{"riscv", {"rv32i"}})};
  ParserPtr parser{ParserFactory::createParser(arch, memoryAccess, "riscv")};
}
