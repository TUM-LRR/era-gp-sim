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
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/datatype-information.hpp"
#include "arch/common/extension-information.hpp"
#include "arch/common/instruction-information.hpp"
#include "arch/common/instruction-key.hpp"
#include "arch/common/instruction-set.hpp"
#include "arch/common/register-information.hpp"
#include "arch/common/unit-information.hpp"

struct ArchCommonTestFixture : ::testing::Test {
  ArchCommonTestFixture() {
    registerInformation.name("r0")
        .id(0)
        .size(32)
        .type(RegisterInformation::Type::INTEGER)
        .constant(5)
        .addAlias("zero")
        .enclosing(1)
        .addConstituents({2, 3, 4});

    unitInformation.name("cpu").addRegister(registerInformation);

    instructionKey.addEntry("opcode", 6).addEntry("function", 9);
    instructionInformation.mnemonic("add").key(instructionKey);

    // clang-format off
    instructionSet.addInstructions(InstructionSet({
        instructionInformation,
        {"mov", InstructionKey({{"opcode", 2}})}
    }));
    // clang-format on

    baseExtensionInformation.name("rvi32");

    baseExtensionInformation.addInstructions(instructionSet);
    baseExtensionInformation.addUnit(unitInformation);

    baseExtensionInformation.wordSize(32);
    baseExtensionInformation.endianness(
        ArchitectureProperties::Endianness::MIXED);
    baseExtensionInformation.alignmentBehavior(
        ArchitectureProperties::AlignmentBehavior::STRICT);

    // clang-format off
    specialExtensionInformation.name("rva32");
    specialExtensionInformation.addInstructions(InstructionSet({
      {"lr", InstructionKey({{"opcode", 1}, {"function", 2}})},
      {"sc", InstructionKey({{"opcode", 3}, {"function", 4}})}
    }));
    // clang-format on
  }

  InstructionKey instructionKey;
  InstructionInformation instructionInformation;
  InstructionSet instructionSet;
  RegisterInformation registerInformation;
  UnitInformation unitInformation;
  ExtensionInformation baseExtensionInformation;
  ExtensionInformation specialExtensionInformation;
};

TEST(ArchCommonTest, TestRegisterInformation) {
  auto registerInformation = RegisterInformation()
                                 .name("r0")
                                 .id(0)
                                 .type(RegisterInformation::Type::INTEGER)
                                 .constant(5)
                                 .addAlias("zero")
                                 .enclosing(1)
                                 .addConstituents({2, 3, 4});

  EXPECT_FALSE(registerInformation.isValid());
  registerInformation.size(32);
  EXPECT_TRUE(registerInformation.isValid());

  EXPECT_EQ(registerInformation.getID(), 0);
  EXPECT_TRUE(registerInformation.hasSize());
  EXPECT_EQ(registerInformation.getSize(), 32);
  EXPECT_EQ(registerInformation.getType(), RegisterInformation::Type::INTEGER);
  EXPECT_TRUE(registerInformation.isConstant());
  EXPECT_EQ(registerInformation.getConstant<std::size_t>(), 5);
  EXPECT_EQ(registerInformation.getAliases(),
            std::vector<std::string>({"zero"}));
  EXPECT_TRUE(registerInformation.hasEnclosing());
  EXPECT_EQ(registerInformation.getEnclosing(), 1);
  EXPECT_EQ(registerInformation.getConstituents(),
            std::vector<std::size_t>({2, 3, 4}));

  EXPECT_FALSE(registerInformation.isSpecial());
  registerInformation.type(RegisterInformation::Type::LINK);
  EXPECT_TRUE(registerInformation.isSpecial());

  // clang-format off
  EXPECT_TRUE(RegisterInformation::isSpecialType(
    registerInformation.getType()
  ));
}

TEST(ArchCommonTest, TestInstructionKey) {
  auto key =
      InstructionKey()
          .addEntry("function", 2)
          .addEntries({{"foo", 3}, {"bar", 4}})
          .addEntries(std::unordered_map<std::string, std::size_t>{{"baz", 5}});

  EXPECT_FALSE(key.isValid());
  key.addEntry("opcode", 1);
  EXPECT_TRUE(key.isValid());

  EXPECT_TRUE(key.hasKey("opcode"));
  EXPECT_TRUE(key.hasKey("function"));
  EXPECT_TRUE(key.hasKey("foo"));
  EXPECT_TRUE(key.hasKey("bar"));
  EXPECT_TRUE(key.hasKey("baz"));
  EXPECT_EQ(key.size(), 5);

  auto keys = key.getKeys();
  std::sort(keys.begin(), keys.end());

  EXPECT_EQ(
      keys,
      std::vector<std::string>({"bar", "baz", "foo", "function", "opcode"}));

  auto values = key.getValues();
  std::sort(values.begin(), values.end());

  EXPECT_EQ(values, std::vector<std::size_t>({1, 2, 3, 4, 5}));

  EXPECT_EQ(key["opcode"], 1);
  EXPECT_EQ(key["function"], 2);
  EXPECT_EQ(key.get("foo"), 3);
  EXPECT_EQ(key.get("bar"), 4);
  EXPECT_EQ(key["baz"], 5);
}

