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

  MemoryValue getValue(DummyMemoryAccess& access)const override {
    return access.getRegisterValue(_id);
  }

  MemoryValue assemble()const override { return MemoryValue{}; }

  const std::string& getIdentifier()const override { return _id; }

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

void test20BitImmediateBounds(InstructionNodeFactory& instrF, std::string instructionToken, ImmediateNodeFactory& immF) {
    constexpr uint64_t boundary = 0xFFFFF;
    std::string registerId = "not relevant";
    auto node = instrF.createInstructionNode(instructionToken);
    node->addChild(std::move(std::make_unique<FakeRegisterNode>(registerId)));
    node->addChild(std::move(std::make_unique<FakeRegisterNode>(registerId)));
    auto immediateNodeIn = immF.createImmediateNode(MemoryValue(boundary));
    node->addChild(std::move(immediateNodeIn));
    ASSERT_TRUE(node->validate());
    auto node2 = instrF.createInstructionNode(instructionToken);
    node2->addChild(std::move(std::make_unique<FakeRegisterNode>(registerId)));
    node2->addChild(std::move(std::make_unique<FakeRegisterNode>(registerId)));
    auto immediateNodeOut = immF.createImmediateNode(MemoryValue(boundary+1));
    node->addChild(std::move(immediateNodeOut));
    ASSERT_FALSE(node->validate());
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
  //test immediate boundary
  test20BitImmediateBounds(factory32, "addi", immediateFactory);

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
  //test immediate boundary
  test20BitImmediateBounds(factory64, "addi", immediateFactory);
}

TEST(IntegerInstructionTest, ADDInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = DummyMemoryAccessImpl();
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "add", false);
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "addi", true);
}

TEST(IntegerInstructionTest, SUBIntruction_testSub) {
  FakeRegister destination1;
  FakeRegister destination2(123);
  FakeRegister firstOp(43);
  FakeRegister secondOp(1);
  DummyMemoryAccessImpl memoryImpl;
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  memoryImpl.addRegister("d0", destination1);
  memoryImpl.addRegister("d1", destination2);
  memoryImpl.addRegister("r1", firstOp);
  memoryImpl.addRegister("r2", secondOp);

  // Test 32Bit versions
  auto factory32 = setUpFactory({"rv32i"});
  assertRegisterInstruction(memoryImpl, factory32, "sub", "d0", "r1", "r2", 42);
  // test 32bit (unsigned) boundary: 0 - 1 = (2^32 -1)= 4294967295
  firstOp.set(0);
  assertRegisterInstruction(memoryImpl, factory32, "sub", "d1", "r1", "r2", 4294967295U);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(123);
  firstOp.set(43);
  assertRegisterInstruction(memoryImpl, factory64, "sub", "d0", "r1", "r2", 42);
  // test 64bit (unsigned) boundary: 0-1 = (2^64 -1) = 18446744073709551615
  firstOp.set(0);
  assertRegisterInstruction(memoryImpl, factory64, "sub", "d1", "r1", "r2", 18446744073709551615ULL);
}

TEST(IntegerInstructionTest, SUBInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = DummyMemoryAccessImpl();
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "sub", false);
}

TEST(IntegerInstructionTest, ANDIntruction_testAnd) {
  FakeRegister destination1;
  FakeRegister destination2;
  FakeRegister destination3(123);
  FakeRegister destination4(123);
  FakeRegister firstOp(0b110110);
  FakeRegister secondOp(0b100100);
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
  assertRegisterInstruction(memoryImpl, factory32, "and", "d0", "r1", "r2", 0b100100);
  assertRegisterInstruction(memoryImpl, factory32, "and", "d1", "r2", "r1", 0b100100);
  firstOp.set(0b101010);
  secondOp.set(0b010101);
  assertRegisterInstruction(memoryImpl, factory32, "and", "d2", "r1", "r2", 0);
  assertRegisterInstruction(memoryImpl, factory32, "and", "d3", "r2", "r1", 0);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  destination3.set(123);
  destination4.set(123);
  firstOp.set(0b110110L << 32);
  secondOp.set(0b100100L << 32);
  assertRegisterInstruction(memoryImpl, factory64, "and", "d0", "r1", "r2", 0b100100L<<32);
  assertRegisterInstruction(memoryImpl, factory64, "and", "d1", "r2", "r1", 0b100100L<<32);
  firstOp.set(0b101010L<<32);
  secondOp.set(0b010101L<<32);
  assertRegisterInstruction(memoryImpl, factory64, "and", "d2", "r1", "r2", 0);
  assertRegisterInstruction(memoryImpl, factory64, "and", "d3", "r2", "r1", 0);
}

