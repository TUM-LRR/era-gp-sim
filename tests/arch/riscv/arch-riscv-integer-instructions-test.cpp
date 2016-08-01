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

#include <cassert>
#include <cstdint>
#include <iostream>  //remove this
#include <unordered_map>

#include "gtest/gtest.h"

#include "arch/common/architecture-formula.hpp"
#include "arch/common/immediate-node.hpp"
#include "arch/common/register-node.hpp"
#include "arch/riscv/immediate-node-factory.hpp"
#include "arch/riscv/instruction-node-factory.hpp"

using namespace riscv;

namespace {
class FakeRegister {
 public:
  FakeRegister() : FakeRegister(0) {}
  FakeRegister(uint64_t value) : _value(value) {}

  void set(uint64_t newValue) { _value = newValue; }

  uint64_t get() { return _value; }

 private:
  uint64_t _value;
};

class FakeRegisterNode : public RegisterNode {
 public:
  explicit FakeRegisterNode(std::string& regId)
      : RegisterNode(regId), _id(regId) {}

  MemoryValue getValue(DummyMemoryAccess& access) override {
    return access.getRegisterValue(_id);
  }

  MemoryValue assemble() override { return MemoryValue(); }

  std::string getIdentifier() override { return _id; }

 private:
  std::string& _id;
};

class DummyMemoryAccessImpl : public DummyMemoryAccess {
 public:
  void addRegister(std::string token, FakeRegister& reg) {
    _register.emplace(token, reg);
  }

  MemoryValue getRegisterValue(std::string& token) override {
    return MemoryValue(_register.at(token).get());
  }
  void setRegisterValue(std::string& token, MemoryValue value) override {
    FakeRegister& desiredRegister = _register.at(token);
    desiredRegister.set(value.getValue());
  }

