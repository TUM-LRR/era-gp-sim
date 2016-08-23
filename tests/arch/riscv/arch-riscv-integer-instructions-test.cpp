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
#include <iostream>
#include <unordered_map>

#include "gtest/gtest.h"

#include "arch/common/architecture-formula.hpp"
#include "arch/common/immediate-node.hpp"
#include "arch/common/register-node.hpp"
#include "arch/riscv/immediate-node-factory.hpp"
#include "arch/riscv/instruction-node-factory.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "core/conversions.hpp"

using namespace riscv;

namespace {
template <typename IntType>
MemoryValue convertToMem(IntType t) {
  return convert<IntType>(t, InstructionNode::RISCV_BITS_PER_BYTE,
                          InstructionNode::RISCV_ENDIANNESS);
}

class FakeRegister {
 public:
  FakeRegister() : FakeRegister(0) {}
  FakeRegister(uint64_t value) : _value(convertToMem<uint64_t>(value)) {}
  FakeRegister(MemoryValue& v) : _value(v) {}

  void set(uint64_t newValue) { _value = convertToMem<uint64_t>(newValue); }
  void set(MemoryValue& v) { _value = v; }

  MemoryValue get() { return _value; }

 private:
  MemoryValue _value;
};

MemoryValue to32BitMemoryValue(uint32_t value) {
  return convertToMem<uint32_t>(value);
}

MemoryValue to64BitMemoryValue(uint64_t value) {
  return convertToMem<uint64_t>(value);
}

class FakeRegisterNode : public RegisterNode {
 public:
  explicit FakeRegisterNode(std::string& regId)
      : RegisterNode(regId), _id(regId) {}

  MemoryValue getValue(DummyMemoryAccess& access) const override {
    return access.getRegisterValue(_id);
  }

  MemoryValue assemble() const override { return MemoryValue{}; }

  const std::string& getIdentifier() const override { return _id; }

 private:
  std::string& _id;
};

class DummyMemoryAccessImpl : public DummyMemoryAccess {
 public:
  void addRegister(std::string token, FakeRegister& reg) {
    _register.emplace(token, reg);
  }

  MemoryValue getRegisterValue(std::string& token) override {
    return _register.at(token).get();
  }
  void setRegisterValue(std::string& token, MemoryValue value) override {
    FakeRegister& desiredRegister = _register.at(token);
    desiredRegister.set(value);
  }

 private:
  std::unordered_map<std::string, FakeRegister&> _register;
};

void assertRegisterInstruction(DummyMemoryAccess& memAccess,
                               InstructionNodeFactory& fact,
                               std::string instructionToken,
                               std::string destinationReg,
                               std::string operand1Reg, std::string operand2Reg,
                               MemoryValue expectedResult) {
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
  ASSERT_EQ(convertToMem<uint64_t>(0), returnValue);

  // Check that operand register stayed the same
  ASSERT_EQ(preOp1, memAccess.getRegisterValue(operand1Reg));
  ASSERT_EQ(preOp2, memAccess.getRegisterValue(operand2Reg));

  // Read result from destination register
  MemoryValue result = memAccess.getRegisterValue(destinationReg);
  ASSERT_EQ(expectedResult, result);
}

void assertImmediateInstruction(
    DummyMemoryAccess& memAccess, InstructionNodeFactory& instructionFactory,
    ImmediateNodeFactory& immediateFactory, std::string instructionToken,
    std::string destinationReg, std::string registerOperand,
    MemoryValue immediateValue, MemoryValue expectedResult) {
  // create instruction node
  auto instrNode = instructionFactory.createInstructionNode(instructionToken);
  ASSERT_FALSE(instrNode->validate());
  auto destination = std::make_unique<FakeRegisterNode>(destinationReg);
  auto registerOp = std::make_unique<FakeRegisterNode>(registerOperand);
  auto immediateOp = immediateFactory.createImmediateNode(immediateValue);
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
  ASSERT_EQ(convertToMem<uint64_t>(0), returnValue);

  // Check that register operand stayed the same
  ASSERT_EQ(preRegisterOp, memAccess.getRegisterValue(registerOperand));

  // Read result from destination register
  MemoryValue result = memAccess.getRegisterValue(destinationReg);
  ASSERT_EQ(result, expectedResult);
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
        std::move(immF.createImmediateNode(convertToMem<uint64_t>(0))));
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
        std::move(immF.createImmediateNode(convertToMem<uint64_t>(0))));
  } else {
    instructionNode3->addChild(
        std::move(std::make_unique<FakeRegisterNode>(registerId)));
  }
  ASSERT_TRUE(instructionNode3->validate());
  // add one more random node
  instructionNode3->addChild(
      std::move(immF.createImmediateNode(convertToMem<uint64_t>(0))));
  ASSERT_FALSE(instructionNode3->validate());
  ASSERT_DEATH(instructionNode3->getValue(memAccess), "");
}

