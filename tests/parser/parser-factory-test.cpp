#include "gtest/gtest.h"

#include "parser/parser-factory.hpp"

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"

TEST(ParserFactory, CreateBadParser) {
  ParserPtr parser{ParserFactory::createParser(Architecture{}, "doesnt_exist")};
  ASSERT_EQ(parser, nullptr);
}

TEST(ParserFactory, CreateRiscVParser) {
  Architecture arch{
      Architecture::Brew(ArchitectureFormula{"riscv", {"rv32i"}})};
  ParserPtr parser{ParserFactory::createParser(arch, "riscv")};
}
