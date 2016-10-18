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
#include "arch/common/immediate-node.hpp"
#include "arch/common/register-node.hpp"
#include "arch/riscv/immediate-node-factory.hpp"
#include "arch/riscv/load-store-instructions.hpp"

#include "test-utils.hpp"

/*
 * BIG TODO:
 * Currently these tests operate on a dummy implementation of the
 * DummyMemoryAccess. In addition, the memory value & conversions don't
 * work as expected.
 *
 * Some issues of this tests are:
 * * The immediate offset of the load/store instructions is always set
 *   to 0. This is done, to archieve better results with the dummy memory
 *   access.
 * * All the test numbers are selected, to have no 0-byte. This is due
 *   to issues with the conversions/memory values: For example if you
 *   convert a 1 using uint32_t, a memory value of size 8 will be returned,
 *   as 1 has three 0-bytes in the 32 bit representation.
 */

using namespace riscv;

class LoadStoreInstructionsTest : public RiscvBaseTest {
 public:
  LoadStoreInstructionsTest() : dest("x1"), base("x2"), src("x3") {
  }
  std::string dest, base, src;
};

/**
 * A makro testing the Load instruction.
 * \param contextNbr A number to identify each test
 * \param convertFunction A function that converts numbers into memory values.
 *        It is supposed to convert into the architectures word size.
 * \param instructionName The name of the instruction to test
 * \param testValue The value to be loaded from memory
 * \param expectedResult The expected value of the register, after load has
 *        been performed
 * \param byteAmount Amount of bytes, loaded from memory.
 */
