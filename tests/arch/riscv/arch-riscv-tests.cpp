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

#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "arch/riscv/instruction-node.hpp"
#include "arch/riscv/integer-instructions.hpp"
#include "arch/riscv/load-store-instructions.hpp"
#include "arch/riscv/riscv-node-factories.hpp"

using namespace riscv;

TEST(InstructionTest, AddInstruction) {
  InstructionNodeFactory instructionFactory;
  RegisterAccessNodeFactory registerFactory;

  std::string addiToken = "ADDI";
  std::string addToken  = "ADD";

  auto addiNode = instructionFactory.createInstructionNode(addiToken);
  auto addNode  = instructionFactory.createInstructionNode(addToken);

  // Basic testing
  ASSERT_EQ(Type::INSTRUCTION, addiNode->getType());
  ASSERT_EQ(Type::INSTRUCTION, addNode->getType());

  ASSERT_EQ(addiToken, addiNode->getIdentifier());
  ASSERT_EQ(addToken, addNode->getIdentifier());

  // Validate the empty syntax trees -> should return false
  ASSERT_FALSE(addiNode->validate());
  ASSERT_FALSE(addNode->validate());

  // Create some registers
  auto r1 = registerFactory.createRegisterAccessNode("x0");
  auto r2 = registerFactory.createRegisterAccessNode("x1");
  auto r3 = registerFactory.createRegisterAccessNode("x2");
  auto r4 = registerFactory.createRegisterAccessNode("x3");
  auto r5 = registerFactory.createRegisterAccessNode("x4");
  auto r6 = registerFactory.createRegisterAccessNode("x5");

  // Add the registers
  addiNode->addChild(std::move(r1));
  addiNode->addChild(std::move(r2));
  addiNode->addChild(std::move(r3));

  addNode->addChild(std::move(r4));
  addNode->addChild(std::move(r5));
  addNode->addChild(std::move(r6));

  // Validate again
  ASSERT_FALSE(addiNode->validate());
  ASSERT_TRUE(addNode->validate());
}

TEST(InstructionTest, LoadInstruction) {
  LoadInstructionNode lw{LoadType::WORD}, lh{LoadType::HALF_WORD},
      lhu{LoadType::HALF_WORD_UNSIGNED}, lb{LoadType::BYTE},
      lbu{LoadType::BYTE_UNSIGNED};
  std::vector<LoadInstructionNode *> nodes{&lw, &lh, &lhu, &lb, &lbu};

  // Basic testing
  for (auto node : nodes) {
    ASSERT_EQ(Type::INSTRUCTION, node->getType());
    ASSERT_FALSE(node->validate());
  }

  ASSERT_EQ("LW", lw.getIdentifier());
  ASSERT_EQ("LH", lh.getIdentifier());
  ASSERT_EQ("LHU", lhu.getIdentifier());
  ASSERT_EQ("LB", lb.getIdentifier());
  ASSERT_EQ("LBU", lbu.getIdentifier());

  // TODO Make tests for immediate values
}

TEST(InstructionTest, StoreInstruction) {
  InstructionNodeFactory instructionFactory;

  auto sw = instructionFactory.createInstructionNode("SW");
  auto sh = instructionFactory.createInstructionNode("SH");
  auto sb = instructionFactory.createInstructionNode("SB");

  // Create a vector over the nodes to make testing easier
  auto nodes = {sw.get(), sh.get(), sb.get()};

  // Basic testing
  for (auto const node : nodes) {
    ASSERT_EQ(Type::INSTRUCTION, node->getType());
    ASSERT_FALSE(node->validate());
  }

  ASSERT_EQ("SW", sw->getIdentifier());
  ASSERT_EQ("SH", sh->getIdentifier());
  ASSERT_EQ("SB", sb->getIdentifier());

  // TODO Make tests for immediate values
}

/* TODO Test more commands */

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}