void test20BitImmediateBounds(InstructionNodeFactory& instrF,
                              std::string instructionToken,
                              ImmediateNodeFactory& immF) {
  constexpr uint64_t boundary = 0xFFFFF;
  std::string registerId = "not relevant";
  auto node = instrF.createInstructionNode(instructionToken);
  node->addChild(std::move(std::make_unique<FakeRegisterNode>(registerId)));
  node->addChild(std::move(std::make_unique<FakeRegisterNode>(registerId)));
  auto immediateNodeIn =
      immF.createImmediateNode(convertToMem<uint64_t>(boundary));
  node->addChild(std::move(immediateNodeIn));
  ASSERT_TRUE(node->validate());
  auto node2 = instrF.createInstructionNode(instructionToken);
  node2->addChild(std::move(std::make_unique<FakeRegisterNode>(registerId)));
  node2->addChild(std::move(std::make_unique<FakeRegisterNode>(registerId)));
  auto immediateNodeOut =
      immF.createImmediateNode(convertToMem<uint64_t>(boundary + 1));
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
  auto v42_32b = to32BitMemoryValue(42);
  auto v0_32b = to32BitMemoryValue(0);
  assertRegisterInstruction(memoryImpl, factory32, "add", "d0", "r1", "r2",
                            v42_32b);
  assertRegisterInstruction(memoryImpl, factory32, "add", "d1", "r2", "r1",
                            v42_32b);
  // test 32bit (unsigned) boundary: (2^32 -1) + 1 = 4294967295 + 1 = 0
  firstOp.set(4294967295U);
  assertRegisterInstruction(memoryImpl, factory32, "add", "d2", "r1", "r2",
                            v0_32b);
  assertRegisterInstruction(memoryImpl, factory32, "add", "d3", "r2", "r1",
                            v0_32b);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  auto v42_64b = to64BitMemoryValue(42);
  auto v0_64b = to64BitMemoryValue(0);
  destination1.set(0);
  destination2.set(0);
  destination3.set(123);
  destination4.set(123);
  firstOp.set(41);
  assertRegisterInstruction(memoryImpl, factory64, "add", "d0", "r1", "r2",
                            v42_64b);
  assertRegisterInstruction(memoryImpl, factory64, "add", "d1", "r2", "r1",
                            v42_64b);
  // test 64bit (unsigned) boundary: (2^64 -1) +1 = 18446744073709551615 + 1 = 0
  firstOp.set(18446744073709551615ULL);
  assertRegisterInstruction(memoryImpl, factory64, "add", "d2", "r1", "r2",
                            v0_64b);
  assertRegisterInstruction(memoryImpl, factory64, "add", "d3", "r2", "r1",
                            v0_64b);
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
  auto v42_32b = to32BitMemoryValue(42);
  auto v1 = to32BitMemoryValue(1);
  auto v41 = to32BitMemoryValue(41);
  auto v0_32b = to32BitMemoryValue(0);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "addi",
                             "d0", "r1", v1, v42_32b);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "addi",
                             "d1", "r2", v41, v42_32b);
  // test 32bit (unsigned) boundary: (2^32 -1) + 1 = 4294967295 + 1 = 0
  regOp1.set(4294967295U);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "addi",
                             "d2", "r1", v1, v0_32b);
  // test immediate boundary
  test20BitImmediateBounds(factory32, "addi", immediateFactory);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  auto v42_64b = to64BitMemoryValue(42);
  auto v0_64b = to64BitMemoryValue(0);
  destination1.set(0);
  destination2.set(0);
  destination3.set(123);
  destination4.set(123);
  regOp1.set(41);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "addi",
                             "d0", "r1", v1, v42_64b);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "addi",
                             "d1", "r2", v41, v42_64b);
  // test 64bit (unsigned) boundary: (2^64 -1) +1 = 18446744073709551615 + 1 = 0
  regOp1.set(18446744073709551615ULL);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "addi",
                             "d2", "r1", v1, v0_64b);
  // test immediate boundary
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

