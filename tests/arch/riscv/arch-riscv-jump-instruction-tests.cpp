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

#include <memory>

#include "gtest/gtest.h"

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/immediate-node.hpp"
#include "arch/riscv/abstract-jump-instruction-node.hpp"
#include "arch/riscv/control-flow-instructions.hpp"

namespace riscv {

struct TestJumpInstructionsFixture : public ::testing::Test {
  using address_t = std::uint32_t;
  using offset_t = std::int32_t;

  TestJumpInstructionsFixture() : progress(0) {
    setInitialAddress(666);
  }

  void setInitialAddress(address_t address) {
    initialAddress = address;
    riscv::storeRegister(memoryAccess, "pc", initialAddress);
  }

  void JAL(const std::string& linkRegisterName, offset_t jumpOffsetValue) {
    auto architecture = Architecture::Brew({"riscv", {"rv32i"}});
    auto& factory = architecture.getNodeFactories();

    auto instruction = factory.createInstructionNode("JAL");
    auto linkRegister = factory.createRegisterNode(linkRegisterName);

    auto memoryValue = riscv::convert(jumpOffsetValue);
    auto jumpOffset = factory.createImmediateNode(memoryValue);

    instruction->addChild(std::move(linkRegister));
    instruction->addChild(std::move(jumpOffset));

    memoryAccess.addRegister(linkRegisterName);

    // assert::test
    assert(instruction->validate().isSuccess());

    instruction->getValue(memoryAccess);
  }

  void testJAL(const std::string& linkRegisterName, offset_t offset) {
    JAL(linkRegisterName, offset);

    auto resultingLinkRegister =
        riscv::loadRegister<std::uint32_t>(memoryAccess, linkRegisterName);
    EXPECT_EQ(resultingLinkRegister, initialAddress + progress + 4);

    auto resultingPC = riscv::loadRegister<std::uint32_t>(memoryAccess, "pc");

    // Remember: offset is in multiples of two
    // progress is necessary if/because we call
    // testJAL multiple times in a fixture
    progress += offset * 2;
    EXPECT_EQ(resultingPC, initialAddress + progress);
  }

  void JALR(const std::string& linkRegisterName,
            const std::string& baseRegisterName,
            offset_t jumpOffsetValue) {
    auto architecture = Architecture::Brew({"riscv", {"rv32i"}});
    auto& factory = architecture.getNodeFactories();

    auto instruction = factory.createInstructionNode("JALR");
    auto linkRegister = factory.createRegisterNode(linkRegisterName);
    auto baseRegister = factory.createRegisterNode(baseRegisterName);

    auto memoryValue = riscv::convert(jumpOffsetValue);
    auto jumpOffset = factory.createImmediateNode(memoryValue);

    instruction->addChild(std::move(linkRegister));
    instruction->addChild(std::move(baseRegister));
    instruction->addChild(std::move(jumpOffset));

    memoryAccess.addRegister(linkRegisterName);

    instruction->getValue(memoryAccess);
  }

  void testJALR(const std::string& linkRegisterName,
                const std::string& baseRegisterName,
                address_t baseAddress,
                offset_t offset) {
    riscv::storeRegister(memoryAccess, baseRegisterName, baseAddress);

    JALR(linkRegisterName, baseRegisterName, offset);

    auto resultingPC = riscv::loadRegister<std::uint32_t>(memoryAccess, "pc");

    // JALR specifies an absolute target, while JAL specifies a target
    // relative to the current program counter. Also, the offset is now in
    // multiples of one, not two. The lowest bit must be unset.
    address_t target = (baseAddress + offset) & ~1;
    EXPECT_EQ(resultingPC, target);

    auto resultingLinkRegister =
        riscv::loadRegister<std::uint32_t>(memoryAccess, linkRegisterName);
    EXPECT_EQ(resultingLinkRegister, initialAddress + 4);

    // Store for next invocation
    initialAddress = target;
  }

