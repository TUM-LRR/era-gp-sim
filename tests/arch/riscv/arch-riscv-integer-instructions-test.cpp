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

void test12BitImmediateBounds(InstructionNodeFactory& instrF,
                              std::string instructionToken,
                              ImmediateNodeFactory& immF) {
  constexpr uint64_t boundary = 0xFFF;
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
  node2->addChild(std::move(immediateNodeOut));
  //TODO enable when IntegerInstructionNode::validate() is fixed
  //ASSERT_FALSE(node2->validate());
}
}
/**
  * This macro performs a register-register test for the given instruction,
 * operands.
  * Apart from the parameters, this macro needs certain exactly named
 * declarations of certain types to compile. This is done to keep the macro
 * argument list short.
 * dest: std::string token, that defines the name of the destination register
 * op1: std::string token, that defines the name of the first operand register
 * op2: std::string token, that defines the name of the second operand register
 * memoryAccess DummyMemoryAccess, where the required registers are added
 * instructionFactory InstructionFactory where nodes with the given
 * instruction-token can be created
 *
 * \param contextNbr running number for variable naming
 * \param memoryValueConverter function that returns a MemoryValue for a given
 * operand-value
 * \param instruction instruction to test (as string literal)
 * \param operand1 first operand value (as literal)
 * \param operand2 second operand value (as literal)
 * \param result expected result of the operation (as literal)
  */
