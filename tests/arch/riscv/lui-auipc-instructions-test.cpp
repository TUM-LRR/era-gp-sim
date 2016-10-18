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
#include "tests/arch/riscv/test-utils.hpp"

using namespace riscv;

struct LuiAuipcInstructionsTest : public RiscvBaseTest {

    LuiAuipcInstructionsTest() : destId("x1"), pcId("pc") {}

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
    typename std::enable_if<std::is_integral<T>::value &&
                                std::is_unsigned<T>::value,
                            void>::type
    performLuiTest(int32_t input,
                   T expectedOutput,
                   ArchitectureFormula::InitializerList modules) {
      load(modules, 10);
      MemoryAccess memoryAccess = getMemoryAccess();


      // Create factory & instruction
      auto instrFactory = getFactories();
      auto immediateFactory = ImmediateNodeFactory{};
      auto instr = instrFactory.createInstructionNode("lui");

      // Fill arguments
      ASSERT_FALSE(instr->validate(memoryAccess));
      instr->addChild(std::make_unique<RegisterNode>(destId));
      ASSERT_FALSE(instr->validate(memoryAccess));
      instr->addChild(
          immediateFactory.createImmediateNode(riscv::convert<int32_t>(input)));
      ASSERT_TRUE(instr->validate(memoryAccess));

      // Execute
      instr->getValue(memoryAccess);

      // Check result
      MemoryValue result = memoryAccess.getRegisterValue(destId).get();
      ASSERT_EQ(riscv::convert<T>(result), expectedOutput);
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
      load(modules, 10);

      MemoryAccess memoryAccess = getMemoryAccess();


      // Set pc to a value
      memoryAccess.putRegisterValue(pcId, riscv::convert<T>(initialPc));

      // Create factory & instruction
      auto instrFactory = getFactories();
      auto immediateFactory = ImmediateNodeFactory{};
      auto instr = instrFactory.createInstructionNode("auipc");

      // Fill arguments
      ASSERT_FALSE(instr->validate(memoryAccess));
      instr->addChild(std::make_unique<RegisterNode>(destId));
      ASSERT_FALSE(instr->validate(memoryAccess));
      instr->addChild(
          immediateFactory.createImmediateNode(riscv::convert<uint32_t>(input)));
      ASSERT_TRUE(instr->validate(memoryAccess));

      // Execute
      instr->getValue(memoryAccess);

      // Check result
      MemoryValue result = memoryAccess.getRegisterValue(destId).get();
      ASSERT_EQ(riscv::convert<T>(result), expectedOutput);
    }

};

TEST_F(LuiAuipcInstructionsTest, Validation) {
  load({"rv32i"});
  MemoryAccess memoryAccess = getMemoryAccess();
  auto immediateFactory = ImmediateNodeFactory{};
  auto fact = getFactories();
  auto lui = fact.createInstructionNode("lui");
  auto auipc = fact.createInstructionNode("auipc");

  ASSERT_FALSE(lui->validate(memoryAccess) || auipc->validate(memoryAccess));
  // Just add some dummy registers
  lui->addChild(std::make_unique<RegisterNode>(destId));
  auipc->addChild(std::make_unique<RegisterNode>(destId));
  ASSERT_FALSE(lui->validate(memoryAccess) || auipc->validate(memoryAccess));
  // Add an immediate value, that can't be represented by 20 bits
  lui->addChild(
      immediateFactory.createImmediateNode(riscv::convert<uint32_t>(1 << 20)));
  // Add a valid immediate
  auipc->addChild(
      immediateFactory.createImmediateNode(riscv::convert<uint32_t>(1)));
  ASSERT_FALSE(lui->validate(memoryAccess));
  ASSERT_TRUE(auipc->validate(memoryAccess));
  // Add another immediate (which is not allowed)
  auipc->addChild(
      immediateFactory.createImmediateNode(riscv::convert<uint32_t>(1)));
  ASSERT_FALSE(auipc->validate(memoryAccess));
}

TEST_F(LuiAuipcInstructionsTest, Lui32) {
  performLuiTest<uint32_t>(1, 1 << 12, {"rv32i"});
  performLuiTest<uint32_t>(0xFFFFF, 0xFFFFF << 12, {"rv32i"});
}

TEST_F(LuiAuipcInstructionsTest, Lui64) {
  // Test without sign expansion
  performLuiTest<uint64_t>(1, 1 << 12, {"rv32i", "rv64i"});
  // Test with sign expansion
  performLuiTest<uint64_t>(0x81234, 0xFFFFFFFF81234 << 12, {"rv32i", "rv64i"});
}

TEST_F(LuiAuipcInstructionsTest, Auipc32) {
  performAuipcTest<uint32_t>(1, 1, (1 << 12) + 1, {"rv32i"});
  performAuipcTest<uint32_t>(0xFFFFF, 0x00000FFF, 0xFFFFFFFF, {"rv32i"});
}

TEST_F(LuiAuipcInstructionsTest, Auipc64) {
  // Test without sign expansion
  performAuipcTest<uint64_t>(1, 1, (1 << 12) + 1, {"rv32i", "rv64i"});
  // Test with sign expansion
  performAuipcTest<uint64_t>(
      0x81234, 1, (0xFFFFFFFF81234 << 12) + 1, {"rv32i", "rv64i"});
}