TEST(ArchCommonTest, TestDataTypeInformation) {
  auto dataType = DataTypeInformation("word", 32);

  EXPECT_TRUE(dataType.isValid());
  EXPECT_EQ(dataType.getName(), "word");
  EXPECT_EQ(dataType.getSize(), 32);
}

TEST(ArchCommonTest, TestArchitectureFormula) {
  auto formula = ArchitectureFormula("riscv");

  EXPECT_FALSE(formula.isValid());
  formula.addExtension("rv32i").addExtension("rv32a");
  EXPECT_TRUE(formula.isValid());

  EXPECT_EQ(formula.getArchitectureName(), "riscv");

  auto iterator = formula.cbegin();

  EXPECT_EQ(*iterator++, "rv32i");
  EXPECT_EQ(*iterator, "rv32a");
}

TEST(ArchCommonTest, TestInstructionInformation) {
  auto instruction = InstructionInformation().mnemonic("add");

  InstructionKey key({{"opcode", 6}, {"function", 9}});

  EXPECT_FALSE(instruction.isValid());
  EXPECT_FALSE(instruction.hasKey());
  instruction.key(key);
  EXPECT_TRUE(instruction.isValid());
  EXPECT_TRUE(instruction.hasKey());
  EXPECT_TRUE(instruction.hasMnemonic());

  EXPECT_EQ(instruction.getMnemonic(), "add");
  EXPECT_EQ(instruction.getKey(), key);
}

TEST_F(ArchCommonTestFixture, TestInstructionSet) {
  InstructionSet set;

  EXPECT_TRUE(set.isEmpty());
  set.addInstruction(instructionInformation);
  EXPECT_TRUE(set.isValid());
}

TEST_F(ArchCommonTestFixture, TestUnitInformation) {
  auto unit = UnitInformation().name("cpu");

  EXPECT_FALSE(unit.isValid());
  unit.addRegister(registerInformation);
  EXPECT_TRUE(unit.isValid());
}

TEST_F(ArchCommonTestFixture, TestExtensionInformation) {
  auto extension = ExtensionInformation().name("rvi32");

  EXPECT_FALSE(extension.isComplete());

  extension.addInstructions(instructionSet);
  extension.addUnit(unitInformation);

  EXPECT_TRUE(extension.isValid());
  EXPECT_FALSE(extension.isComplete());

  extension.wordSize(32);
  extension.endianness(ArchitectureProperties::Endianness::MIXED);
  extension.alignmentBehavior(
      ArchitectureProperties::AlignmentBehavior::STRICT);

  EXPECT_TRUE(extension.isValid());
  EXPECT_TRUE(extension.isComplete());


  EXPECT_EQ(extension.getName(), "rvi32");
  EXPECT_EQ(extension.getInstructions(), instructionSet);

  ASSERT_TRUE(extension.getUnits().count(unitInformation));
  EXPECT_EQ(*extension.getUnits().find(unitInformation), unitInformation);

  EXPECT_EQ(extension.getWordSize(), 32);
  EXPECT_EQ(extension.getEndianness(),
            ArchitectureProperties::Endianness::MIXED);
  EXPECT_EQ(extension.getAlignmentBehavior(),
            ArchitectureProperties::AlignmentBehavior::STRICT);
}

TEST_F(ArchCommonTestFixture, TestExtensionInformationMerging) {
  ASSERT_TRUE(baseExtensionInformation.isComplete());
  ASSERT_TRUE(specialExtensionInformation.isValid());

  auto extension = baseExtensionInformation + specialExtensionInformation;

  // Same as for the base-only test
  EXPECT_EQ(extension.getName(), "rvi32");

  ASSERT_TRUE(extension.getUnits().count(unitInformation));
  EXPECT_EQ(*extension.getUnits().find(unitInformation), unitInformation);

  EXPECT_EQ(extension.getWordSize(), 32);
  EXPECT_EQ(extension.getEndianness(),
            ArchitectureProperties::Endianness::MIXED);
  EXPECT_EQ(extension.getAlignmentBehavior(),
            ArchitectureProperties::AlignmentBehavior::STRICT);

  // This should now include the new instructions
  instructionSet += specialExtensionInformation.getInstructions();
  EXPECT_EQ(extension.getInstructions(), instructionSet);
}

TEST_F(ArchCommonTestFixture, TestArchitecture) {
  Architecture architecture("riscv");

  EXPECT_FALSE(architecture.isValid());

  architecture.extendBy(baseExtensionInformation);
  architecture.extendBy(specialExtensionInformation);

  EXPECT_TRUE(architecture.isValid());

  architecture.validate();

  EXPECT_EQ(architecture.getName(), "riscv");

  ASSERT_TRUE(architecture.getUnits().count(unitInformation));
  EXPECT_EQ(*architecture.getUnits().find(unitInformation), unitInformation);

  EXPECT_EQ(architecture.getWordSize(), 32);
  EXPECT_EQ(architecture.getEndianness(),
            ArchitectureProperties::Endianness::MIXED);
  EXPECT_EQ(architecture.getAlignmentBehavior(),
            ArchitectureProperties::AlignmentBehavior::STRICT);

  instructionSet += specialExtensionInformation.getInstructions();
  EXPECT_EQ(architecture.getInstructions(), instructionSet);
}