TEST(IntegerInstructionTest, SUBInstruction_testSub) {
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
  auto v42_32b = to32BitMemoryValue(42);
  auto max32bitValue = to32BitMemoryValue(0xFFFFFFFFU);
  assertRegisterInstruction(memoryImpl, factory32, "sub", "d0", "r1", "r2",
                            v42_32b);
  // test 32bit (unsigned) boundary: 0 - 1 = (2^32 -1)= 4294967295
  firstOp.set(0);
  assertRegisterInstruction(memoryImpl, factory32, "sub", "d1", "r1", "r2",
                            max32bitValue);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  auto v42_64b = to64BitMemoryValue(42);
  auto max64bitValue = to64BitMemoryValue(0xFFFFFFFFFFFFFFFFULL);
  destination1.set(0);
  destination2.set(123);
  firstOp.set(43);
  assertRegisterInstruction(memoryImpl, factory64, "sub", "d0", "r1", "r2",
                            v42_64b);
  // test 64bit (unsigned) boundary: 0-1 = (2^64 -1) = 18446744073709551615
  firstOp.set(0);
  assertRegisterInstruction(memoryImpl, factory64, "sub", "d1", "r1", "r2",
                            max64bitValue);
}

TEST(IntegerInstructionTest, SUBInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = DummyMemoryAccessImpl();
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "sub", false);
}

TEST(IntegerInstructionTest, ANDInstruction_testAnd) {
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
  auto expPattern32b = to32BitMemoryValue(0b100100);
  auto v0_32b = to32BitMemoryValue(0);
  assertRegisterInstruction(memoryImpl, factory32, "and", "d0", "r1", "r2",
                            expPattern32b);
  assertRegisterInstruction(memoryImpl, factory32, "and", "d1", "r2", "r1",
                            expPattern32b);
  firstOp.set(0b101010);
  secondOp.set(0b010101);
  assertRegisterInstruction(memoryImpl, factory32, "and", "d2", "r1", "r2",
                            v0_32b);
  assertRegisterInstruction(memoryImpl, factory32, "and", "d3", "r2", "r1",
                            v0_32b);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  auto expPattern64b = to64BitMemoryValue(0b100100L << 32);
  auto v0_64b = to64BitMemoryValue(0);
  destination1.set(0);
  destination2.set(0);
  destination3.set(123);
  destination4.set(123);
  firstOp.set(0b110110L << 32);
  secondOp.set(0b100100L << 32);
  assertRegisterInstruction(memoryImpl, factory64, "and", "d0", "r1", "r2",
                            expPattern64b);
  assertRegisterInstruction(memoryImpl, factory64, "and", "d1", "r2", "r1",
                            expPattern64b);
  firstOp.set(0b101010L << 32);
  secondOp.set(0b010101L << 32);
  assertRegisterInstruction(memoryImpl, factory64, "and", "d2", "r1", "r2",
                            v0_64b);
  assertRegisterInstruction(memoryImpl, factory64, "and", "d3", "r2", "r1",
                            v0_64b);
}

