/*
* C++ Assembler Interpreter
* Copyright (C) 2016 Chair of Computer Architecture
* at Technical University of Munich
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>
#include <algorithm>
#include <vector>

#include "arch/common/architecture-brewery.hpp"
#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/instruction-key.hpp"
#include "arch/common/instruction-set.hpp"
#include "arch/common/register-information.hpp"

struct ArchDeserializationTestFixture : public ::testing::Test {
  ArchDeserializationTestFixture() {
    // clang-format off
    instructionSet.addInstructions(InstructionSet({
      {"add", InstructionKey({{"opcode", 6}, {"function", 3}})},
      {"sub", InstructionKey({{"opcode", 9}, {"function", 3}})}
    }));
    // clang-format on

    // clang-format off
    r0 = RegisterInformation()
          .id(0)
          .name("r0")
          .size(32)
          .addConstituents({{1,1}, {2,2}})
          .addAlias("zero");
    r1 = RegisterInformation()
          .id(1)
          .name("r1")
          .size(16)
          .type(RegisterInformation::Type::FLOAT)
          .enclosing(0)
          .constant(3.14)
          .addAliases({"foo", "bar"});
    // clang-format on

    unitInformation.name("cpu").addRegisters({r0, r1});
    units.emplace_back(unitInformation);
  }

  InstructionSet instructionSet;
  RegisterInformation r0;
  RegisterInformation r1;
  UnitInformation unitInformation;
  std::vector<UnitInformation> units;
};

TEST_F(ArchDeserializationTestFixture, TestBaseWithoutDependencies) {
  ArchitectureFormula formula("test", {"no-deps"});

  auto architecture = ArchitectureBrewery(formula).brew();

  EXPECT_TRUE(architecture.isValid());
  EXPECT_EQ(architecture.getName(), "test");
  EXPECT_EQ(architecture.getEndianness(), Architecture::Endianness::LITTLE);
  EXPECT_EQ(architecture.getAlignmentBehavior(),
            Architecture::AlignmentBehavior::STRICT);
  EXPECT_EQ(architecture.getWordSize(), 32);
  EXPECT_EQ(architecture.getSignedRepresentation(),
            Architecture::SignedRepresentation::SIGN_BIT);

  EXPECT_EQ(architecture.getInstructions(), instructionSet);

  auto expected = architecture.getUnits();
  for (auto& unit : units) {
    auto iterator = expected.find(unit);
    ASSERT_NE(iterator, expected.end());
    EXPECT_EQ(*iterator, unit);
  }
}

TEST_F(ArchDeserializationTestFixture, TestBaseWithBasicDependencies) {
  ArchitectureFormula formula("test", {"with-deps-basic"});

  auto architecture = ArchitectureBrewery(formula).brew();

  EXPECT_TRUE(architecture.isValid());
  EXPECT_EQ(architecture.getName(), "test");
  EXPECT_EQ(architecture.getEndianness(), Architecture::Endianness::LITTLE);
  EXPECT_EQ(architecture.getAlignmentBehavior(),
            Architecture::AlignmentBehavior::RELAXED);
  EXPECT_EQ(architecture.getWordSize(), 32);
  EXPECT_EQ(architecture.getSignedRepresentation(),
            Architecture::SignedRepresentation::SIGN_BIT);

  // clang-format off
  instructionSet.addInstruction({"sll", InstructionKey({
      {"opcode", 6},
      {"function", 6},
      {"width", 6}
  })});
  // clang-format on

  EXPECT_EQ(architecture.getInstructions(), instructionSet);

  auto expected = architecture.getUnits();
  for (auto& unit : units) {
    auto iterator = expected.find(unit);
    ASSERT_NE(iterator, expected.end());
    EXPECT_EQ(*iterator, unit);
  }
}

TEST_F(ArchDeserializationTestFixture, TestBaseWithComplexDependenciesNoReset) {
  ArchitectureFormula formula("test", {"with-deps-complex"});

  auto architecture = ArchitectureBrewery(formula).brew();

  EXPECT_TRUE(architecture.isValid());
  EXPECT_EQ(architecture.getName(), "test");
  EXPECT_EQ(architecture.getEndianness(), Architecture::Endianness::LITTLE);
  EXPECT_EQ(architecture.getAlignmentBehavior(),
            Architecture::AlignmentBehavior::RELAXED);
  EXPECT_EQ(architecture.getWordSize(), 32);
  EXPECT_EQ(architecture.getSignedRepresentation(),
            Architecture::SignedRepresentation::SIGN_BIT);

  // clang-format off
  instructionSet.addInstruction({"sll", InstructionKey({
      {"opcode", 6},
      {"function", 6},
      {"width", 6}
  })});
  // clang-format on

  ASSERT_EQ(architecture.getInstructions(), instructionSet);

  // clang-format off
  auto f0 = RegisterInformation()
    .name("f0")
    .id(13)
    .size(64)
    .type(RegisterInformation::Type::PROGRAM_COUNTER)
    .addAlias("pc");
  // clang-format on

  units.push_back({"fpu", {f0}});

  f0.name("r5");
  units[0].addRegister(f0);

  auto expected = architecture.getUnits();
  for (auto& unit : units) {
    auto iterator = expected.find(unit);
    ASSERT_NE(iterator, expected.end());
    EXPECT_EQ(*iterator, unit);
  }
}