#define TEST_RR(contextNbr, memoryValueConverter, instruction, operand1,       \
                operand2, result)                                              \
  /* Put operand values into register */                                       \
  memoryAccess.setRegisterValue(op1, memoryValueConverter(operand1));          \
  memoryAccess.setRegisterValue(op2, memoryValueConverter(operand2));          \
  auto cmd_##contextNbr =                                                      \
      instructionFactory.createInstructionNode(instruction);                   \
  ASSERT_FALSE(cmd_##contextNbr->validate());                                  \
  /*Assemble instruction with destination & operands*/                         \
  cmd_##contextNbr->addChild(std::make_unique<FakeRegisterNode>(dest));        \
  ASSERT_FALSE(cmd_##contextNbr->validate());                                  \
  cmd_##contextNbr->addChild(std::make_unique<FakeRegisterNode>(op1));         \
  ASSERT_FALSE(cmd_##contextNbr->validate());                                  \
  cmd_##contextNbr->addChild(std::make_unique<FakeRegisterNode>(op2));         \
  ASSERT_TRUE(cmd_##contextNbr->validate());                                   \
  /* Save values of operand registers to determine change*/                    \
  MemoryValue preOp1_##contextNbr = memoryAccess.getRegisterValue(op1);        \
  MemoryValue preOp2_##contextNbr = memoryAccess.getRegisterValue(op2);        \
  /* Perform instruction*/                                                     \
  MemoryValue returnValue_##contextNbr =                                       \
      cmd_##contextNbr->getValue(memoryAccess);                                \
  ASSERT_EQ(memoryValueConverter(0), returnValue_##contextNbr);                \
  /* Check that operand registers stayed the same*/                            \
  ASSERT_EQ(preOp1_##contextNbr, memoryAccess.getRegisterValue(op1));          \
  ASSERT_EQ(preOp2_##contextNbr, memoryAccess.getRegisterValue(op2));          \
  /* Read result from destination register*/                                   \
  MemoryValue actualResult_##contextNbr = memoryAccess.getRegisterValue(dest); \
  ASSERT_EQ(memoryValueConverter(result), actualResult_##contextNbr);

/**
  * This macro performs a register-immediate test for the given instruction,
 * operands
  * Apart from the parameters, this macro needs certain exactly named
 * declarations of certain types to compile. This is done to keep the macro
 * argument list short
 * dest: std::string token, that defines the name of the destination register
 * reg: std::string token, that defines the name of the operand register
 * memoryAccess DummyMemoryAccess, where the required registers are added
 * instructionFactory InstructionFactory where nodes with the given
 * instruction-token can be created
 * immediateFactory ImmediateFactory, where Immediate-Nodes can be created
 *
 * \param contextNbr running number for variable naming
 * \param memoryValueConverter function that returns a MemoryValue for a given
 * operand-value
 * \param instruction instruction to test (as string literal)
 * \param operand1 first operand value (as literal)
 * \param operand2 second operand value (as literal)
 * \param result expected result of the operation (as literal)
  */
#define TEST_RI(contextNbr, memoryValueConverter, instruction, operand1,       \
                operand2, result)                                              \
  memoryAccess.setRegisterValue(reg, memoryValueConverter(operand1));          \
  /* Assemble instruction node with destination, operand & immediate node*/    \
  auto cmd_##contextNbr =                                                      \
      instructionFactory.createInstructionNode(instruction);                   \
  ASSERT_FALSE(cmd_##contextNbr->validate())                                   \
      << "empty instruction node validation failed";                           \
  cmd_##contextNbr->addChild(std::make_unique<FakeRegisterNode>(dest));        \
  ASSERT_FALSE(cmd_##contextNbr->validate())                                   \
      << "instruction node + destination register node validation failed";     \
  cmd_##contextNbr->addChild(std::make_unique<FakeRegisterNode>(reg));         \
  ASSERT_FALSE(cmd_##contextNbr->validate())                                   \
      << "instruction node + 2 register nodes validation failed";              \
  cmd_##contextNbr->addChild(                                                  \
      immediateFactory.createImmediateNode(memoryValueConverter(operand2)));   \
  ASSERT_TRUE(cmd_##contextNbr->validate())                                    \
      << "instruction node + 2 register + immediate node validation failed";   \
  /* Save value of operand register to determine change */                     \
  MemoryValue preRegisterOp_##contextNbr = memoryAccess.getRegisterValue(reg); \
  /* Perform instruction*/                                                     \
  MemoryValue returnValue_##contextNbr =                                       \
      cmd_##contextNbr->getValue(memoryAccess);                                \
  ASSERT_EQ(memoryValueConverter(0), returnValue_##contextNbr);                \
  /* Check that register operand stayed the same*/                             \
  ASSERT_EQ(preRegisterOp_##contextNbr, memoryAccess.getRegisterValue(reg));   \
  /* Read result from destination register */                                  \
  MemoryValue result_##contextNbr = memoryAccess.getRegisterValue(dest);       \
  ASSERT_EQ(memoryValueConverter(result), result_##contextNbr);

TEST(IntegerInstructionTest, ADDInstruction_testAdd32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RR(0, to32BitMemoryValue, "add", 41, 1, 42);
  TEST_RR(1, to32BitMemoryValue, "add", 1, 41, 42);
  // test 32bit (unsigned) boundary: (2^32 -1) + 1 = 4294967295 + 1 = 0
  TEST_RR(2, to32BitMemoryValue, "add", 0xFFFFFFFFU, 1, 0);
  TEST_RR(3, to32BitMemoryValue, "add", 1, 0xFFFFFFFFU, 0);
}

TEST(IntegerInstructionTest, ADDInstruction_testAdd64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RR(0, to64BitMemoryValue, "add", 41, 1, 42);
  TEST_RR(1, to64BitMemoryValue, "add", 1, 41, 42);
  // test 64bit (unsigned) boundary: (2^64 -1) +1 = 18446744073709551615 + 1 = 0
  TEST_RR(2, to64BitMemoryValue, "add", 0xFFFFFFFFFFFFFFFFULL, 1, 0);
  TEST_RR(3, to64BitMemoryValue, "add", 1, 0xFFFFFFFFFFFFFFFFULL, 0);
}

TEST(IntegerInstructionTest, ADDInstruction_testAddi32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand;
  std::string dest("d0"), reg("r1");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RI(0, to32BitMemoryValue, "addi", 41, 1, 42);
  TEST_RI(1, to32BitMemoryValue, "addi", 1, 41, 42);
  // test 32bit (unsigned) boundary: (2^32 -1) + 1 = 4294967295 + 1 = 0
  TEST_RI(2, to32BitMemoryValue, "addi", 0xFFFFFFFFU, 1, 0);
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "addi", immediateFactory);
}
TEST(IntegerInstructionTest, ADDInstruction_testAddi64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand;
  std::string dest("d0"), reg("r1");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RI(0, to64BitMemoryValue, "addi", 41, 1, 42);
  TEST_RI(1, to64BitMemoryValue, "addi", 1, 41, 42);
  // test 64bit (unsigned) boundary: (2^64 -1) +1 = 18446744073709551615 + 1 = 0
  TEST_RI(2, to64BitMemoryValue, "addi", 0xFFFFFFFFFFFFFFFFULL, 1, 0);
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "addi", immediateFactory);
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

TEST(IntegerInstructionTest, SUBInstruction_testSub32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RR(0, to32BitMemoryValue, "sub", 43, 1, 42);
  // test 32bit (unsigned) boundary: 0 - 1 = (2^32 -1)= 4294967295
  TEST_RR(1, to32BitMemoryValue, "sub", 0, 1, 0xFFFFFFFFU);
}

TEST(IntegerInstructionTest, SUBInstruction_testSub64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RR(0, to64BitMemoryValue, "sub", 43, 1, 42);
  // test 64bit (unsigned) boundary: 0-1 = (2^64 -1) = 18446744073709551615
  TEST_RR(1, to64BitMemoryValue, "sub", 0, 1, 0xFFFFFFFFFFFFFFFFULL);
}

TEST(IntegerInstructionTest, SUBInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = DummyMemoryAccessImpl();
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "sub", false);
}

TEST(IntegerInstructionTest, ANDInstruction_testAnd32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RR(0, to32BitMemoryValue, "and", 0b110110, 0b100100, 0b100100);
  TEST_RR(1, to32BitMemoryValue, "and", 0b100100, 0b110110, 0b100100);
  TEST_RR(2, to32BitMemoryValue, "and", 0b101010, 0b010101, 0b000000);
  TEST_RR(3, to32BitMemoryValue, "and", 0b010101, 0b101010, 0b000000);
}

TEST(IntegerInstructionTest, ANDInstruction_testAnd64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RR(0, to64BitMemoryValue, "and", 0b110110L << 32, 0b100100L << 32,
          0b100100L << 32);
  TEST_RR(1, to64BitMemoryValue, "and", 0b100100L << 32, 0b110110L << 32,
          0b100100L << 32);
  TEST_RR(2, to64BitMemoryValue, "and", 0b101010L << 32, 0b010101L << 32, 0);
  TEST_RR(3, to64BitMemoryValue, "and", 0b010101L << 32, 0b101010L << 32, 0);
}

TEST(IntegerInstructionTest, ANDInstruction_testAndi32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand;
  std::string dest("d0"), reg("r1");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RI(0, to32BitMemoryValue, "andi", 0b110110, 0b100100, 0b100100);
  TEST_RI(1, to32BitMemoryValue, "andi", 0b100100, 0b110110, 0b100100);
  TEST_RI(2, to32BitMemoryValue, "andi", 0b101010, 0b010101, 0);
  TEST_RI(3, to32BitMemoryValue, "andi", 0b010101, 0b101010, 0);

  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "andi", immediateFactory);
}

TEST(IntegerInstructionTest, ANDInstruction_testAndi64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand;
  std::string dest("d0"), reg("r1");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});
  // it is difficult to test 64bit version, because immediate values cannot be
  // greater than 20bit
  // therefore no upper 32bit testing can be done, except for testing against 0
  TEST_RI(0, to64BitMemoryValue, "andi", 0b110110L << 32, 0b100100, 0);
  TEST_RI(1, to64BitMemoryValue, "andi", 0b100100L, 0b110110L, 0b100100L);

  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "andi", immediateFactory);
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

TEST(IntegerInstructionTest, ORInstruction_testOr32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RR(0, to32BitMemoryValue, "or", 0b110110, 0b100100, 0b110110);
  TEST_RR(1, to32BitMemoryValue, "or", 0b100100, 0b110110, 0b110110);
  TEST_RR(2, to32BitMemoryValue, "or", 0b101010, 0b010101, 0b111111);
  TEST_RR(3, to32BitMemoryValue, "or", 0b010101, 0b101010, 0b111111);
}

TEST(IntegerInstructionTest, ORINstruction_testOr64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RR(0, to64BitMemoryValue, "or", 0b110110L << 32, 0b100100L << 32,
          0b110110L << 32);
  TEST_RR(1, to64BitMemoryValue, "or", 0b100100L << 32, 0b110110L << 32,
          0b110110L << 32);
  TEST_RR(2, to64BitMemoryValue, "or", 0b101010L << 32, 0b010101L << 32,
          0b111111L << 32);
  TEST_RR(3, to64BitMemoryValue, "or", 0b010101L << 32, 0b101010L << 32,
          0b111111L << 32);
}

TEST(IntegerInstructionTest, ORInstruction_testOri32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand;
  std::string dest("d0"), reg("r1");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RI(0, to32BitMemoryValue, "ori", 0b110110, 0b100100, 0b110110);
  TEST_RI(1, to32BitMemoryValue, "ori", 0b100100, 0b110110, 0b110110);
  TEST_RI(2, to32BitMemoryValue, "ori", 0b101010, 0b010101, 0b111111);
  TEST_RI(3, to32BitMemoryValue, "ori", 0b010101, 0b101010, 0b111111);
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "ori", immediateFactory);
}
TEST(IntegerInstructionTest, ORInstruction_testOri64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand;
  std::string dest("d0"), reg("r1");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RI(0, to64BitMemoryValue, "ori", (0b1010110L << 32), 0b10,
          ((0b1010110L << 32) + 2));
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "ori", immediateFactory);
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

TEST(IntegerInstructionTest, XORInstruction_testXor32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RR(0, to32BitMemoryValue, "xor", 0b110110, 0b100100, 0b010010);
  TEST_RR(1, to32BitMemoryValue, "xor", 0b100100, 0b110110, 0b010010);
  TEST_RR(2, to32BitMemoryValue, "xor", 0b101010, 0b010101, 0b111111);
  TEST_RR(3, to32BitMemoryValue, "xor", 0b010101, 0b101010, 0b111111);
}
TEST(IntegerInstructionTest, XORInstruction_testXor64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RR(0, to64BitMemoryValue, "xor", 0b110110L << 32, 0b100100L << 32,
          0b010010L << 32);
  TEST_RR(1, to64BitMemoryValue, "xor", 0b100100L << 32, 0b110110L << 32,
          0b010010L << 32);
  TEST_RR(2, to64BitMemoryValue, "xor", 0b101010L << 32, 0b010101L << 32,
          0b111111L << 32);
  TEST_RR(3, to64BitMemoryValue, "xor", 0b010101L << 32, 0b101010L << 32,
          0b111111L << 32);
}

TEST(IntegerInstructionTest, XORInstruction_testXori32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand;
  std::string dest("d0"), reg("r1");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RI(0, to32BitMemoryValue, "xori", 0b110110, 0b100100, 0b010010);
  TEST_RI(1, to32BitMemoryValue, "xori", 0b100100, 0b110110, 0b010010);
  TEST_RI(2, to32BitMemoryValue, "xori", 0b101010, 0b010101, 0b111111);
  TEST_RI(3, to32BitMemoryValue, "xori", 0b010101, 0b101010, 0b111111);
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "xori", immediateFactory);
}

TEST(IntegerInstructionTest, XORInstruction_testXori64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand;
  std::string dest("d0"), reg("r1");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RI(0, to64BitMemoryValue, "xori", (0b100000001010110L << 18),
          (0b10L << 18), (0b100000001010100L << 18));
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "xori", immediateFactory);
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

TEST(IntegerInstructionTest, ShiftLeftInstruction_testSll32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RR(0, to32BitMemoryValue, "sll", 0b110110, 3, 0b110110000);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1, to32BitMemoryValue, "sll", 3, 0b110110, 3 << 22);
}

TEST(IntegerInstructionTest, ShiftLeftInstruction_testSll64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RR(0, to64BitMemoryValue, "sll", 0b110110L << 32, 3, 0b110110000L << 32);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1, to64BitMemoryValue, "sll", 3, 0b110110L << 32, 3);
}

TEST(IntegerInstructionTest, ShiftLeftInstruction_testSlli32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand;
  std::string dest("d0"), reg("r1");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RI(0, to32BitMemoryValue, "slli", 0b110110, 3, 0b110110000);
  // shifts use only the lower 5bit of the second operand
  TEST_RI(1, to32BitMemoryValue, "slli", 1, 0b11100001, 2);
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "slli", immediateFactory);
}
TEST(IntegerInstructionTest, ShiftLeftInstruction_testSlli64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand;
  std::string dest("d0"), reg("r1");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RI(0, to64BitMemoryValue, "slli", (0b110110L << 32), 3,
          (0b110110000L << 32));
  // shifts use only the lower 5bit of the second operand
  TEST_RI(1, to64BitMemoryValue, "slli", (0b1L << 32), 0b11100001, 0b10L << 32);
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "slli", immediateFactory);
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

TEST(IntegerInstructionTest, ShiftRightInstruction_testSrl32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RR(0, to32BitMemoryValue, "srl", 0b110110, 3, 0b110);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1, to32BitMemoryValue, "srl", 3, 0b110110, 0);
}

TEST(IntegerInstructionTest, ShiftRightInstruction_testSrl64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RR(0, to64BitMemoryValue, "srl", 0b110110L << 32, 3, 0b110110L << 29);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1, to64BitMemoryValue, "srl", 3, 0b110110L << 32, 3);
}

TEST(IntegerInstructionTest, ShiftRightInstruction_testSrli32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand;
  std::string dest("d0"), reg("r1");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RI(0, to32BitMemoryValue, "srli", 0b110110, 3, 0b110);
  // shifts use only the lower 5bit of the second operand
  TEST_RI(1, to32BitMemoryValue, "srli", 0b11, 0b11100001, 0b1);
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "srli", immediateFactory);
}
TEST(IntegerInstructionTest, ShiftRightInstruction_testSrli64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand;
  std::string dest("d0"), reg("r1");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RI(0, to64BitMemoryValue, "srli", 0b110110L << 32, 3, 0b110110L << 29);
  // shifts use only the lower 5bit of the second operand
  TEST_RI(1, to64BitMemoryValue, "srli", 0b11L << 32, 0b11100001, 0b11L << 31);
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "srli", immediateFactory);
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