TEST(IntegerInstructionTest, ANDInstruction_testAndi) {
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
  auto immediatePattern32b_1 = to32BitMemoryValue(0b100100);
  auto immediatePattern32b_2 = to32BitMemoryValue(0b110110);
  auto immediatePattern32b_3 = to32BitMemoryValue(0b01010101);
  auto expPattern32b = to32BitMemoryValue(0b100100);
  auto v0_32b = to32BitMemoryValue(0);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "andi",
                             "d0", "r1", immediatePattern32b_1, expPattern32b);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "andi",
                             "d1", "r2", immediatePattern32b_2, expPattern32b);
  regOp1.set(0b10101010);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "andi",
                             "d2", "r1", immediatePattern32b_3, v0_32b);
  // test immediate boundary
  test20BitImmediateBounds(factory32, "andi", immediateFactory);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(1);
  destination2.set(1);
  destination3.set(123);
  destination4.set(123);
  regOp1.set(0b110110L << 32);
  regOp2.set(0b100100L);
  // it is difficult to test 64bit version, because immediate values cannot be
  // greater than 20bit
  // therefore no upper 32bit testing can be done, except for testing against 0
  auto pattern64b_1 = to64BitMemoryValue(0b100100);
  auto pattern64b_2 = to64BitMemoryValue(0b110110L);
  auto v0_64b = to64BitMemoryValue(0);
  auto expPattern64b = to64BitMemoryValue(0b100100L);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "andi",
                             "d0", "r1", pattern64b_1, v0_64b);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "andi",
                             "d1", "r2", pattern64b_2, expPattern64b);
  // test immediate boundary
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

TEST(IntegerInstructionTest, ORInstruction_testOr) {
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
  auto pattern32b_1 = to32BitMemoryValue(0b110110);
  auto pattern32b_2 = to32BitMemoryValue(0b111111);
  assertRegisterInstruction(memoryImpl, factory32, "or", "d0", "r1", "r2",
                            pattern32b_1);
  assertRegisterInstruction(memoryImpl, factory32, "or", "d1", "r2", "r1",
                            pattern32b_1);
  firstOp.set(0b101010);
  secondOp.set(0b010101);
  assertRegisterInstruction(memoryImpl, factory32, "or", "d2", "r1", "r2",
                            pattern32b_2);
  assertRegisterInstruction(memoryImpl, factory32, "or", "d3", "r2", "r1",
                            pattern32b_2);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  destination3.set(123);
  destination4.set(123);
  firstOp.set(0b110110L << 32);
  secondOp.set(0b100100L << 32);
  auto pattern64b_1 = to64BitMemoryValue(0b110110L << 32);
  auto pattern64b_2 = to64BitMemoryValue(0b111111L << 32);
  assertRegisterInstruction(memoryImpl, factory64, "or", "d0", "r1", "r2",
                            pattern64b_1);
  assertRegisterInstruction(memoryImpl, factory64, "or", "d1", "r2", "r1",
                            pattern64b_1);
  firstOp.set(0b101010L << 32);
  secondOp.set(0b010101L << 32);
  assertRegisterInstruction(memoryImpl, factory64, "or", "d2", "r1", "r2",
                            pattern64b_2);
  assertRegisterInstruction(memoryImpl, factory64, "or", "d3", "r2", "r1",
                            pattern64b_2);
}

TEST(IntegerInstructionTest, ORInstruction_testOri) {
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
  auto pattern32b_1 = to32BitMemoryValue(0b100100);
  auto exp32b_1 = to32BitMemoryValue(0b110110);
  auto pattern32b_2 = to32BitMemoryValue(0b01010101);
  auto exp32b_2 = to32BitMemoryValue(0b11111111);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "ori",
                             "d0", "r1", pattern32b_1, exp32b_1);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "ori",
                             "d1", "r2", exp32b_1, exp32b_1);
  regOp1.set(0b10101010);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "ori",
                             "d2", "r1", pattern32b_2, exp32b_2);
  // test immediate boundary
  test20BitImmediateBounds(factory32, "ori", immediateFactory);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  destination3.set(123);
  destination4.set(123);
  regOp1.set(0b1010110L << 32);
  auto pattern64b_1 = to64BitMemoryValue(0b10);
  auto exp64b_1 = to64BitMemoryValue((0b1010110L << 32) + 2);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "ori",
                             "d0", "r1", pattern64b_1, exp64b_1);
  // test immediate boundary
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

