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

#include "gtest/gtest.h"

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/riscv/immediate-node-factory.hpp"
#include "arch/riscv/instruction-node-factory.hpp"
#include "arch/riscv/load-store-instructions.hpp"

#include "dummies.hpp"

using namespace riscv;

TEST(LoadStoreInstructionsTest, StoreWord32) {
  FakeRegister base, src;
  std::string baseId{"base"}, srcId{"src"};
  MemoryValue storeValue = convertToMem<uint32_t>(0xBADC0DED);

  DummyMemoryAccessImpl memoryAccess;
  // Add registers
  memoryAccess.addRegister(baseId, base);
  memoryAccess.addRegister(srcId, src);
  // Fill registers
  memoryAccess.setRegisterValue(baseId, convertToMem<uint32_t>(0));
  memoryAccess.setRegisterValue(srcId, storeValue);

  auto instrFactory     = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory{};
  auto instr            = instrFactory.createInstructionNode("sw");

  // Fill the instructions operands
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(baseId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(srcId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(
      immediateFactory.createImmediateNode(convertToMem<uint32_t>(0)));
  ASSERT_TRUE(instr->validate());

  // Execute the instruction
  instr->getValue(memoryAccess);

  // Check result
  ASSERT_EQ(memoryAccess.getMemoryValueAt(0, 4), storeValue);
}

TEST(LoadStoreInstructionsTest, LoadWord32) {
  FakeRegister dest, base;
  std::string destId{"dest"}, baseId{"base"};
  MemoryValue loadValue = convertToMem<uint32_t>(0xDEADBEEF);

  DummyMemoryAccessImpl memoryAccess;
  // Add registers
  memoryAccess.addRegister(destId, dest);
  memoryAccess.addRegister(baseId, base);
  // Fill registers & memory
  memoryAccess.setRegisterValue(baseId, convertToMem<uint32_t>(0));
  memoryAccess.setMemoryValueAt(0, loadValue);

  auto instrFactory     = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory{};
  auto instr            = instrFactory.createInstructionNode("lw");

  // Fill the instructions operands
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(destId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(baseId));
  ASSERT_FALSE(instr->validate());
  instr->addChild(
      immediateFactory.createImmediateNode(convertToMem<uint32_t>(0)));
  ASSERT_TRUE(instr->validate());

  // Execute the instruction
  instr->getValue(memoryAccess);

  // Check result
  ASSERT_EQ(memoryAccess.getRegisterValue(destId), loadValue);
}
