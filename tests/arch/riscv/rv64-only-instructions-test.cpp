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

#include "dummies.hpp"

using namespace riscv;

TEST(RV64OnlyInstructionsTest, aosidfn) {
  // Set up registers
  FakeRegister dest, src;
  std::string destId{"dest"}, srcId{"src"};
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(destId, dest);
  memoryAccess.addRegister(srcId, src);

  memoryAccess.setRegisterValue(srcId, convertToMem<uint64_t>((uint64_t{1} << 31) + 1));

  auto instrFactory     = setUpFactory({"rv32i", "rv64i"});
  auto immediateFactory = ImmediateNodeFactory{};
  auto instr            = instrFactory.createInstructionNode("addiw");

  ASSERT_TRUE(instr);
  ASSERT_FALSE(instr->validate());
  instr->addChild(std::make_unique<FakeRegisterNode>(destId));
  instr->addChild(std::make_unique<FakeRegisterNode>(srcId));
  instr->addChild(immediateFactory.createImmediateNode(convertToMem<uint64_t>(2)));
  ASSERT_TRUE(instr->validate());

  instr->getValue(memoryAccess);

  MemoryValue result = memoryAccess.getRegisterValue(destId);
  ASSERT_EQ(convertToInt<uint64_t>(result), 3);
}