#define TEST_LOAD(contextNbr,                                            \
                  convertFunction,                                       \
                  instructionName,                                       \
                  testValue,                                             \
                  expectedResult,                                        \
                  byteAmount)                                            \
  MemoryValue loadValue_##contextNbr = convertFunction(testValue);       \
  memoryAccess.putRegisterValue(base, convertFunction(0));               \
  memoryAccess.setMemoryValueAt(                                         \
      0, loadValue_##contextNbr.subSet(0, byteAmount* BITS_PER_BYTE));   \
  auto instr_##contextNbr =                                              \
      getFactories().createInstructionNode(instructionName);             \
  ASSERT_FALSE(instr_##contextNbr->validate(memoryAccess));              \
  instr_##contextNbr->addChild(getFactories().createRegisterNode(dest)); \
  ASSERT_FALSE(instr_##contextNbr->validate(memoryAccess));              \
  instr_##contextNbr->addChild(getFactories().createRegisterNode(base)); \
  ASSERT_FALSE(instr_##contextNbr->validate(memoryAccess));              \
  instr_##contextNbr->addChild(                                          \
      getFactories().createImmediateNode(convertFunction(0)));           \
  ASSERT_TRUE(instr_##contextNbr->validate(memoryAccess));               \
  instr_##contextNbr->getValue(memoryAccess);                            \
  ASSERT_EQ(memoryAccess.getRegisterValue(dest).get(),                   \
            convertFunction(expectedResult));

/**
 * A makro testing the Store instruction.
 * \param contextNbr A number to identify each test
 * \param convertFunction A function that converts numbers into memory values.
 *        It is supposed to convert into the architectures word size.
 * \param instructionName The name of the instruction to test
 * \param testValue The value to be stored into memory
 * \param byteAmount The amount of bytes, stored into memory
 */
#define TEST_STORE(                                                      \
    contextNbr, convertFunction, instructionName, testValue, byteAmount) \
  MemoryValue storeValue_##contextNbr = convertFunction(testValue);      \
  memoryAccess.putRegisterValue(base, convertFunction(0));               \
  memoryAccess.setRegisterValue(src, storeValue_##contextNbr);           \
  auto instr_##contextNbr =                                              \
      getFactories().createInstructionNode(instructionName);             \
  ASSERT_FALSE(instr_##contextNbr->validate(memoryAccess));              \
  instr_##contextNbr->addChild(getFactories().createRegisterNode(base)); \
  ASSERT_FALSE(instr_##contextNbr->validate(memoryAccess));              \
  instr_##contextNbr->addChild(getFactories().createRegisterNode(src));  \
  ASSERT_FALSE(instr_##contextNbr->validate(memoryAccess));              \
  instr_##contextNbr->addChild(                                          \
      getFactories().createImmediateNode(convertFunction(0)));           \
  ASSERT_TRUE(instr_##contextNbr->validate(memoryAccess));               \
  instr_##contextNbr->getValue(memoryAccess);                            \
  ASSERT_EQ(memoryAccess.getMemoryValueAt(0, byteAmount).get(),          \
            storeValue_##contextNbr.subSet(0, byteAmount* BITS_PER_BYTE));

// Load Tests

TEST_F(LoadStoreInstructionsTest, Load_RV32I) {
  load({"rv32i"});
  auto memoryAccess = getMemoryAccess();

  TEST_LOAD(0, riscv::convert<uint32_t>, "lw", 0xDEADBEEF, 0xDEADBEEF, 4);

  TEST_LOAD(1, riscv::convert<uint32_t>, "lh", 0xF00D, 0xFFFFF00D, 2);
  TEST_LOAD(2, riscv::convert<uint32_t>, "lh", 0x1, 0x1, 2);
  TEST_LOAD(3, riscv::convert<uint32_t>, "lhu", 0xF00D, 0xF00D, 2);

  TEST_LOAD(4, riscv::convert<uint32_t>, "lb", 0x80, 0xFFFFFF80, 1);
  TEST_LOAD(5, riscv::convert<uint32_t>, "lb", 0x01, 0x01, 1);
  TEST_LOAD(6, riscv::convert<uint32_t>, "lbu", 0x80, 0x80, 1);
}

TEST_F(LoadStoreInstructionsTest, Load_RV64_I) {
  load({"rv32i", "rv64i"});
  auto memoryAccess = getMemoryAccess();

  // clang-format off
  TEST_LOAD(0, riscv::convert<uint64_t>, "ld", 0x0123456789ABCDEF, 0x0123456789ABCDEF, 8);

  TEST_LOAD(1, riscv::convert<uint64_t>, "lw", 0x80000000, 0xFFFFFFFF80000000, 4);
  TEST_LOAD(2, riscv::convert<uint64_t>, "lw", 0x70000000, 0x70000000, 4);
  TEST_LOAD(3, riscv::convert<uint64_t>, "lwu", 0x80000000, 0x80000000, 4);
  // clang-format on

  TEST_LOAD(4, riscv::convert<uint64_t>, "lh", 0xF00D, 0xFFFFFFFFFFFFF00D, 2);
  TEST_LOAD(5, riscv::convert<uint64_t>, "lh", 0x1, 0x1, 2);
  TEST_LOAD(6, riscv::convert<uint64_t>, "lhu", 0xF00D, 0xF00D, 2);

  TEST_LOAD(7, riscv::convert<uint64_t>, "lb", 0x80, 0xFFFFFFFFFFFFFF80, 1);
  TEST_LOAD(8, riscv::convert<uint64_t>, "lb", 0x01, 0x01, 1);
  TEST_LOAD(9, riscv::convert<uint64_t>, "lbu", 0x80, 0x80, 1);
}

// Store Tests

TEST_F(LoadStoreInstructionsTest, Store_RV32I) {
  load({"rv32i"});
  auto memoryAccess = getMemoryAccess();

  TEST_STORE(0, riscv::convert<uint32_t>, "sw", 0xBADC0DED, 4);
  TEST_STORE(1, riscv::convert<uint32_t>, "sh", 0xBABE, 2);
  TEST_STORE(2, riscv::convert<uint32_t>, "sb", 0x42, 1);
}

TEST_F(LoadStoreInstructionsTest, Store_RV64I) {
  load({"rv32i", "rv64i"});
  auto memoryAccess = getMemoryAccess();

  TEST_STORE(0, riscv::convert<uint64_t>, "sd", 0xC0CAC01AADD511FE, 8);
  TEST_STORE(1, riscv::convert<uint64_t>, "sw", 0xBADC0DED, 4);
  TEST_STORE(2, riscv::convert<uint64_t>, "sh", 0xBABE, 2);
  TEST_STORE(3, riscv::convert<uint64_t>, "sb", 0x42, 1);
}