TEST(IntegerInstructionTest, ANDIntruction_testAndi) {
  FakeRegister destination1(4444);
  FakeRegister destination2;
  FakeRegister destination3(123);
  FakeRegister destination4(123);
  FakeRegister regOp1(0b110110);
  FakeRegister regOp2(0b100100);
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
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "andi",
                             "d0", "r1", 0b100100, 0b100100);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "andi",
                             "d1", "r2", 0b110110, 0b100100);
  regOp1.set(0b10101010);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "andi",
                             "d2", "r1", 0b01010101, 0);
  //test immediate boundary
  test20BitImmediateBounds(factory32, "andi", immediateFactory);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(1);
  destination2.set(1);
  destination3.set(123);
  destination4.set(123);
  regOp1.set(0b110110L<<32);
  regOp2.set(0b100100L);
  //it is difficult to test 64bit version, because immediate values cannot be greater than 20bit
  //therefore no upper 32bit testing can be done, except for testing against 0
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "andi",
                             "d0", "r1", 0b100100, 0);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "andi",
                             "d1", "r2", 0b110110L, 0b100100L);
  //test immediate boundary
  test20BitImmediateBounds(factory64, "andi", immediateFactory);
}

TEST(IntegerInstructionTest, ANDInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = DummyMemoryAccessImpl();
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "and", false);
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "andi", true);
}

TEST(IntegerInstructionTest, ORIntruction_testOr) {
  FakeRegister destination1;
  FakeRegister destination2;
  FakeRegister destination3(123);
  FakeRegister destination4(123);
  FakeRegister firstOp(0b110110);
  FakeRegister secondOp(0b100100);
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
  assertRegisterInstruction(memoryImpl, factory32, "or", "d0", "r1", "r2", 0b110110);
  assertRegisterInstruction(memoryImpl, factory32, "or", "d1", "r2", "r1", 0b110110);
  firstOp.set(0b101010);
  secondOp.set(0b010101);
  assertRegisterInstruction(memoryImpl, factory32, "or", "d2", "r1", "r2", 0b111111);
  assertRegisterInstruction(memoryImpl, factory32, "or", "d3", "r2", "r1", 0b111111);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  destination3.set(123);
  destination4.set(123);
  firstOp.set(0b110110L << 32);
  secondOp.set(0b100100L << 32);
  assertRegisterInstruction(memoryImpl, factory64, "or", "d0", "r1", "r2", 0b110110L<<32);
  assertRegisterInstruction(memoryImpl, factory64, "or", "d1", "r2", "r1", 0b110110L<<32);
  firstOp.set(0b101010L<<32);
  secondOp.set(0b010101L<<32);
  assertRegisterInstruction(memoryImpl, factory64, "or", "d2", "r1", "r2", 0b111111L<<32);
  assertRegisterInstruction(memoryImpl, factory64, "or", "d3", "r2", "r1", 0b111111L<<32);
}