TEST(IntegerInstructionTest, XORInstruction_testXor) {
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
  auto exp32b_1 = to32BitMemoryValue(0b010010);
  auto exp32b_2 = to32BitMemoryValue(0b111111);
  assertRegisterInstruction(memoryImpl, factory32, "xor", "d0", "r1", "r2",
                            exp32b_1);
  assertRegisterInstruction(memoryImpl, factory32, "xor", "d1", "r2", "r1",
                            exp32b_1);
  firstOp.set(0b101010);
  secondOp.set(0b010101);
  assertRegisterInstruction(memoryImpl, factory32, "xor", "d2", "r1", "r2",
                            exp32b_2);
  assertRegisterInstruction(memoryImpl, factory32, "xor", "d3", "r2", "r1",
                            exp32b_2);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  destination3.set(123);
  destination4.set(123);
  firstOp.set(0b110110L << 32);
  secondOp.set(0b100100L << 32);
  auto exp64b_1 = to64BitMemoryValue(0b010010L << 32);
  auto exp64b_2 = to64BitMemoryValue(0b111111L << 32);
  assertRegisterInstruction(memoryImpl, factory64, "xor", "d0", "r1", "r2",
                            exp64b_1);
  assertRegisterInstruction(memoryImpl, factory64, "xor", "d1", "r2", "r1",
                            exp64b_1);
  firstOp.set(0b101010L << 32);
  secondOp.set(0b010101L << 32);
  assertRegisterInstruction(memoryImpl, factory64, "xor", "d2", "r1", "r2",
                            exp64b_2);
  assertRegisterInstruction(memoryImpl, factory64, "xor", "d3", "r2", "r1",
                            exp64b_2);
}

TEST(IntegerInstructionTest, XORInstruction_testXori) {
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
  auto pattern32b_1 = to32BitMemoryValue(0b100100);
  auto pattern32b_2 = to32BitMemoryValue(0b110110);
  auto exp32b_1 = to32BitMemoryValue(0b010010);
  auto pattern32b_3 = to32BitMemoryValue(0b01010101);
  auto exp32b_2 = to32BitMemoryValue(0b11111111);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "xori",
                             "d0", "r1", pattern32b_1, exp32b_1);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "xori",
                             "d1", "r2", pattern32b_2, exp32b_1);
  regOp1.set(0b10101010);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "xori",
                             "d2", "r1", pattern32b_3, exp32b_2);
  // test immediate boundary
  test20BitImmediateBounds(factory32, "xori", immediateFactory);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  auto pattern64b_1 = to64BitMemoryValue(0b10 << 18);
  auto exp64b_1 = to64BitMemoryValue(0b100000001010100L << 18);
  destination1.set(0);
  destination2.set(0);
  destination3.set(123);
  destination4.set(123);
  regOp1.set(0b100000001010110L << 18);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "xori",
                             "d0", "r1", pattern64b_1, exp64b_1);
  // test immediate boundary
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

TEST(IntegerInstructionTest, ShiftLeftInstruction_testSll) {
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
  auto exp32b_1 = to32BitMemoryValue(0b110110000);
  auto exp32b_2 = to32BitMemoryValue(3 << 22);
  assertRegisterInstruction(memoryImpl, factory32, "sll", "d0", "r1", "r2",
                            exp32b_1);
  // shifts use only the lower 5bit of the second operand
  assertRegisterInstruction(memoryImpl, factory32, "sll", "d1", "r2", "r1",
                            exp32b_2);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  firstOp.set(0b110110L << 32);
  secondOp.set(3);
  auto exp64b_1 = to64BitMemoryValue(0b110110000L << 32);
  auto v3_64b = to64BitMemoryValue(3);
  assertRegisterInstruction(memoryImpl, factory64, "sll", "d0", "r1", "r2",
                            exp64b_1);
  // shifts use only the lower 5bit of the second operand
  assertRegisterInstruction(memoryImpl, factory64, "sll", "d1", "r2", "r1",
                            v3_64b);
}

