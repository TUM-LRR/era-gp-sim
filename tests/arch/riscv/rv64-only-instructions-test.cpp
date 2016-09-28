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

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include "gtest/gtest.h"

#include "arch/riscv/immediate-node-factory.hpp"

#include "dummies.hpp"

using namespace riscv;

namespace {

/**
 * Tests Register-Immediate instructions.
 */
void testRV64OnlyInstructionRI(const std::string& instructionName,
                               uint64_t op1,
                               uint64_t op2,
                               uint64_t expectedResult) {
  // Set up registers
  FakeRegister dest, src;
  std::string destId{"dest"}, srcId{"src"};
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(destId, dest);
  memoryAccess.addRegister(srcId, src);

  memoryAccess.setRegisterValue(srcId, convertToMem<uint64_t>(op1));

  // Set up instruction
  auto instrFactory     = setUpFactory({"rv32i", "rv64i"});
  auto immediateFactory = ImmediateNodeFactory{};
  auto instr            = instrFactory.createInstructionNode(instructionName);

  // Fill instruction with arguments
  ASSERT_TRUE(instr);
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(destId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(srcId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(
      immediateFactory.createImmediateNode(convertToMem<uint64_t>(op2)));
  ASSERT_TRUE(instr->validate());

  // Execute
  ASSERT_EQ(instr->getValue(memoryAccess), MemoryValue{});

  // Check result
  MemoryValue result = memoryAccess.getRegisterValue(destId);
  ASSERT_EQ(convertToInt<uint64_t>(result), expectedResult);
}

/**
 * Tests Register-Register instructions.
 */
void testRV64OnlyInstructionRR(const std::string& instructionName,
                               uint64_t op1,
                               uint64_t op2,
                               uint64_t expectedResult) {
  // Set up registers
  FakeRegister dest, src1, src2;
  std::string destId{"dest"}, src1Id{"src1"}, src2Id{"src2"};
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(destId, dest);
  memoryAccess.addRegister(src1Id, src1);
  memoryAccess.addRegister(src2Id, src2);

  memoryAccess.setRegisterValue(src1Id, convertToMem<uint64_t>(op1));
  memoryAccess.setRegisterValue(src2Id, convertToMem<uint64_t>(op2));

  // Set up instruction
  auto instrFactory     = setUpFactory({"rv32i", "rv64i"});
  auto immediateFactory = ImmediateNodeFactory{};
  auto instr            = instrFactory.createInstructionNode(instructionName);

  // Fill instruction with arguments
  ASSERT_TRUE(instr);
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(destId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(src1Id));
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(src2Id));
  ASSERT_TRUE(instr->validate());

  // Execute
  ASSERT_EQ(instr->getValue(memoryAccess), MemoryValue{});

  // Check result
  MemoryValue result = memoryAccess.getRegisterValue(destId);
  ASSERT_EQ(convertToInt<uint64_t>(result), expectedResult);
}

/**
 * Tests both Register-Immediate & Register-Register instructions.
 */
void testRV64OnlyInstructions(const std::string& instructionNameRI,
                              const std::string& instructionNameRR,
                              uint64_t op1,
                              uint64_t op2,
                              uint64_t expectedResult) {
  testRV64OnlyInstructionRI(instructionNameRI, op1, op2, expectedResult);
  testRV64OnlyInstructionRR(instructionNameRR, op1, op2, expectedResult);
}

}// Private namespace

TEST(RV64OnlyInstructionsTest, Add) {
  std::string ri = "addiw";
  std::string rr = "addw";

  // What was 1+1 again?
  testRV64OnlyInstructions(ri, rr, 1, 1, 2);
  testRV64OnlyInstructions(ri, rr, 42, 1, 43);
  testRV64OnlyInstructions(ri, rr, 1, 42, 43);
  // Check if upper 32 bits are ignored
  testRV64OnlyInstructions(ri, rr, (uint64_t{1} << 32) + 1, 1, 2);
  // Check if overflow is ignored
  testRV64OnlyInstructions(ri, rr, UINT32_MAX, 1, 0);
  // Check if sign extension works
  testRV64OnlyInstructions(ri, rr, UINT32_MAX, 0, UINT64_MAX);
}

TEST(RV64OnlyInstructionsTest, Sub) {
  std::string rr = "subw";

  // What was 1-1 again?
  testRV64OnlyInstructionRR(rr, 1, 1, 0);
  // Check if uppper 32 bits are ignored
  testRV64OnlyInstructionRR(rr, (uint64_t{1} << 32) + 1, 1, 0);
  // Check if overflow is ignored
  testRV64OnlyInstructionRR(rr, uint64_t{1} << 31, 1, INT32_MAX);
  // Check if sign extension works
  testRV64OnlyInstructionRR(rr, -1, 0, UINT64_MAX);
}

TEST(RV64OnlyInstructionsTest, Sll) {
  std::string ri = "slliw";
  std::string rr = "sllw";

  // Nothing special
  testRV64OnlyInstructions(ri, rr, 1, 31, uint64_t{1} << 31);
  // Check if only lower 5 bits of shift amount are used
  // That means, that nothing will happen, if shift amount is
  // bigger than 31
  testRV64OnlyInstructions(ri, rr, 1, 32, 1);
}

TEST(RV64OnlyInstructionsTest, Srl) {
  std::string ri = "srliw";
  std::string rr = "srlw";

  // Nothing special
  testRV64OnlyInstructions(ri, rr, uint64_t{1} << 31, 31, 1);
  // Check if only lower 5 bits of shift amount are used
  // That means, that nothing will happen, if shift amount is
  // bigger than 31
  testRV64OnlyInstructions(ri, rr, 1, 32, 1);
}

TEST(RV64OnlyInstructionsTest, Sra) {
  std::string ri = "sraiw";
  std::string rr = "sraw";

  // Nothing special
  testRV64OnlyInstructions(ri, rr, 4, 2, 1);
  // Check if only lower 5 bits of shift amount are used
  // That means, that nothing will happen, if shift amount is
  // bigger than 31
  testRV64OnlyInstructions(ri, rr, 4, 32, 4);
  // Check if arithmetic shift works
  testRV64OnlyInstructions(ri, rr, -4, 2, UINT32_MAX);// max because thats -1
}