TEST(IntegerInstructionTest, ORIntruction_testOri) {
  FakeRegister destination1(4444);
  FakeRegister destination2;
  FakeRegister destination3(123);
  FakeRegister destination4(123);
  FakeRegister regOp1(0b110110);
  FakeRegister regOp2(0b100100);
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
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "ori",
                             "d0", "r1", 0b100100, 0b110110);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "ori",
                             "d1", "r2", 0b110110, 0b110110);
  regOp1.set(0b10101010);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "ori",
                             "d2", "r1", 0b01010101, 0b11111111);
  //test immediate boundary
  test20BitImmediateBounds(factory32, "ori", immediateFactory);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  destination3.set(123);
  destination4.set(123);
  regOp1.set(0b1010110L<<32);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "ori",
                             "d0", "r1", 0b10, ((0b1010110L<<32) + 2));
  //test immediate boundary
  test20BitImmediateBounds(factory64, "ori", immediateFactory);
}

TEST(IntegerInstructionTest, ORInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = DummyMemoryAccessImpl();
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "or", false);
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "ori", true);
}

TEST(IntegerInstructionTest, XORIntruction_testXor) {
  FakeRegister destination1;
  FakeRegister destination2;
  FakeRegister destination3(123);
  FakeRegister destination4(123);
  FakeRegister firstOp(0b110110);
  FakeRegister secondOp(0b100100);
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
  assertRegisterInstruction(memoryImpl, factory32, "xor", "d0", "r1", "r2", 0b010010);
  assertRegisterInstruction(memoryImpl, factory32, "xor", "d1", "r2", "r1", 0b010010);
  firstOp.set(0b101010);
  secondOp.set(0b010101);
  assertRegisterInstruction(memoryImpl, factory32, "xor", "d2", "r1", "r2", 0b111111);
  assertRegisterInstruction(memoryImpl, factory32, "xor", "d3", "r2", "r1", 0b111111);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  destination3.set(123);
  destination4.set(123);
  firstOp.set(0b110110L << 32);
  secondOp.set(0b100100L << 32);
  assertRegisterInstruction(memoryImpl, factory64, "xor", "d0", "r1", "r2", 0b010010L<<32);
  assertRegisterInstruction(memoryImpl, factory64, "xor", "d1", "r2", "r1", 0b010010L<<32);
  firstOp.set(0b101010L<<32);
  secondOp.set(0b010101L<<32);
  assertRegisterInstruction(memoryImpl, factory64, "xor", "d2", "r1", "r2", 0b111111L<<32);
  assertRegisterInstruction(memoryImpl, factory64, "xor", "d3", "r2", "r1", 0b111111L<<32);
}

TEST(IntegerInstructionTest, XORIntruction_testXori) {
  FakeRegister destination1(4444);
  FakeRegister destination2;
  FakeRegister destination3(123);
  FakeRegister destination4(123);
  FakeRegister regOp1(0b110110);
  FakeRegister regOp2(0b100100);
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
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "xori",
                             "d0", "r1", 0b100100, 0b010010);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "xori",
                             "d1", "r2", 0b110110, 0b010010);
  regOp1.set(0b10101010);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "xori",
                             "d2", "r1", 0b01010101, 0b11111111);
  //test immediate boundary
  test20BitImmediateBounds(factory32, "xori", immediateFactory);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  destination3.set(123);
  destination4.set(123);
  regOp1.set(0b100000001010110L<<18);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "xori",
                             "d0", "r1", 0b10<<18, (0b100000001010100L<<18));
  //test immediate boundary
  test20BitImmediateBounds(factory64, "xori", immediateFactory);
}

TEST(IntegerInstructionTest, XORInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = DummyMemoryAccessImpl();
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "xor", false);
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "xori", true);
}

