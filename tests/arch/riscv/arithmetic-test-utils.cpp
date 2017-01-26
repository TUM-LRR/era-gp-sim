/*
* ERASIM Assembler Interpreter
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <string>

#include "arithmetic-test-utils.hpp"
#include "common/assert.hpp"

void testIntegerInstructionValidation(MemoryAccess& memAccess,
                                      const NodeFactoryCollection& factories,
                                      ImmediateNodeFactory& immF,
                                      const std::string& instructionToken,
                                      bool isImmediateInstr) {
  using AssertionError = assert::AssertionError;
  auto registerId = "x0";
  // add 0-3 random Nodes
  auto instructionNode = factories.createInstructionNode(instructionToken);
  ASSERT_TRUE(instructionNode);
  // ASSERT_THROW(instructionNode->getValue(memAccess), AssertionError);
  instructionNode->addChild(std::move(immF.createImmediateNode(MemoryValue())));
  // ASSERT_THROW(instructionNode->getValue(memAccess), AssertionError);
  instructionNode->addChild(std::move(immF.createImmediateNode(MemoryValue())));
  // ASSERT_THROW(instructionNode->getValue(memAccess), AssertionError);
  instructionNode->addChild(std::move(immF.createImmediateNode(MemoryValue())));
  // ASSERT_THROW(instructionNode->getValue(memAccess), AssertionError);

  // test opposite operand configuration
  auto instructionNode2 = factories.createInstructionNode(instructionToken);
  instructionNode2->addChild(
      std::move(factories.createRegisterNode(registerId)));
  instructionNode2->addChild(
      std::move(factories.createRegisterNode(registerId)));
  if (isImmediateInstr) {
    // Immediate Instraction has now 3 Register Operands
    instructionNode2->addChild(
        std::move(factories.createRegisterNode(registerId)));
  } else {
    // Register Instraction has now 2 Register & 1 Immediate Operand
    instructionNode2->addChild(
        std::move(immF.createImmediateNode(riscv::convert<std::uint64_t>(0))));
  }
  ASSERT_FALSE(instructionNode2->validate(memAccess).isSuccess());
  // ASSERT_THROW(instructionNode2->getValue(memAccess), AssertionError);

  // test valid children, but with one more operand
  auto instructionNode3 = factories.createInstructionNode(instructionToken);
  instructionNode3->addChild(
      std::move(factories.createRegisterNode(registerId)));
  instructionNode3->addChild(
      std::move(factories.createRegisterNode(registerId)));
  if (isImmediateInstr) {
    instructionNode3->addChild(
        std::move(immF.createImmediateNode(riscv::convert<std::uint64_t>(0))));
  } else {
    instructionNode3->addChild(
        std::move(factories.createRegisterNode(registerId)));
  }
  ASSERT_TRUE(instructionNode3->validate(memAccess).isSuccess());
  // add one more random node
  instructionNode3->addChild(
      std::move(immF.createImmediateNode(riscv::convert<std::uint64_t>(0))));
  ASSERT_FALSE(instructionNode3->validate(memAccess).isSuccess());
  // ASSERT_THROW(instructionNode3->getValue(memAccess), AssertionError);
}

void test12BitImmediateBounds(const NodeFactoryCollection& factories,
                              const std::string& instructionToken,
                              ImmediateNodeFactory& immF,
                              MemoryAccess& access) {
  constexpr std::uint64_t boundary = 0x7FF;
  constexpr std::int64_t negative_boundary =
      -2048;// smallest negative integer in 12bit
  auto registerId = "x0";
  auto nodeTrue = factories.createInstructionNode(instructionToken);
  nodeTrue->addChild(std::move(factories.createRegisterNode(registerId)));
  nodeTrue->addChild(std::move(factories.createRegisterNode(registerId)));
  auto immediateNodeIn =
      immF.createImmediateNode(riscv::convert<std::uint64_t>(boundary));
  nodeTrue->addChild(std::move(immediateNodeIn));
  ASSERT_TRUE(nodeTrue->validate(access).isSuccess());

  auto nodeTrueNegative = factories.createInstructionNode(instructionToken);
  nodeTrueNegative->addChild(
      std::move(factories.createRegisterNode(registerId)));
  nodeTrueNegative->addChild(
      std::move(factories.createRegisterNode(registerId)));
  auto immediateNodeNegative =
      immF.createImmediateNode(riscv::convert<std::int64_t>(negative_boundary));
  nodeTrueNegative->addChild(std::move(immediateNodeNegative));
  ASSERT_TRUE(nodeTrueNegative->validate(access).isSuccess());

  auto nodeFalse = factories.createInstructionNode(instructionToken);
  nodeFalse->addChild(std::move(factories.createRegisterNode(registerId)));
  nodeFalse->addChild(std::move(factories.createRegisterNode(registerId)));
  auto immediateNodeOut =
      immF.createImmediateNode(riscv::convert<std::int64_t>(boundary + 1));
  nodeFalse->addChild(std::move(immediateNodeOut));
  ASSERT_FALSE(nodeFalse->validate(access).isSuccess());
}
