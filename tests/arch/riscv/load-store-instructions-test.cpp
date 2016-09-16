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
#include "arch/riscv/load-store-instructions.hpp"

#include "dummies.hpp"

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

namespace {

/**
 * Performs a unsigned load.
 * \tparam T The unsigned integral of the value to be loaded (e.g. uint16_t for
 *         lwu)
 * \tparam W The word size of the architecture (e.g. uint32_t/uint64_t)
 * \param testValue The integer value to test
 * \param modules The modules that should be passed to the architecture forumla
 * \param instructionName The instruction to test (e.g. "lw")
 * \param byteAmount The amount of bytes that are loaded by the instruction
 */
template <typename T, typename W>
typename std::enable_if<
    std::is_integral<T>::value && std::is_unsigned<T>::value &&
        std::is_integral<W>::value && std::is_unsigned<W>::value,
    void>::type
performLoadTestUnsigned(T testValue,
                        ArchitectureFormula::InitializerList modules,
                        std::string instructionName,
                        size_t byteAmount) {
  FakeRegister dest, base;
  std::string destId{"dest"}, baseId{"base"};
  MemoryValue loadValue;
  loadValue = convertToMem<T>(testValue);

  DummyMemoryAccessImpl memoryAccess;
  // Add registers
  memoryAccess.addRegister(destId, dest);
  memoryAccess.addRegister(baseId, base);
  // Fill registers & memory
  memoryAccess.setRegisterValue(baseId, convertToMem<W>(0));
  memoryAccess.setMemoryValueAt(0, loadValue);

  auto instrFactory     = setUpFactory(modules);
  auto immediateFactory = ImmediateNodeFactory{};
  auto instr            = instrFactory.createInstructionNode(instructionName);

  // Fill the instructions operands
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(destId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(baseId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(immediateFactory.createImmediateNode(convertToMem<W>(0)));
  ASSERT_TRUE(instr->validate());

  // Execute the instruction
  instr->getValue(memoryAccess);

  // Check result
  ASSERT_EQ(memoryAccess.getRegisterValue(destId), loadValue);
}

/**
 * Performs a signed load. See performLoadTestUnsigned for parameter
 * documentation.
 * \tparam T The signed integral of the value to be loaded.
 */
template <typename T, typename W>
typename std::enable_if<
    std::is_integral<T>::value && std::is_signed<T>::value &&
        std::is_integral<W>::value && std::is_unsigned<W>::value,
    void>::type
performLoadTestSigned(T testValue,
                      ArchitectureFormula::InitializerList modules,
                      std::string instructionName,
                      size_t byteAmount) {
  FakeRegister dest, base;
  std::string destId{"dest"}, baseId{"base"};
  MemoryValue loadValue = convertToMemSigned<T>(testValue);

  DummyMemoryAccessImpl memoryAccess;
  // Add registers
  memoryAccess.addRegister(destId, dest);
  memoryAccess.addRegister(baseId, base);
  // Fill registers & memory
  memoryAccess.setRegisterValue(baseId, convertToMem<W>(0));
  memoryAccess.setMemoryValueAt(0, loadValue);

  auto instrFactory     = setUpFactory(modules);
  auto immediateFactory = ImmediateNodeFactory{};
  auto instr            = instrFactory.createInstructionNode(instructionName);

  // Fill the instructions operands
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(destId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(baseId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(immediateFactory.createImmediateNode(convertToMem<W>(0)));
  ASSERT_TRUE(instr->validate());

  // Execute the instruction
  instr->getValue(memoryAccess);

  // Check result
  ASSERT_EQ(memoryAccess.getRegisterValue(destId), loadValue);
}

/**
 * Performs a store.
 * \tparam T The unsigned integral of the value to be stored.
 * \param testValue The integer value to be stored into memory.
 * \param modules The modules to be passed to the architecture formula.
 * \param instructionName The name of the store instruction to be tested.
 * \param byteAmount The amount of bytes, that are stored into memory by
 *        the specified instruction.
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value &&
                            std::is_unsigned<T>::value,
                        void>::type
performStoreTest(T testValue,
                 ArchitectureFormula::InitializerList modules,
                 std::string instructionName,
                 size_t byteAmount) {
  FakeRegister base, src;
  std::string baseId{"base"}, srcId{"src"};
  MemoryValue storeValue = convertToMem<T>(testValue);

  DummyMemoryAccessImpl memoryAccess;
  // Add registers
  memoryAccess.addRegister(baseId, base);
  memoryAccess.addRegister(srcId, src);
  // Fill registers
  memoryAccess.setRegisterValue(baseId, convertToMem<T>(0));
  memoryAccess.setRegisterValue(srcId, storeValue);

  auto instrFactory     = setUpFactory(modules);
  auto immediateFactory = ImmediateNodeFactory{};
  auto instr            = instrFactory.createInstructionNode(instructionName);

  // Fill the instructions operands
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(baseId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(srcId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(immediateFactory.createImmediateNode(convertToMem<T>(0)));
  ASSERT_TRUE(instr->validate());

  // Execute the instruction
  instr->getValue(memoryAccess);

  // Check result
  ASSERT_EQ(memoryAccess.getMemoryValueAt(0, byteAmount), storeValue);
}
}

// Load Tests

TEST(LoadStoreInstructionsTest, LoadDoubleWord) {
  performLoadTestUnsigned<uint64_t, uint64_t>(
      0x0123456789ABCDEF, {"rv32i", "rv64i"}, "ld", 8);
}

TEST(LoadStoreInstructionsTest, LoadWord) {
  performLoadTestUnsigned<uint32_t, uint32_t>(0xDEADBEEF, {"rv32i"}, "lw", 4);
  performLoadTestUnsigned<uint32_t, uint64_t>(
      0xDEADBEEF, {"rv32i", "rv64i"}, "lwu", 4);
  performLoadTestSigned<int32_t, uint64_t>(
      -123456789, {"rv32i", "rv64i"}, "lw", 4);
}

TEST(LoadStoreInstructionsTest, LoadHalfWord) {
  performLoadTestUnsigned<uint16_t, uint32_t>(0xF00D, {"rv32i"}, "lhu", 2);
  performLoadTestUnsigned<uint16_t, uint64_t>(
      0xF00D, {"rv32i", "rv64i"}, "lhu", 2);
  performLoadTestSigned<int16_t, uint32_t>(-22353, {"rv32i"}, "lh", 2);
  performLoadTestSigned<int16_t, uint64_t>(-22353, {"rv32i", "rv64i"}, "lh", 2);
}

TEST(LoadStoreInstructionsTest, LoadByte) {
  performLoadTestUnsigned<uint8_t, uint32_t>(0x99, {"rv32i"}, "lbu", 1);
  performLoadTestUnsigned<uint8_t, uint64_t>(
      0x99, {"rv32i", "rv64i"}, "lbu", 1);
  performLoadTestSigned<int8_t, uint32_t>(-42, {"rv32i"}, "lb", 1);
  performLoadTestSigned<int8_t, uint64_t>(-42, {"rv32i", "rv64i"}, "lb", 1);
}

// Store Tests

TEST(LoadStoreInstructionsTest, StoreDoubleWord) {
  performStoreTest<uint64_t>(0xC0CAC01AADD511FE, {"rv32i", "rv64i"}, "sd", 8);
}

TEST(LoadStoreInstructionsTest, StoreWord) {
  performStoreTest<uint32_t>(0xBADC0DED, {"rv32i"}, "sw", 4);
  performStoreTest<uint32_t>(0xBADC0DED, {"rv32i", "rv64i"}, "sw", 4);
}

TEST(LoadStoreInstructionsTest, StoreHalfWord) {
  performStoreTest<uint16_t>(0xBABE, {"rv32i"}, "sh", 2);
  performStoreTest<uint16_t>(0xBABE, {"rv32i", "rv64i"}, "sh", 2);
}

TEST(LoadStoreInstructionsTest, StoreByte) {
  performStoreTest<uint8_t>(0x42, {"rv32i"}, "sb", 1);
  performStoreTest<uint8_t>(0x42, {"rv32i", "rv64i"}, "sb", 1);
}