 private:
  std::unordered_map<std::string, FakeRegister&> _register;
};

void assertRegisterInstruction(DummyMemoryAccess& memAccess,
                               InstructionNodeFactory& fact,
                               std::string instructionToken,
                               std::string destinationReg,
                               std::string operand1Reg, std::string operand2Reg,
                               uint64_t expectedResult) {
  // Create the nodes for the instruction
  auto instrNode = fact.createInstructionNode(instructionToken);
  ASSERT_FALSE(instrNode->validate());
  auto destNode = std::make_unique<FakeRegisterNode>(destinationReg);
  auto operand1Node = std::make_unique<FakeRegisterNode>(operand1Reg);
  auto operand2Node = std::make_unique<FakeRegisterNode>(operand2Reg);
  // Add them step-by-step
  instrNode->addChild(std::move(destNode));
  ASSERT_FALSE(instrNode->validate());
  instrNode->addChild(std::move(operand1Node));
  ASSERT_FALSE(instrNode->validate());
  instrNode->addChild(std::move(operand2Node));
  ASSERT_TRUE(instrNode->validate());

  // Save values of operand registers to determine change
  MemoryValue preOp1 = memAccess.getRegisterValue(operand1Reg);
  MemoryValue preOp2 = memAccess.getRegisterValue(operand2Reg);

  // Perform instruction
  MemoryValue returnValue = instrNode->getValue(memAccess);
  ASSERT_EQ(0, returnValue.getValue());

  // Check that operand register stayed the same
  ASSERT_EQ(preOp1.getValue(),
            memAccess.getRegisterValue(operand1Reg).getValue());
  ASSERT_EQ(preOp2.getValue(),
            memAccess.getRegisterValue(operand2Reg).getValue());

  // Read result from destination register
  MemoryValue result = memAccess.getRegisterValue(destinationReg);
  ASSERT_EQ(expectedResult, result.getValue());
}

void assertImmediateInstruction(
    DummyMemoryAccess& memAccess, InstructionNodeFactory& instructionFactory,
    ImmediateNodeFactory& immediateFactory, std::string instructionToken,
    std::string destinationReg, std::string registerOperand,
    uint64_t immediateValue, uint64_t expectedResult) {
  // create instruction node
  auto instrNode = instructionFactory.createInstructionNode(instructionToken);
  ASSERT_FALSE(instrNode->validate());
  auto destination = std::make_unique<FakeRegisterNode>(destinationReg);
  auto registerOp = std::make_unique<FakeRegisterNode>(registerOperand);
  auto immediateOp =
      immediateFactory.createImmediateNode(MemoryValue(immediateValue));
  // Add them step-by-step
  instrNode->addChild(std::move(destination));
  ASSERT_FALSE(instrNode->validate());
  instrNode->addChild(std::move(registerOp));
  ASSERT_FALSE(instrNode->validate());
  instrNode->addChild(std::move(immediateOp));
  ASSERT_TRUE(instrNode->validate());

  // Save values of operand register to determine change
  MemoryValue preRegisterOp = memAccess.getRegisterValue(registerOperand);

  // Perform Instruction
  MemoryValue returnValue = instrNode->getValue(memAccess);
  ASSERT_EQ(0, returnValue.getValue());

  // Check that register operand stayed the same
  ASSERT_EQ(preRegisterOp.getValue(),
            memAccess.getRegisterValue(registerOperand).getValue());

  // Read result from destination register
  MemoryValue result = memAccess.getRegisterValue(destinationReg);
  ASSERT_EQ(expectedResult, result.getValue());
}

InstructionNodeFactory setUpFactory(
    ArchitectureFormula::InitializerList modules =
        ArchitectureFormula::InitializerList()) {
  auto formula = ArchitectureFormula("riscv", modules);
  auto riscv = Architecture::Brew(formula);
  return InstructionNodeFactory(riscv.getInstructions(), riscv);
}

void testIntegerInstructionValidation(DummyMemoryAccess& memAccess,
                                      InstructionNodeFactory& instrF,
                                      ImmediateNodeFactory& immF,
                                      std::string instructionToken,
                                      bool isImmediateInstr) {
  std::string registerId = "not relevant";
  // add 0-3 random Nodes
  auto instructionNode = instrF.createInstructionNode(instructionToken);
  ASSERT_DEATH(instructionNode->getValue(memAccess), "");
  instructionNode->addChild(std::move(immF.createImmediateNode(MemoryValue())));
  ASSERT_DEATH(instructionNode->getValue(memAccess), "");
  instructionNode->addChild(std::move(immF.createImmediateNode(MemoryValue())));
  ASSERT_DEATH(instructionNode->getValue(memAccess), "");
  instructionNode->addChild(std::move(immF.createImmediateNode(MemoryValue())));
  ASSERT_DEATH(instructionNode->getValue(memAccess), "");

  // test opposite operand configuration
  auto instructionNode2 = instrF.createInstructionNode(instructionToken);
  instructionNode2->addChild(
      std::move(std::make_unique<FakeRegisterNode>(registerId)));
  instructionNode2->addChild(
      std::move(std::make_unique<FakeRegisterNode>(registerId)));
  if (isImmediateInstr) {
    // Immediate Instraction has now 3 Register Operands
    instructionNode2->addChild(
        std::move(std::make_unique<FakeRegisterNode>(registerId)));
  } else {
    // Register Instraction has now 2 Register & 1 Immediate Operand
    instructionNode2->addChild(
        std::move(immF.createImmediateNode(MemoryValue())));
  }
  ASSERT_FALSE(instructionNode2->validate());
  ASSERT_DEATH(instructionNode2->getValue(memAccess), "");

  // test valid children, but with one more operand
  auto instructionNode3 = instrF.createInstructionNode(instructionToken);
  instructionNode3->addChild(
      std::move(std::make_unique<FakeRegisterNode>(registerId)));
  instructionNode3->addChild(
      std::move(std::make_unique<FakeRegisterNode>(registerId)));
  if (isImmediateInstr) {
    instructionNode3->addChild(
        std::move(immF.createImmediateNode(MemoryValue())));
  } else {
    instructionNode3->addChild(
        std::move(std::make_unique<FakeRegisterNode>(registerId)));
  }
  ASSERT_TRUE(instructionNode3->validate());
  // add one more random node
  instructionNode3->addChild(
      std::move(immF.createImmediateNode(MemoryValue())));
  ASSERT_FALSE(instructionNode3->validate());
  ASSERT_DEATH(instructionNode3->getValue(memAccess), "");
}
}