TEST(IntegerInstructionTest, ShiftLeftInstruction_testSlli) {
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
  auto v3_32b = to32BitMemoryValue(3);
  auto exp32b_1 = to32BitMemoryValue(0b110110000);
  auto immediate32b = to32BitMemoryValue(0b11100001);
  auto v2_32b = to32BitMemoryValue(2);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "slli",
                             "d0", "r1", v3_32b, exp32b_1);
  // shifts use only the lower 5bit of the second operand
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "slli",
                             "d1", "r2", immediate32b, v2_32b);
  // test immediate boundary
  test20BitImmediateBounds(factory32, "slli", immediateFactory);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  destination1.set(0);
  destination2.set(0);
  regOp1.set(0b110110L << 32);
  regOp2.set(0b1L << 32);
  auto v3_64b = to64BitMemoryValue(3);
  auto exp64b_1 = to64BitMemoryValue(0b110110000L << 32);
  auto immediate64b = to64BitMemoryValue(0b11100001);
  auto exp64b_2 = to64BitMemoryValue(0b10L << 32);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "slli",
                             "d0", "r1", v3_64b, exp64b_1);
  // shifts use only the lower 5bit of the second operand
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "slli",
                             "d1", "r2", immediate64b, exp64b_2);

  // test immediate boundary
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
  auto exp32b_1 = to32BitMemoryValue(0b110);
  auto v0_32b = to32BitMemoryValue(0);
  assertRegisterInstruction(memoryImpl, factory32, "srl", "d0", "r1", "r2",
                            exp32b_1);
  // shifts use only the lower 5bit of the second operand
  assertRegisterInstruction(memoryImpl, factory32, "srl", "d1", "r2", "r1",
                            v0_32b);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  auto exp64b_1 = to64BitMemoryValue(0b110110L << 29);
  auto v3_64b = to64BitMemoryValue(3);
  destination1.set(0);
  destination2.set(0);
  firstOp.set(0b110110L << 32);
  secondOp.set(3);
  assertRegisterInstruction(memoryImpl, factory64, "srl", "d0", "r1", "r2",
                            exp64b_1);
  // shifts use only the lower 5bit of the second operand
  assertRegisterInstruction(memoryImpl, factory64, "srl", "d1", "r2", "r1",
                            v3_64b);
}

TEST(IntegerInstructionTest, ShiftRightInstruction_testSrli) {
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
  auto v3_32b = to32BitMemoryValue(3);
  auto exp32b_1 = to32BitMemoryValue(0b110);
  auto immediate32b = to32BitMemoryValue(0b11100001);
  auto exp32b_2 = to32BitMemoryValue(0b11011);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "srli",
                             "d0", "r1", v3_32b, exp32b_1);
  // shifts use only the lower 5bit of the second operand
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "srli",
                             "d1", "r1", immediate32b, exp32b_2);
  // test immediate boundary
  test20BitImmediateBounds(factory32, "srli", immediateFactory);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  auto v3_64b = to64BitMemoryValue(3);
  auto exp64b_1 = to64BitMemoryValue(0b110110L << 29);
  auto immediate64b = to64BitMemoryValue(0b11100001);
  auto exp64b_2 = to64BitMemoryValue(0b11L << 31);
  destination1.set(0);
  destination2.set(0);
  regOp1.set(0b110110L << 32);
  regOp2.set(0b11L << 32);
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "srli",
                             "d0", "r1", v3_64b, exp64b_1);
  // shifts use only the lower 5bit of the second operand
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "srli",
                             "d1", "r2", immediate64b, exp64b_2);

  // test immediate boundary
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
  auto exp32b_1 = to32BitMemoryValue(0b11110000000000000000000000000110);
  auto v0_32b = to32BitMemoryValue(0);
  assertRegisterInstruction(memoryImpl, factory32, "sra", "d0", "r1", "r2",
                            exp32b_1);
  // shifts use only the lower 5bit of the second operand
  assertRegisterInstruction(memoryImpl, factory32, "sra", "d1", "r2", "r1",
                            v0_32b);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  auto exp64b_1 = to64BitMemoryValue(
      0b1111000000000000000000000000001101100000000000000000000000000000L);
  auto v3_64b = to64BitMemoryValue(3);
  destination1.set(0);
  destination2.set(0);
  firstOp.set(
      0b1000000000000000000000000001101100000000000000000000000000000000L);
  secondOp.set(3);
  assertRegisterInstruction(memoryImpl, factory64, "sra", "d0", "r1", "r2",
                            exp64b_1);
  // shifts use only the lower 5bit of the second operand
  assertRegisterInstruction(memoryImpl, factory64, "srl", "d1", "r2", "r1",
                            v3_64b);
}