TEST(IntegerInstructionTest, ShiftRightArithmeticInstruction_testSra32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RR(0, to32BitMemoryValue, "sra", 0b10000000000000000000000000110110, 3,
          0b11110000000000000000000000000110);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1, to32BitMemoryValue, "sra", 3, 0b10000000000000000000000000110110,
          0);
}

TEST(IntegerInstructionTest, ShiftRightArithmeticInstruction_testSra64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RR(0, to64BitMemoryValue, "sra",
          0b1000000000000000000000000001101100000000000000000000000000000000L,
          3,
          0b1111000000000000000000000000001101100000000000000000000000000000L);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1, to64BitMemoryValue, "sra", 3,
          0b1000000000000000000000000001101100000000000000000000000000000000L,
          3);
}

TEST(IntegerInstructionTest, ShiftRightInstruction_testSrai32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand;
  std::string dest("d0"), reg("r1");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RI(0, to32BitMemoryValue, "srai", 0b10000000000000000000000000110110, 3,
          0b11110000000000000000000000000110);
  // shifts use only the lower 5bit of the second operand
  TEST_RI(1, to32BitMemoryValue, "srai", 0b10000000000000000000000000110110,
          0b11100001, 0b11000000000000000000000000011011);
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "srai", immediateFactory);
}

TEST(IntegerInstructionTest, ShiftRightInstruction_testSrai64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  FakeRegister destination, operand;
  std::string dest("d0"), reg("r1");
  DummyMemoryAccessImpl memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RI(0, to64BitMemoryValue, "srai",
          0b1000000000000000000000000001101100000000000000000000000000000000L,
          3,
          0b1111000000000000000000000000001101100000000000000000000000000000L);
  // shifts use only the lower 5bit of the second operand
  TEST_RI(1, to64BitMemoryValue, "srai", 0b11L << 32, 0b11100001, 0b11L << 31);
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "srai", immediateFactory);
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