TEST(IntegerInstructionTest, ADDIntruction_testAdd) {
  FakeRegister destination1;
  FakeRegister destination2;
  FakeRegister destination3(123);
  FakeRegister destination4(123);
  FakeRegister firstOp(41);
  FakeRegister secondOp(1);
  DummyMemoryAccessImpl memoryImpl;
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  memoryImpl.addRegister("d0", destination1);
  memoryImpl.addRegister("d1", destination2);
  memoryImpl.addRegister("d2", destination3);
  memoryImpl.addRegister("d3", destination4);
  memoryImpl.addRegister("r1", firstOp);
  memoryImpl.addRegister("r2", secondOp);

  // Test 32Bit versions
  auto factory32 = setUpFactory({"rv32i"});
  assertRegisterInstruction(memoryImpl, factory32, "add", "d0", "r1", "r2", 42);
  assertRegisterInstruction(memoryImpl, factory32, "add", "d1", "r2", "r1", 42);
  // test 32bit (unsigned) boundary: (2^32 -1) + 1 = 4294967295 + 1 = 0
  firstOp.set(4294967295U);
  assertRegisterInstruction(memoryImpl, factory32, "add", "d2", "r1", "r2", 0);
  assertRegisterInstruction(memoryImpl, factory32, "add", "d3", "r2", "r1", 0);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  destination3.set(123);
  destination4.set(123);
  firstOp.set(41);
  assertRegisterInstruction(memoryImpl, factory64, "add", "d0", "r1", "r2", 42);
  assertRegisterInstruction(memoryImpl, factory64, "add", "d1", "r2", "r1", 42);
  // test 64bit (unsigned) boundary: (2^64 -1) +1 = 18446744073709551615 + 1 = 0
  firstOp.set(18446744073709551615ULL);
  assertRegisterInstruction(memoryImpl, factory64, "add", "d2", "r1", "r2", 0);
  assertRegisterInstruction(memoryImpl, factory64, "add", "d3", "r2", "r1", 0);
}

TEST(IntegerInstructionTest, ADDIntruction_testAddi) {
  FakeRegister destination1(4444);
  FakeRegister destination2;
  FakeRegister destination3(123);
  FakeRegister destination4(123);
  FakeRegister regOp1(41);
  FakeRegister regOp2(1);
  DummyMemoryAccessImpl memoryImpl;
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  memoryImpl.addRegister("d0", destination1);
  memoryImpl.addRegister("d1", destination2);
  memoryImpl.addRegister("d2", destination3);
  memoryImpl.addRegister("d3", destination4);
  memoryImpl.addRegister("r1", regOp1);
  memoryImpl.addRegister("r2", regOp2);

  auto immediateFactory = ImmediateNodeFactory();
  // Test 32Bit versions
  auto factory32 = setUpFactory({"rv32i"});
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "addi",
                             "d0", "r1", 1, 42);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "addi",
                             "d1", "r2", 41, 42);
  // test 32bit (unsigned) boundary: (2^32 -1) + 1 = 4294967295 + 1 = 0
  regOp1.set(4294967295U);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "addi",
                             "d2", "r1", 1, 0);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "addi",
                             "d3", "r2", 4294967295U, 0);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  destination3.set(123);
  destination4.set(123);
  regOp1.set(41);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "addi",
                             "d0", "r1", 1, 42);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "addi",
                             "d1", "r2", 41, 42);
  // test 64bit (unsigned) boundary: (2^64 -1) +1 = 18446744073709551615 + 1 = 0
  regOp1.set(18446744073709551615ULL);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "addi",
                             "d2", "r1", 1, 0);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "addi",
                             "d3", "r2", 18446744073709551615ULL, 0);
}

TEST(IntegerInstructionTest, ADDInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = DummyMemoryAccessImpl();
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "add", false);
}
