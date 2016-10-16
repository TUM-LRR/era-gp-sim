#include "gtest/gtest.h"

#include "parser/parser-factory.hpp"

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"

static void createBadParser() {
  ParserPtr parser{ParserFactory::createParser(Architecture{}, "doesnt_exist")};
}

TEST(ParserFactory, CreateBadParser) {
  ASSERT_THROW(createBadParser(), assert::AssertionError);
}

TEST(ParserFactory, CreateRiscVParser) {
  Architecture arch{
      Architecture::Brew(ArchitectureFormula{"riscv", {"rv32i"}})};
  ParserPtr parser{ParserFactory::createParser(arch, "riscv")};
}
