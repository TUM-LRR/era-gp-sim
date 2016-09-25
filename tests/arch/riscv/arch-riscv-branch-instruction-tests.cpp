/* C++ Assembler Interpreter
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <cassert>
#include <memory>
#include <unordered_map>

#include "gtest/gtest.h"

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/immediate-node.hpp"
#include "arch/riscv/abstract-branch-instruction-node.hpp"
#include "arch/riscv/abstract-branch-instruction-node.hpp"
#include "arch/riscv/control-flow-instructions.hpp"

namespace riscv {

struct TestBranchInstructions : public ::testing::Test {
  using address_t = std::uint32_t;
  using offset_t = std::int32_t;

  using RelationMap = std::unordered_map<
      std::string,
      std::function<bool(riscv::signed32_t, riscv::signed32_t)>>;

  static RelationMap relationMap;

  TestBranchInstructions() : progress(0) {
    auto architecture = Architecture::Brew({"riscv", {"rv32i"}});
    factory = architecture.getNodeFactories();

    setInitialAddress(6'666'666);
  }

  void setInitialAddress(address_t address) {
    initialAddress = address;
    riscv::storeRegister(memoryAccess, "pc", initialAddress);
  }

  template <typename T>
  void branch(const std::string& mnemonic,
              const T& firstRegisterName,
              const T& secondRegisterName,
              offset_t offset) {
    auto instruction = factory.createInstructionNode(mnemonic);

    auto firstRegister = factory.createRegisterNode(firstRegisterName);
    auto secondRegister = factory.createRegisterNode(secondRegisterName);

    auto offsetMemory = riscv::convert(offset);
    auto offsetImmediate = factory.createImmediateNode(offsetMemory);

    instruction->addChild(std::move(firstRegister));
    instruction->addChild(std::move(secondRegister));
    instruction->addChild(std::move(offsetImmediate));

    // assert::that
    assert(instruction->validate().isSuccess());

    instruction->getValue(memoryAccess);
  }

  template <typename T = riscv::signed32_t>
  void testBranch(const std::string& mnemonic,
                  const T& firstValue,
                  const T& secondValue,
                  offset_t offset) {
    riscv::storeRegister(memoryAccess, "r1", firstValue);
    riscv::storeRegister(memoryAccess, "r2", secondValue);

    branch(mnemonic, "r1", "r2", offset);

    auto resultingProgramCounter =
        riscv::loadRegister<address_t>(memoryAccess, "pc");
    // Check if the branch should have been taken
    if (relationMap[mnemonic](firstValue, secondValue)) {
      // Remember: offset is in multiples of two
      // progress is necessary if/because we call
      // testJAL multiple times in a fixture
      progress += offset * 2;
    }

    EXPECT_EQ(resultingProgramCounter, initialAddress + progress);
  }

  template <typename... Args>
  void testUnsignedBranch(Args&&... args) {
    testBranch<riscv::signed32_t>(std::forward<Args>(args)...);
  }

  NodeFactoryCollection factory;
  MemoryAccess memoryAccess;
  address_t initialAddress;
  address_t progress;
};

// clang-format off
TestBranchInstructions::RelationMap TestBranchInstructions::relationMap = {
    {"beq", [](auto first, auto second) { return first == second; }},
    {"bne", [](auto first, auto second) { return first != second; }},
    {"blt", [](auto first, auto second) { return first < second; }},
    {"bge", [](auto first, auto second) { return first >= second; }},
    {"bltu", [](auto first, auto second) { return first < second; }},
    {"bgeu", [](auto first, auto second) { return first >= second; }}
};
// clang-format off

TEST_F(TestBranchInstructions, TestValidation) {
  using BranchNode =
      AbstractBranchInstructionNode<riscv::unsigned32_t, riscv::signed32_t>;

  // Typical instruction: BEQ r1, r2, 123 ; <first> <second> <12 bit offset>

  auto architecture = Architecture::Brew({"riscv", {"rv32i"}});
  auto& factory = architecture.getNodeFactories();

  auto instruction = factory.createInstructionNode("BEQ");
  auto firstRegister = factory.createRegisterNode("r1");
  auto secondRegister = factory.createRegisterNode("r2");
  auto invalidRegister = factory.createRegisterNode("r3");

  // For internal tests
  auto internal = dynamic_cast<BranchNode*>(instruction.get());
  // assert::test
  assert(internal != nullptr);

  // First create an invalid target (i.e. greater than 11 bit)
  MemoryValue target = riscv::convert(0xFFF);
  auto targetImmediate = factory.createImmediateNode(target);

  EXPECT_FALSE(instruction->validate().isSuccess());

  instruction->addChild(std::move(firstRegister));
  instruction->addChild(std::move(secondRegister));
  EXPECT_FALSE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_FALSE(instruction->validate().isSuccess());

  // Invalid third register node, just to ensure
  // node type validation (not just number of children)
  instruction->addChild(std::move(invalidRegister));
  EXPECT_TRUE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_FALSE(internal->_validateOperandTypes().isSuccess());
  EXPECT_FALSE(instruction->validate().isSuccess());

  // Now add the immediate node so that the type
  // checks pass, but the offset still begin invalid
  instruction->setChild(2, std::move(targetImmediate));
  EXPECT_TRUE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_TRUE(internal->_validateOperandTypes().isSuccess());
  EXPECT_FALSE(internal->_validateOffset().isSuccess());
  EXPECT_FALSE(instruction->validate().isSuccess());

  // This should work now
  targetImmediate = factory.createImmediateNode(riscv::convert(-0x800));
  instruction->setChild(2, std::move(targetImmediate));
  EXPECT_TRUE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_TRUE(internal->_validateOperandTypes().isSuccess());
  EXPECT_TRUE(internal->_validateOffset().isSuccess());
  EXPECT_TRUE(instruction->validate().isSuccess());
}

TEST_F(TestBranchInstructions, TestBEQIsTakenWhenShould) {
  testBranch<>("beq", 1, 1, 123);
  testBranch<>("beq", 0xDEAD, 0xDEAD, 0x7FF);
  testBranch<>("beq", 0xFF, 0xFF, -0x800);
}

TEST_F(TestBranchInstructions, TestBEQIsNotTakenWhenShouldnt) {
  testBranch<>("beq", 1, 2, 123);
  testBranch<>("beq", 0xDEAD, 0xAEAD, 0x7FF);
  testBranch<>("beq", 0xFF, 0xFFFFF, -0x800);
}

TEST_F(TestBranchInstructions, TestBNEIsTakenWhenShould) {
  testBranch<>("bne", 1, 2, 123);
  testBranch<>("bne", 0xDEAD, 0xAEAD, 0x7FF);
  testBranch<>("bne", 0xFF, 0xFFFFF, -0x800);
}

TEST_F(TestBranchInstructions, TestBNEIsNotTakenWhenShouldnt) {
  testBranch<>("bne", 1, 1, 123);
  testBranch<>("bne", 0xDEAD, 0xDEAD, 0x7FF);
  testBranch<>("bne", 0xFF, 0xFF, -0x800);
}

TEST_F(TestBranchInstructions, TestBLTIsTakenWhenShould) {
  testBranch<>("blt", -1, 2, 123);
  testBranch<>("blt", -0xDEAD, 0xDEAD, 0x7FF);
  testBranch<>("blt", -0xFFF, -0xFF, -0x800);
}

TEST_F(TestBranchInstructions, TestBLTIsNotTakenWhenShouldnt) {
  testBranch<>("blt", -1, -1, 123);
  testBranch<>("blt", -0xBEEF, -0xDEAD, 0x7FF);
  testBranch<>("blt", 0xFF, -0xFFFF, -0x800);
}

TEST_F(TestBranchInstructions, TestBGEIsTakenWhenShould) {
  testBranch<>("bge", -1, -1, 123);
  testBranch<>("bge", 0xFFF, 0xFFF, -0x800);
  testBranch<>("bge", 0xDEAD, -0xDEAD, 0x7FF);
  testBranch<>("bge", -0xFF, -0xFF, -0x800);
}

TEST_F(TestBranchInstructions, TestBGEIsNotTakenWhenShouldnt) {
  testBranch<>("bge", -1, 2, 123);
  testBranch<>("bge", -0xBEEF, 0xDEAD, 0x7FF);
  // testBranch<>("bge", 0xFF, 0xFFFE, -0x800);
}

TEST_F(TestBranchInstructions, TestBLTUIsTakenWhenShould) {
  testUnsignedBranch("bltu", 1, 2, 123);
  testUnsignedBranch("bltu", 0xBEEF, 0xDEAD, 0x7FF);
  testUnsignedBranch("bltu", 0xFF, 0xFFFFF, -0x800);
}

TEST_F(TestBranchInstructions, TestBLTUIsNotTakenWhenShouldnt) {
  testUnsignedBranch("bltu", 1, 1, 123);
  testUnsignedBranch("bltu", 0xDEAD, 0xBEEF, 0x7FF);
  testUnsignedBranch("bltu", 0xFFFF, 0xFF, -0x800);
}


TEST_F(TestBranchInstructions, TestBGEUIsTakenWhenShould) {
  testUnsignedBranch("bltu", 1, 1, 123);
  testUnsignedBranch("bltu", 0xFFF, 0xFFF, -0x800);
  testUnsignedBranch("bltu", 0xDEAD, 0xBEEF, 0x7FF);
  testUnsignedBranch("bltu", 0xFFF, 0xFF, -0x800);
}

TEST_F(TestBranchInstructions, TestBGEUIsNotTakenWhenShouldnt) {
  testUnsignedBranch("bgeu", 1, 2, 123);
  testUnsignedBranch("bgeu", 0xBEEF, 0xDEAD, 0x7FF);
  testUnsignedBranch("bgeu", 0xFF, 0xFFFF, -0x800);
}

}
