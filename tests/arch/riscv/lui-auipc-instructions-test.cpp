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

#include <iostream>
#include <memory>
#include <string>

#include "gtest/gtest.h"

#include "arch/riscv/immediate-node-factory.hpp"
#include "arch/riscv/lui-auipc-instructions.hpp"

#include "dummies.hpp"

using namespace riscv;

namespace {

/**
 * Tests the lui instruction.
 *
 * \tparam T The unsigned word size of the architecture. The result of the lui
 *         operation must be of that type.
 * \param input The input to the lui instruction (passed as immediate value)
 * \param expectedOutput The expected output of the lui instruction
 * \param modules The modules to be passed to the architecture formula
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value &&
                            std::is_unsigned<T>::value,
                        void>::type
performLuiTest(uint32_t input,
               T expectedOutput,
               ArchitectureFormula::InitializerList modules) {
  // Init memory access & registers
  FakeRegister dest;
  std::string destId{"dest"};
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(destId, dest);

  // Create factory & instruction
  auto instrFactory     = setUpFactory(modules);
  auto immediateFactory = ImmediateNodeFactory{};
  auto instr            = instrFactory.createInstructionNode("lui");

  // Fill arguments
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(destId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(
      immediateFactory.createImmediateNode(convertToMem<uint32_t>(input)));
  ASSERT_TRUE(instr->validate());

  // Execute
  instr->getValue(memoryAccess);

  // Check result
  MemoryValue result = memoryAccess.getRegisterValue(destId);
  ASSERT_EQ(convertToInt<T>(result), expectedOutput);
}

/**
 * Tests the auipc instruction.
 *
 * \tparam T The unsigned word size of the architecture. The result of the auipc
 *         operation must be of that type
 * \param input The input to the auipc operation (passed as immediate)
 * \param initialPc The initial value of the pc (program counter)
 * \param expectedOutput The expected value of the destination register
 * \param modules The modules passed to the architecture formula
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value &&
                            std::is_unsigned<T>::value,
                        void>::type
performAuipcTest(uint32_t input,
                 T initialPc,
                 T expectedOutput,
                 ArchitectureFormula::InitializerList modules) {
  // Init memory access & registers
  // Note that the program counter is realized by a register named pc
  FakeRegister dest, pc;
  std::string destId{"dest"}, pcId{"pc"};

  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(destId, dest);
  memoryAccess.addRegister(pcId, pc);

  // Set pc to a value
  memoryAccess.setRegisterValue(pcId, convertToMem<T>(initialPc));

  // Create factory & instruction
  auto instrFactory     = setUpFactory(modules);
  auto immediateFactory = ImmediateNodeFactory{};
  auto instr            = instrFactory.createInstructionNode("auipc");

  // Fill arguments
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(destId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(
      immediateFactory.createImmediateNode(convertToMem<uint32_t>(input)));
  ASSERT_TRUE(instr->validate());

  // Execute
  instr->getValue(memoryAccess);

  // Check result
  MemoryValue result = memoryAccess.getRegisterValue(destId);
  ASSERT_EQ(convertToInt<T>(result), expectedOutput);
}

}// Private namespace

TEST(LuiAuipcInstructionsTest, Lui32) {
  performLuiTest<uint32_t>(1, 1 << 12, {"rv32i"});
  performLuiTest<uint32_t>(0xFFFFF, 0xFFFFF << 12, {"rv32i"});
}

TEST(LuiAuipcInstructionsTest, Lui64) {
  // Test without sign expansion
  performLuiTest<uint64_t>(1, 1 << 12, {"rv32i", "rv64i"});
  // Test with sign expansion
  performLuiTest<uint64_t>(0x81234, 0xFFFFFFFF81234 << 12, {"rv32i", "rv64i"});
}

TEST(LuiAuipcInstructionsTest, Auipc32) {
  performAuipcTest<uint32_t>(1, 1, (1 << 12) + 1, {"rv32i"});
  performAuipcTest<uint32_t>(0xFFFFF, 0x00000FFF, 0xFFFFFFFF, {"rv32i"});
}

TEST(LuiAuipcInstructionsTest, Auipc64) {
  // Test without sign expansion
  performAuipcTest<uint64_t>(1, 1, (1 << 12) + 1, {"rv32i", "rv64i"});
  // Test with sign expansion
  performAuipcTest<uint64_t>(
      0x81234, 1, (0xFFFFFFFF81234 << 12) + 1, {"rv32i", "rv64i"});
}