TEST(IntegerInstructionTest, ShiftLeftIntruction_testSll) {
  FakeRegister destination1;
  FakeRegister destination2;
  FakeRegister firstOp(0b110110);
  FakeRegister secondOp(3);
  DummyMemoryAccessImpl memoryImpl;
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  memoryImpl.addRegister("d0", destination1);
  memoryImpl.addRegister("d1", destination2);
  memoryImpl.addRegister("r1", firstOp);
  memoryImpl.addRegister("r2", secondOp);

  // Test 32Bit versions
  auto factory32 = setUpFactory({"rv32i"});
  assertRegisterInstruction(memoryImpl, factory32, "sll", "d0", "r1", "r2", 0b110110000);
  //shifts use only the lower 5bit of the second operand
  assertRegisterInstruction(memoryImpl, factory32, "sll", "d1", "r2", "r1", 3 << 22);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  firstOp.set(0b110110L << 32);
  secondOp.set(3);
  assertRegisterInstruction(memoryImpl, factory64, "sll", "d0", "r1", "r2", 0b110110000L<<32);
  //shifts use only the lower 5bit of the second operand
  assertRegisterInstruction(memoryImpl, factory64, "sll", "d1", "r2", "r1", 3);
}

TEST(IntegerInstructionTest, ShiftLeftIntruction_testSlli) {
  FakeRegister destination1;
  FakeRegister destination2;
  FakeRegister regOp1(0b110110);
  FakeRegister regOp2(1);
  DummyMemoryAccessImpl memoryImpl;
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  memoryImpl.addRegister("d0", destination1);
  memoryImpl.addRegister("d1", destination2);
  memoryImpl.addRegister("r1", regOp1);
  memoryImpl.addRegister("r2", regOp2);

  auto immediateFactory = ImmediateNodeFactory();
  // Test 32Bit versions
  auto factory32 = setUpFactory({"rv32i"});
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "slli",
                             "d0", "r1", 3, 0b110110000);
   //shifts use only the lower 5bit of the second operand
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "slli",
                             "d1", "r2", 0b11100001, 2);
  //test immediate boundary
  test20BitImmediateBounds(factory32, "slli", immediateFactory);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  regOp1.set(0b110110L<<32);
  regOp2.set(0b1L<<32);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "slli",
                             "d0", "r1", 3, 0b110110000L<<32);
  //shifts use only the lower 5bit of the second operand
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "slli", "d1", "r2", 0b11100001, 0b10L<<32);

  //test immediate boundary
  test20BitImmediateBounds(factory64, "slli", immediateFactory);
}

TEST(IntegerInstructionTest, ShiftLeftInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = DummyMemoryAccessImpl();
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "sll", false);
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "slli", true);
}

TEST(IntegerInstructionTest, ShiftRightIntruction_testSrl) {
  FakeRegister destination1;
  FakeRegister destination2;
  FakeRegister firstOp(0b110110);
  FakeRegister secondOp(3);
  DummyMemoryAccessImpl memoryImpl;
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  memoryImpl.addRegister("d0", destination1);
  memoryImpl.addRegister("d1", destination2);
  memoryImpl.addRegister("r1", firstOp);
  memoryImpl.addRegister("r2", secondOp);

  // Test 32Bit versions
  auto factory32 = setUpFactory({"rv32i"});
  assertRegisterInstruction(memoryImpl, factory32, "srl", "d0", "r1", "r2", 0b110);
  //shifts use only the lower 5bit of the second operand
  assertRegisterInstruction(memoryImpl, factory32, "srl", "d1", "r2", "r1", 0);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  firstOp.set(0b110110L << 32);
  secondOp.set(3);
  assertRegisterInstruction(memoryImpl, factory64, "srl", "d0", "r1", "r2", 0b110110L<<29);
  //shifts use only the lower 5bit of the second operand
  assertRegisterInstruction(memoryImpl, factory64, "srl", "d1", "r2", "r1", 3);
}

