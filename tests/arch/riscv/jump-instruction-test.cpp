/* ERASIM Assembler Interpreter
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
#include "common/assert.hpp"

#include "tests/arch/riscv/base-fixture.hpp"

// Need to put into namespace so that `FRIEND_TEST` works
namespace riscv {

struct JumpInstructionTest : public riscv::BaseFixture {
  using address_t = std::uint32_t;
  using offset_t = std::int32_t;
  using super = riscv::BaseFixture;

  JumpInstructionTest() : progress(0) {
    setInitialAddress(666);
  }

  void setInitialAddress(address_t address) {
    initialAddress = address;
    riscv::storeRegister(getMemoryAccess(), "pc", initialAddress);
  }

  void jal(const std::string& linkRegisterName, offset_t jumpOffsetValue) {
    auto instruction = factories.createInstructionNode("JAL");
    auto linkRegister = factories.createRegisterNode(linkRegisterName);

    auto memoryValue = riscv::convert(jumpOffsetValue);
    auto jumpOffset = factories.createImmediateNode(memoryValue);

    instruction->addChild(std::move(linkRegister));
    instruction->addChild(std::move(jumpOffset));

    auto& memoryAccess = getMemoryAccess();

    assert::gtest(instruction->validate(memoryAccess).isSuccess());

    MemoryValue newPc = instruction->getValue(memoryAccess);
    memoryAccess.putRegisterValue("pc", newPc);
  }

  void testJAL(const std::string& linkRegisterName, offset_t offset) {
    jal(linkRegisterName, offset);

    auto& memoryAccess = getMemoryAccess();

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

  void jalr(const std::string& linkRegisterName,
            const std::string& baseRegisterName,
            offset_t jumpOffsetValue) {
    auto instruction = factories.createInstructionNode("JALR");
    auto linkRegister = factories.createRegisterNode(linkRegisterName);
    auto baseRegister = factories.createRegisterNode(baseRegisterName);

    auto memoryValue = riscv::convert(jumpOffsetValue);
    auto jumpOffset = factories.createImmediateNode(memoryValue);

    instruction->addChild(std::move(linkRegister));
    instruction->addChild(std::move(baseRegister));
    instruction->addChild(std::move(jumpOffset));

    auto& memoryAccess = getMemoryAccess();

    MemoryValue newPc = instruction->getValue(memoryAccess);
    memoryAccess.putRegisterValue("pc", newPc);
  }

  void testJALR(const std::string& linkRegisterName,
                const std::string& baseRegisterName,
                address_t baseAddress,
                offset_t offset) {
    auto& memoryAccess = getMemoryAccess();
    riscv::storeRegister(memoryAccess, baseRegisterName, baseAddress);

    jalr(linkRegisterName, baseRegisterName, offset);

    auto resultingProgramCounter =
        riscv::loadRegister<address_t>(memoryAccess, "pc");

    // JALR specifies an absolute target, while JAL specifies a target
    // relative to the current program counter. Also, the offset is now in
    // multiples of one, not two. The lowest bit must be unset.
    address_t target = (baseAddress + offset) & ~1;
    EXPECT_EQ(resultingProgramCounter, target);

    auto resultingLinkRegister =
        riscv::loadRegister<address_t>(memoryAccess, linkRegisterName);
    EXPECT_EQ(resultingLinkRegister, initialAddress + 4);

    // Store for next invocation
    initialAddress = target;
  }

  address_t initialAddress;
  address_t progress;
};

TEST_F(JumpInstructionTest, JALValidation) {
  using JumpAndLinkNode =
      AbstractJumpAndLinkInstructionNode<riscv::unsigned32_t,
                                         riscv::signed32_t>;

  // Typical instruction: JAL r1, -123 ; <link>, <20 bit offset>

  auto instruction = factories.createInstructionNode("JAL");
  auto linkRegister = factories.createRegisterNode("x1");
  auto invalidRegister = factories.createRegisterNode("x3");


  // For internal tests
  auto internal = dynamic_cast<JumpAndLinkNode*>(instruction.get());

  assert::gtest(internal != nullptr);

  // First create an invalid target (i.e. greater than 19 bit)
  MemoryValue target = riscv::convert(0xFFFFF);
  auto targetImmediate = factories.createImmediateNode(target);

  auto& memoryAccess = getMemoryAccess();

  EXPECT_FALSE(instruction->validate(memoryAccess).isSuccess());

  instruction->addChild(std::move(linkRegister));
  EXPECT_FALSE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_FALSE(instruction->validate(memoryAccess).isSuccess());

  // Invalid second register node, just to ensure
  // node type validation (not just number of children)
  instruction->addChild(std::move(invalidRegister));
  EXPECT_TRUE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_FALSE(internal->_validateOperandTypes().isSuccess());
  EXPECT_FALSE(instruction->validate(memoryAccess).isSuccess());

  // Now add the immediate node so that the type
  // checks pass, but the offset still begin invalid
  instruction->setChild(1, std::move(targetImmediate));
  EXPECT_TRUE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_TRUE(internal->_validateOperandTypes().isSuccess());
  EXPECT_FALSE(internal->_validateOffset(memoryAccess).isSuccess());
  EXPECT_FALSE(instruction->validate(memoryAccess).isSuccess());

  // This should work now
  targetImmediate = factories.createImmediateNode(riscv::convert(-0x80000));
  instruction->setChild(1, std::move(targetImmediate));
  EXPECT_TRUE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_TRUE(internal->_validateOperandTypes().isSuccess());
  EXPECT_TRUE(internal->_validateOffset(memoryAccess).isSuccess());
  EXPECT_TRUE(instruction->validate(memoryAccess).isSuccess());

  // TODO: Validate that the resulting PC will be valid
}

TEST_F(JumpInstructionTest, JALCanJumpForwards) {
  testJAL("x1", 2);
  testJAL("x2", 124);
  testJAL("x3", 12346);
  testJAL("x4", 0x7FFFF -3 - initialAddress - progress);
}

TEST_F(JumpInstructionTest, JALCanJumpBackwards) {
  // Swap sides
  setInitialAddress(std::numeric_limits<address_t>::max() - initialAddress);

  testJAL("x1", -2);
  testJAL("x2", -124);
  testJAL("x3", -1236);
  testJAL("x4", -0x8000 +3 + initialAddress + progress);
}

TEST_F(JumpInstructionTest, JALDoesNothingWhenOffsetIsZero) {
  testJAL("x1", 0);
}

TEST_F(JumpInstructionTest, JALRValidation) {
  using JumpAndLinkNode =
      AbstractJumpAndLinkInstructionNode<riscv::unsigned32_t,
                                         riscv::signed32_t>;

  // Typical instruction: JALR r1, r2, -123 ; <link> <base> <12s bit offset>
  auto instruction = factories.createInstructionNode("JALR");
  auto linkRegister = factories.createRegisterNode("x1");
  auto baseRegister = factories.createRegisterNode("x2");
  auto invalidRegister = factories.createRegisterNode("x3");

  // For internal tests
  auto internal = dynamic_cast<JumpAndLinkNode*>(instruction.get());

  assert::gtest(internal != nullptr);

  // First create an invalid target (i.e. greater than 11 bit)
  MemoryValue target = riscv::convert(0xFFF);
  auto targetImmediate = factories.createImmediateNode(target);

  auto& memoryAccess = getMemoryAccess();

  EXPECT_FALSE(instruction->validate(memoryAccess).isSuccess());

  instruction->addChild(std::move(linkRegister));
  instruction->addChild(std::move(baseRegister));
  EXPECT_FALSE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_FALSE(instruction->validate(memoryAccess).isSuccess());

  // Invalid third register node, just to ensure
  // node type validation (not just number of children)
  instruction->addChild(std::move(invalidRegister));
  EXPECT_TRUE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_FALSE(internal->_validateOperandTypes().isSuccess());
  EXPECT_FALSE(instruction->validate(memoryAccess).isSuccess());

  // Now add the immediate node so that the type
  // checks pass, but the offset still is invalid
  instruction->setChild(2, std::move(targetImmediate));
  EXPECT_TRUE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_TRUE(internal->_validateOperandTypes().isSuccess());
  EXPECT_FALSE(internal->_validateOffset(memoryAccess).isSuccess());
  EXPECT_FALSE(instruction->validate(memoryAccess).isSuccess());

  // This should work now
  targetImmediate = factories.createImmediateNode(riscv::convert(0x7FF));
  instruction->setChild(2, std::move(targetImmediate));
  EXPECT_TRUE(internal->_validateNumberOfChildren().isSuccess());
  EXPECT_TRUE(internal->_validateOperandTypes().isSuccess());
  EXPECT_TRUE(internal->_validateOffset(memoryAccess).isSuccess());
  EXPECT_TRUE(instruction->validate(memoryAccess).isSuccess());
}

TEST_F(JumpInstructionTest, JALRCanJumpForwards) {
  testJALR("x1", "x2", 1, 1);
  testJALR("x2", "x3", 6969, 123);
  testJALR("x3", "x4", 8888, 1234);
  testJALR("x4", "x1", 1, 0x7FF);
}

TEST_F(JumpInstructionTest, JALRCanJumpBackwards) {
  testJALR("x1", "x2", 1, -1);
  testJALR("x2", "x3", 6969, -123);
  testJALR("x3", "x4", 8888, -1234);
  testJALR("x4", "x1", 1, -0x7FF);
}

TEST_F(JumpInstructionTest, JALRDoesNothingWhenOffsetIsZero) {
  testJALR("x1", "x2", 0, 0);
}
}
