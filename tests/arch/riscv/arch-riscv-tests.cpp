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

#include "arch/riscv/integer_instructions.hpp"

#include <gtest/gtest.h>
#include <memory>

TEST(InstructionTest, AddInstruction) {
  AddInstructionNode addiNode{true};
  AddInstructionNode addNode{false};

  // Basic testing
  ASSERT_EQ(NodeType::INSTRUCTION, addiNode.getType());
  ASSERT_EQ(NodeType::INSTRUCTION, addNode.getType());

  ASSERT_EQ("addi", addiNode.getIdentifier());
  ASSERT_EQ("add", addNode.getIdentifier());

  // Validate the empty syntax trees -> should return false
  ASSERT_FALSE(addiNode.validate());
  ASSERT_FALSE(addNode.validate());

  // Create some registers
  std::unique_ptr<RegisterNode> r1 = std::make_unique<RegisterNode>("zero");
  std::unique_ptr<RegisterNode> r2 = std::make_unique<RegisterNode>("x1");
  std::unique_ptr<RegisterNode> r3 = std::make_unique<RegisterNode>("x10");
  std::unique_ptr<RegisterNode> r4 = std::make_unique<RegisterNode>("x3");
  std::unique_ptr<RegisterNode> r5 = std::make_unique<RegisterNode>("t0");
  std::unique_ptr<RegisterNode> r6 = std::make_unique<RegisterNode>("x0");

  // Add the registers
  addiNode.addChild(std::move(r1));
  addiNode.addChild(std::move(r2));
  addiNode.addChild(std::move(r3));

  addNode.addChild(std::move(r4));
  addNode.addChild(std::move(r5));
  addNode.addChild(std::move(r6));

  // Validate again
  ASSERT_FALSE(addiNode.validate());
  ASSERT_TRUE(addNode.validate());
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