TEST(IntegerInstructionTest, ShiftRightIntruction_testSrli) {
  FakeRegister destination1;
  FakeRegister destination2;
  FakeRegister regOp1(0b110110);
  FakeRegister regOp2(1);
  DummyMemoryAccessImpl memoryImpl;
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  memoryImpl.addRegister("d0", destination1);
  memoryImpl.addRegister("d1", destination2);
  memoryImpl.addRegister("r1", regOp1);
  memoryImpl.addRegister("r2", regOp2);

  auto immediateFactory = ImmediateNodeFactory();
  // Test 32Bit versions
  auto factory32 = setUpFactory({"rv32i"});
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "srli",
                             "d0", "r1", 3, 0b110);
   //shifts use only the lower 5bit of the second operand
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "srli",
                             "d1", "r1", 0b11100001, 0b11011);
  //test immediate boundary
  test20BitImmediateBounds(factory32, "srli", immediateFactory);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  regOp1.set(0b110110L<<32);
  regOp2.set(0b11L<<32);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "srli",
                             "d0", "r1", 3, 0b110110L<<29);
  //shifts use only the lower 5bit of the second operand
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "srli", "d1", "r2", 0b11100001, 0b11L<<31);

  //test immediate boundary
  test20BitImmediateBounds(factory64, "srli", immediateFactory);
}

TEST(IntegerInstructionTest, ShiftRightInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = DummyMemoryAccessImpl();
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "srl", false);
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "srli", true);
}

TEST(IntegerInstructionTest, ShiftRightArithmeticIntruction_testSra) {
  FakeRegister destination1;
  FakeRegister destination2;
  FakeRegister firstOp(0b10000000000000000000000000110110);
  FakeRegister secondOp(3);
  DummyMemoryAccessImpl memoryImpl;
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  memoryImpl.addRegister("d0", destination1);
  memoryImpl.addRegister("d1", destination2);
  memoryImpl.addRegister("r1", firstOp);
  memoryImpl.addRegister("r2", secondOp);

  // Test 32Bit versions
  auto factory32 = setUpFactory({"rv32i"});
  assertRegisterInstruction(memoryImpl, factory32, "sra", "d0", "r1", "r2", 0b11110000000000000000000000000110);
  //shifts use only the lower 5bit of the second operand
  assertRegisterInstruction(memoryImpl, factory32, "sra", "d1", "r2", "r1", 0);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  firstOp.set(0b1000000000000000000000000001101100000000000000000000000000000000L);
  secondOp.set(3);
  assertRegisterInstruction(memoryImpl, factory64, "sra", "d0", "r1", "r2", 0b1111000000000000000000000000001101100000000000000000000000000000L);
  //shifts use only the lower 5bit of the second operand
  assertRegisterInstruction(memoryImpl, factory64, "srl", "d1", "r2", "r1", 3);
}

TEST(IntegerInstructionTest, ShiftRightIntruction_testSrai) {
  FakeRegister destination1;
  FakeRegister destination2;
  FakeRegister regOp1(0b10000000000000000000000000110110);
  FakeRegister regOp2(1);
  DummyMemoryAccessImpl memoryImpl;
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  memoryImpl.addRegister("d0", destination1);
  memoryImpl.addRegister("d1", destination2);
  memoryImpl.addRegister("r1", regOp1);
  memoryImpl.addRegister("r2", regOp2);

  auto immediateFactory = ImmediateNodeFactory();
  // Test 32Bit versions
  auto factory32 = setUpFactory({"rv32i"});
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "srai",
                             "d0", "r1", 3, 0b11110000000000000000000000000110);
   //shifts use only the lower 5bit of the second operand
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "srai",
                             "d1", "r1", 0b11100001, 0b11000000000000000000000000011011);
  //test immediate boundary
  test20BitImmediateBounds(factory32, "srai", immediateFactory);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  regOp1.set(0b1000000000000000000000000001101100000000000000000000000000000000L);
  regOp2.set(0b11L<<32);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "srai",
                             "d0", "r1", 3, 0b1111000000000000000000000000001101100000000000000000000000000000L);
  //shifts use only the lower 5bit of the second operand
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "srai", "d1", "r2", 0b11100001, 0b11L<<31);

  //test immediate boundary
  test20BitImmediateBounds(factory64, "srai", immediateFactory);
}

TEST(IntegerInstructionTest, ShiftRightArithmeticInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = DummyMemoryAccessImpl();
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "sra", false);
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "srai", true);
}
