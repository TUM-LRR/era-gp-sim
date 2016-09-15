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

using namespace riscv;

namespace {

template <typename T>
void performLoadTest(T testValue,
                     ArchitectureFormula::InitializerList modules,
                     std::string instructionName,
                     size_t byteAmount) {
  FakeRegister dest, base;
  std::string destId{"dest"}, baseId{"base"};
  MemoryValue loadValue = convertToMem<T>(testValue);

  DummyMemoryAccessImpl memoryAccess;
  // Add registers
  memoryAccess.addRegister(destId, dest);
  memoryAccess.addRegister(baseId, base);
  // Fill registers & memory
  memoryAccess.setRegisterValue(baseId, convertToMem<T>(0));
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
  instr->addChild(immediateFactory.createImmediateNode(convertToMem<T>(0)));
  ASSERT_TRUE(instr->validate());

  // Execute the instruction
  instr->getValue(memoryAccess);

  // Check result
  std::cout << instructionName << memoryAccess.getRegisterValue(destId)
            << std::endl;
  ASSERT_EQ(memoryAccess.getRegisterValue(destId), loadValue);
}

template <typename T>
void performStoreTest(T testValue,
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

// Load Tests (unsigned)

TEST(LoadStoreInstructionsTest, LoadWord) {
  performLoadTest<uint32_t>(0xDEADBEEF, {"rv32i"}, "lw", 4);
  performLoadTest<uint32_t>(0xDEADBEEF, {"rv32i", "rv64i"}, "lwu", 4);
}

TEST(LoadStoreInstructionsTest, LoadHalfWordUnsigned) {
  performLoadTest<uint16_t>(0xF00D, {"rv32i"}, "lhu", 2);
  performLoadTest<uint16_t>(0xF00D, {"rv32i", "rv64i"}, "lhu", 2);
}

TEST(LoadStoreInstructionsTest, LoadByteUnsigned) {
  performLoadTest<uint16_t>(0x99, {"rv32i"}, "lbu", 1);
  performLoadTest<uint16_t>(0x99, {"rv32i", "rv64i"}, "lbu", 1);
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

TEST(LoadStoreInstructionsTest, doiashndf) {
  MemoryValue c = convert<uint8_t>(
      0x1, InstructionNode::RISCV_BITS_PER_BYTE, Endianness::LITTLE);
  std::cout << c<< std::endl;
  std::cout << c.get(0) <<  c.get(c.getSize() - 1) << std::endl;
  uint16_t rc = convert<uint16_t>(c, Endianness::BIG);
  std::cout << std::hex << rc << std::endl;
}