TEST(IntegerInstructionTest, ShiftRightInstruction_testSrai) {
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
  auto v3_32b = to32BitMemoryValue(3);
  auto exp32b_1 = to32BitMemoryValue(0b11110000000000000000000000000110);
  auto immediate32b = to32BitMemoryValue(0b11100001);
  auto exp32b_2 = to32BitMemoryValue(0b11000000000000000000000000011011);
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "srai",
                             "d0", "r1", v3_32b, exp32b_1);
  // shifts use only the lower 5bit of the second operand
  assertImmediateInstruction(memoryImpl, factory32, immediateFactory, "srai",
                             "d1", "r1", immediate32b, exp32b_2);
  // test immediate boundary
  test20BitImmediateBounds(factory32, "srai", immediateFactory);

  // Test 64Bit versions
  auto factory64 = setUpFactory({"rv32i", "rv64i"});
  auto v3_64b = to64BitMemoryValue(3);
  auto exp64b_1 = to64BitMemoryValue(0b1111000000000000000000000000001101100000000000000000000000000000L);
  auto immediate64b = to64BitMemoryValue(0b11100001);
  auto exp64b_2 = to64BitMemoryValue(0b11L << 31);
  destination1.set(0);
  destination2.set(0);
  regOp1.set(
      0b1000000000000000000000000001101100000000000000000000000000000000L);
  regOp2.set(0b11L << 32);
  assertImmediateInstruction(
      memoryImpl, factory64, immediateFactory, "srai", "d0", "r1", v3_64b,
      exp64b_1);
  // shifts use only the lower 5bit of the second operand
  assertImmediateInstruction(memoryImpl, factory64, immediateFactory, "srai",
                             "d1", "r2", immediate64b, exp64b_2);

  // test immediate boundary
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

TEST(MulDivInstructionTest, Multiplication_testMUL) {
  FakeRegister destination1, destination2;
  FakeRegister op1, op2;
  DummyMemoryAccessImpl memoryImpl;
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  memoryImpl.addRegister("d0", destination1);
  memoryImpl.addRegister("d1", destination2);
  memoryImpl.addRegister("r1", op1);
  memoryImpl.addRegister("r2", op2);
  auto factory32 = setUpFactory({"rv32i", "rv32m"});

  // 42 x 0 => 0
  // 0 x 42 => 0
  op1.set(42);
  op2.set(0);
  assertRegisterInstruction(memoryImpl, factory32, "mul", "d0", "r1", "r2", 0);
  assertRegisterInstruction(memoryImpl, factory32, "mul", "d1", "r2", "r1", 0);

  //13 x 17 => 221
  op1.set(13);
  op2.set(17);
  assertRegisterInstruction(memoryImpl, factory32, "mul", "d0", "r1", "r2", 221);

  //-13 x 17 => -221
  std::cout << "-13x17, ";
  op1.set(-13);
  op2.set(17);
  assertRegisterInstruction(memoryImpl, factory32, "mul", "d0", "r1", "r2", -221);

  //13 x -17 => -221
  std::cout << "13x-17, ";
  op1.set(13);
  op2.set(-17);
  assertRegisterInstruction(memoryImpl, factory32, "mul", "d0", "r1", "r2", -221);

  //-13 x -17 => 221
  std::cout << "-13x-17" << std::endl;
  op1.set(-13);
  op2.set(-17);
  assertRegisterInstruction(memoryImpl, factory32, "mul", "d0", "r1", "r2", 221);

  // test 32bit bounds
  // 2^30 x 2 => 2^31
  // 2^31 x 2 => 0
  op1.set(1 << 30);
  op2.set(2);
  assertRegisterInstruction(memoryImpl, factory32, "mul", "d0", "r1", "r2",
                            (1U << 31));
  destination2.set(1234);  // set the destination register to some random number
                           // != 0, so that the test will fail if the
                           // instruction would not change the register value
  op1.set(1 << 31);
  assertRegisterInstruction(memoryImpl, factory32, "mul", "d1", "r1", "r2", 0);
}

TEST(MulDivInstructionTest, Multiplication_testValidation) {
    auto instructionFactory = setUpFactory({"rv32i", "rv32m"});
    auto immediateFactory = ImmediateNodeFactory();
    auto memAccess = DummyMemoryAccessImpl();
    testIntegerInstructionValidation(memAccess, instructionFactory,
                                     immediateFactory, "mul", false);
}
