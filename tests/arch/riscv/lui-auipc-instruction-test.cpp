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

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/riscv/immediate-node-factory.hpp"
#include "arch/riscv/instruction-node-factory.hpp"
#include "arch/riscv/lui-auipc-instructions.hpp"
#include "tests/arch/riscv/base-fixture.hpp"

using namespace riscv;

struct LuiAuipcInstructionTest : public riscv::BaseFixture {
  LuiAuipcInstructionTest() : destId("x1"), pcId("pc") {
  }

  std::string destId, pcId;


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
  std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value>
  performLuiTest(int32_t input,
                 T expectedOutput,
                 ArchitectureFormula::InitializerList modules) {
    loadArchitecture(modules, 10);

    auto& memoryAccess = getMemoryAccess();

    // Create factory & instruction
    auto instr = factories.createInstructionNode("lui");

    // Fill arguments
    ASSERT_FALSE(instr->validate(memoryAccess));
    instr->addChild(factories.createRegisterNode(destId));
    ASSERT_FALSE(instr->validate(memoryAccess));
    instr->addChild(
        factories.createImmediateNode(riscv::convert<int32_t>(input)));
    ASSERT_TRUE(instr->validate(memoryAccess));

    // Execute
    instr->getValue(memoryAccess);

    // Check result
    auto result = memoryAccess.getRegisterValue(destId).get();
    ASSERT_EQ(riscv::convert<T>(result), expectedOutput);
  }


  /**
   * Tests the auipc instruction.
   *
   * \tparam T The unsigned word size of the architecture. The result of the
   * auipc
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
    loadArchitecture(modules, 10);

    auto& memoryAccess = getMemoryAccess();
    // Set pc to a value
    memoryAccess.putRegisterValue(pcId, riscv::convert<T>(initialPc));

    // Create factory & instruction
    auto instr = factories.createInstructionNode("auipc");

    // Fill arguments
    ASSERT_FALSE(instr->validate(memoryAccess));
    instr->addChild(factories.createRegisterNode(destId));
    ASSERT_FALSE(instr->validate(memoryAccess));
    instr->addChild(
        factories.createImmediateNode(riscv::convert<uint32_t>(input)));
    ASSERT_TRUE(instr->validate(memoryAccess));

    // Execute
    instr->getValue(memoryAccess);

    // Check result
    MemoryValue result = memoryAccess.getRegisterValue(destId).get();
    ASSERT_EQ(riscv::convert<T>(result), expectedOutput);
  }
};

TEST_F(LuiAuipcInstructionTest, Validation) {
  loadArchitecture({"rv32i"});
  auto& memoryAccess = getMemoryAccess();

  auto lui = factories.createInstructionNode("lui");
  auto auipc = factories.createInstructionNode("auipc");

  ASSERT_FALSE(lui->validate(memoryAccess) || auipc->validate(memoryAccess));
  // Just add some dummy registers
  lui->addChild(factories.createRegisterNode(destId));
  auipc->addChild(factories.createRegisterNode(destId));
  ASSERT_FALSE(lui->validate(memoryAccess) || auipc->validate(memoryAccess));
  // Add an immediate value, that can't be represented by 20 bits
  lui->addChild(
      factories.createImmediateNode(riscv::convert<uint32_t>(1 << 20)));
  // Add a valid immediate
  auipc->addChild(factories.createImmediateNode(riscv::convert<uint32_t>(1)));
  ASSERT_FALSE(lui->validate(memoryAccess));
  ASSERT_TRUE(auipc->validate(memoryAccess));
  // Add another immediate (which is not allowed)
  auipc->addChild(factories.createImmediateNode(riscv::convert<uint32_t>(1)));
  ASSERT_FALSE(auipc->validate(memoryAccess));
}

TEST_F(LuiAuipcInstructionTest, LUI_32) {
  performLuiTest<uint32_t>(1, 1 << 12, {"rv32i"});
  performLuiTest<uint32_t>(0xFFFFF, 0xFFFFF << 12, {"rv32i"});
}

TEST_F(LuiAuipcInstructionTest, LUI_64) {
  // Test without sign expansion
  performLuiTest<uint64_t>(1, 1 << 12, {"rv32i", "rv64i"});
  // Test with sign expansion
  performLuiTest<uint64_t>(0x81234, 0xFFFFFFFF81234 << 12, {"rv32i", "rv64i"});
}

TEST_F(LuiAuipcInstructionTest, AUIPC_32) {
  performAuipcTest<uint32_t>(1, 1, (1 << 12) + 1, {"rv32i"});
  performAuipcTest<uint32_t>(0xFFFFF, 0x00000FFF, 0xFFFFFFFF, {"rv32i"});
}

TEST_F(LuiAuipcInstructionTest, AUIPC_64) {
  // Test without sign expansion
  performAuipcTest<uint64_t>(1, 1, (1 << 12) + 1, {"rv32i", "rv64i"});
  // Test with sign expansion
  performAuipcTest<uint64_t>(
      0x81234, 1, (0xFFFFFFFF81234 << 12) + 1, {"rv32i", "rv64i"});
}