  MemoryAccess memoryAccess;
  address_t initialAddress;
  address_t progress;
};

TEST(TestJumpInstructions, TestJALValidation) {
  using JumpAndLinkNode =
      AbstractJumpAndLinkInstructionNode<riscv::unsigned32_t,
                                         riscv::signed32_t>;

  // Typical instruction: JAL r1, -123 ; <link>, <20 bit offset>

  auto architecture = Architecture::Brew({"riscv", {"rv32i"}});
  auto& factory = architecture.getNodeFactories();

  auto instruction = factory.createInstructionNode("JAL");
  auto linkRegister = factory.createRegisterNode("r1");
  auto invalidRegister = factory.createRegisterNode("r3");

  // For internal tests
  auto internal = dynamic_cast<JumpAndLinkNode*>(instruction.get());
  // assert::test
  assert(internal != nullptr);

  // First create an invalid target (i.e. greater than 19 bit)
  MemoryValue target = riscv::convert(0xFFFFF);
  auto targetImmediate = factory.createImmediateNode(target);

  EXPECT_FALSE(instruction->validate().isSuccess());

  instruction->addChild(std::move(linkRegister));
  EXPECT_FALSE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_FALSE(instruction->validate().isSuccess());

  // Invalid second register node, just to ensure
  // node type validation (not just number)
  instruction->addChild(std::move(invalidRegister));
  EXPECT_TRUE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_FALSE(internal->_validateOperandTypes().isSuccess());
  EXPECT_FALSE(instruction->validate().isSuccess());

  // Now add the immediate node so that the type
  // checks pass, but the offset still begin invalid
  instruction->setChild(1, std::move(targetImmediate));
  EXPECT_TRUE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_TRUE(internal->_validateOperandTypes().isSuccess());
  EXPECT_FALSE(internal->_validateOffset().isSuccess());
  EXPECT_FALSE(instruction->validate().isSuccess());

  // This should work now
  targetImmediate = factory.createImmediateNode(riscv::convert(-0x80000));
  instruction->setChild(1, std::move(targetImmediate));
  EXPECT_TRUE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_TRUE(internal->_validateOperandTypes().isSuccess());
  EXPECT_TRUE(internal->_validateOffset().isSuccess());
  EXPECT_TRUE(instruction->validate().isSuccess());

  // TODO: Validate that the resulting PC will be valid
}

TEST_F(TestJumpInstructionsFixture, TestJALCanJumpForwards) {
  testJAL("r1", 1);
  testJAL("r2", 123);
  testJAL("r3", 12345);
  testJAL("r4", 0x7FFFF - initialAddress - progress);
}

TEST_F(TestJumpInstructionsFixture, TestJALCanJumpBackwards) {
  // Swap sides
  setInitialAddress(std::numeric_limits<address_t>::max() - initialAddress);

  testJAL("r1", -1);
  testJAL("r2", -123);
  testJAL("r3", -1235);
  testJAL("r4", -0x8000 + initialAddress + progress);
}

TEST_F(TestJumpInstructionsFixture, TestJALDoesNothingWhenOffsetIsZero) {
  testJAL("r1", 0);
}

TEST(TestJumpInstructions, TestJALRValidation) {
  using JumpAndLinkNode =
      AbstractJumpAndLinkInstructionNode<riscv::unsigned32_t,
                                         riscv::signed32_t>;

  // Typical instruction: JALR r1, r2, -123 ; <link> <base> <20 bit offset>

  auto architecture = Architecture::Brew({"riscv", {"rv32i"}});
  auto& factory = architecture.getNodeFactories();

  auto instruction = factory.createInstructionNode("JALR");
  auto linkRegister = factory.createRegisterNode("r1");
  auto baseRegister = factory.createRegisterNode("r2");
  auto invalidRegister = factory.createRegisterNode("r3");

  // For internal tests
  auto internal = dynamic_cast<JumpAndLinkNode*>(instruction.get());
  // assert::test
  assert(internal != nullptr);

  // First create an invalid target (i.e. greater than 11 bit)
  MemoryValue target = riscv::convert(0xFFF);
  auto targetImmediate = factory.createImmediateNode(target);

  EXPECT_FALSE(instruction->validate().isSuccess());

  instruction->addChild(std::move(linkRegister));
  instruction->addChild(std::move(baseRegister));
  EXPECT_FALSE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_FALSE(instruction->validate().isSuccess());

  // Invalid third register node, just to ensure
  // node type validation (not just number)
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
  targetImmediate = factory.createImmediateNode(riscv::convert(0x7FF));
  instruction->setChild(2, std::move(targetImmediate));
  EXPECT_TRUE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_TRUE(internal->_validateOperandTypes().isSuccess());
  EXPECT_TRUE(internal->_validateOffset().isSuccess());
  EXPECT_TRUE(instruction->validate().isSuccess());
}

TEST_F(TestJumpInstructionsFixture, TestJALRCanJumpForwards) {
  testJALR("r1", "r2", 1, 1);
  testJALR("r2", "r3", 6969, 123);
  testJALR("r3", "r4", 8888, 12345);
  testJALR("r4", "r1", 1, 0x7FF);
}

TEST_F(TestJumpInstructionsFixture, TestJALRCanJumpBackwards) {
  testJALR("r1", "r2", 1, -1);
  testJALR("r2", "r3", 6969, -123);
  testJALR("r3", "r4", 8888, -12345);
  testJALR("r4", "r1", 1, -0x7FF);
}

TEST_F(TestJumpInstructionsFixture, TestJALRDoesNothingWhenOffsetIsZero) {
  testJALR("r1", "r2", 0, 0);
}
}
