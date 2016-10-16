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

#include "arithmetic-test-utils.hpp"
#include "tests/arch/riscv/common.hpp"

using namespace riscv;

TEST(IntegerInstructionTest, ADDInstruction_testAdd32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
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
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
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
  Register destination, operand;
  std::string dest("d0"), reg("r1");
  MemoryAccess memoryAccess;
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
  Register destination, operand;
  std::string dest("d0"), reg("r1");
  MemoryAccess memoryAccess;
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
  auto memAccess = MemoryAccess();
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "add", false);
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "addi", true);
}

TEST(IntegerInstructionTest, SUBInstruction_testSub32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
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
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
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
  auto memAccess = MemoryAccess();
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "sub", false);
}

TEST(IntegerInstructionTest, ANDInstruction_testAnd32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
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
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RR(0,
          to64BitMemoryValue,
          "and",
          0b110110LL << 32,
          0b100100LL << 32,
          0b100100LL << 32);
  TEST_RR(1,
          to64BitMemoryValue,
          "and",
          0b100100LL << 32,
          0b110110LL << 32,
          0b100100LL << 32);
  TEST_RR(2, to64BitMemoryValue, "and", 0b101010LL << 32, 0b010101LL << 32, 0);
  TEST_RR(3, to64BitMemoryValue, "and", 0b010101LL << 32, 0b101010LL << 32, 0);
}

TEST(IntegerInstructionTest, ANDInstruction_testAndi32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand;
  std::string dest("d0"), reg("r1");
  MemoryAccess memoryAccess;
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
  Register destination, operand;
  std::string dest("d0"), reg("r1");
  MemoryAccess memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});
  // it is difficult to test 64bit version, because immediate values cannot be
  // greater than 20bit
  // therefore no upper 32bit testing can be done, except for testing against 0
  TEST_RI(0, to64BitMemoryValue, "andi", 0b110110LL << 32, 0b100100, 0);
  TEST_RI(1, to64BitMemoryValue, "andi", 0b100100LL, 0b110110LL, 0b100100LL);

  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "andi", immediateFactory);
}

TEST(IntegerInstructionTest, ANDInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = MemoryAccess();
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "and", false);
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "andi", true);
}

TEST(IntegerInstructionTest, ORInstruction_testOr32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
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
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RR(0,
          to64BitMemoryValue,
          "or",
          0b110110LL << 32,
          0b100100LL << 32,
          0b110110LL << 32);
  TEST_RR(1,
          to64BitMemoryValue,
          "or",
          0b100100LL << 32,
          0b110110LL << 32,
          0b110110LL << 32);
  TEST_RR(2,
          to64BitMemoryValue,
          "or",
          0b101010LL << 32,
          0b010101LL << 32,
          0b111111LL << 32);
  TEST_RR(3,
          to64BitMemoryValue,
          "or",
          0b010101LL << 32,
          0b101010LL << 32,
          0b111111LL << 32);
}

TEST(IntegerInstructionTest, ORInstruction_testOri32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand;
  std::string dest("d0"), reg("r1");
  MemoryAccess memoryAccess;
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
  Register destination, operand;
  std::string dest("d0"), reg("r1");
  MemoryAccess memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RI(0,
          to64BitMemoryValue,
          "ori",
          (0b1010110LL << 32),
          0b10,
          ((0b1010110LL << 32) + 2));
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "ori", immediateFactory);
}

TEST(IntegerInstructionTest, ORInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = MemoryAccess();
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "or", false);
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "ori", true);
}

TEST(IntegerInstructionTest, XORInstruction_testXor32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
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
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RR(0,
          to64BitMemoryValue,
          "xor",
          0b110110LL << 32,
          0b100100LL << 32,
          0b010010LL << 32);
  TEST_RR(1,
          to64BitMemoryValue,
          "xor",
          0b100100LL << 32,
          0b110110LL << 32,
          0b010010LL << 32);
  TEST_RR(2,
          to64BitMemoryValue,
          "xor",
          0b101010LL << 32,
          0b010101LL << 32,
          0b111111LL << 32);
  TEST_RR(3,
          to64BitMemoryValue,
          "xor",
          0b010101LL << 32,
          0b101010LL << 32,
          0b111111LL << 32);
}

TEST(IntegerInstructionTest, XORInstruction_testXori32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand;
  std::string dest("d0"), reg("r1");
  MemoryAccess memoryAccess;
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
  Register destination, operand;
  std::string dest("d0"), reg("r1");
  MemoryAccess memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RI(0,
          to64BitMemoryValue,
          "xori",
          (0b100000001010110LL << 5),
          (0b10LL << 5),
          (0b100000001010100LL << 5));
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "xori", immediateFactory);
}

TEST(IntegerInstructionTest, XORInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = MemoryAccess();
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "xor", false);
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "xori", true);
}

TEST(IntegerInstructionTest, ShiftLeftInstruction_testSll32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
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
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RR(
      0, to64BitMemoryValue, "sll", 0b110110LL << 32, 3, 0b110110000LL << 32);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1, to64BitMemoryValue, "sll", 3, 0b110110LL << 32, 3);
}

TEST(IntegerInstructionTest, ShiftLeftInstruction_testSlli32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand;
  std::string dest("d0"), reg("r1");
  MemoryAccess memoryAccess;
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
  Register destination, operand;
  std::string dest("d0"), reg("r1");
  MemoryAccess memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RI(0,
          to64BitMemoryValue,
          "slli",
          (0b110110LL << 32),
          3,
          (0b110110000LL << 32));
  // shifts use only the lower 5bit of the second operand
  TEST_RI(
      1, to64BitMemoryValue, "slli", (0b1LL << 32), 0b11100001, 0b10LL << 32);
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "slli", immediateFactory);
}

TEST(IntegerInstructionTest, ShiftLeftInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = MemoryAccess();
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "sll", false);
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "slli", true);
}

TEST(IntegerInstructionTest, ShiftRightInstruction_testSrl32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
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
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RR(0, to64BitMemoryValue, "srl", 0b110110LL << 32, 3, 0b110110LL << 29);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1, to64BitMemoryValue, "srl", 3, 0b110110LL << 32, 3);
}

TEST(IntegerInstructionTest, ShiftRightInstruction_testSrli32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand;
  std::string dest("d0"), reg("r1");
  MemoryAccess memoryAccess;
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
  Register destination, operand;
  std::string dest("d0"), reg("r1");
  MemoryAccess memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RI(0, to64BitMemoryValue, "srli", 0b110110LL << 32, 3, 0b110110LL << 29);
  // shifts use only the lower 5bit of the second operand
  TEST_RI(
      1, to64BitMemoryValue, "srli", 0b11LL << 32, 0b11100001, 0b11LL << 31);
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "srli", immediateFactory);
}

TEST(IntegerInstructionTest, ShiftRightInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = MemoryAccess();
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "srl", false);
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "srli", true);
}

TEST(IntegerInstructionTest, ShiftRightArithmeticInstruction_testSra32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RR(0,
          to32BitMemoryValue,
          "sra",
          0b10000000000000000000000000110110,
          3,
          0b11110000000000000000000000000110);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(
      1, to32BitMemoryValue, "sra", 3, 0b10000000000000000000000000110110, 0);
}

TEST(IntegerInstructionTest, ShiftRightArithmeticInstruction_testSra64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand1, operand2;
  std::string dest("d0"), op1("r1"), op2("r2");
  MemoryAccess memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(op1, operand1);
  memoryAccess.addRegister(op2, operand2);
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RR(0,
          to64BitMemoryValue,
          "sra",
          0b1000000000000000000000000001101100000000000000000000000000000000LL,
          3,
          0b1111000000000000000000000000001101100000000000000000000000000000LL);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1,
          to64BitMemoryValue,
          "sra",
          3,
          0b1000000000000000000000000001101100000000000000000000000000000000LL,
          3);
}

TEST(IntegerInstructionTest, ShiftRightInstruction_testSrai32) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand;
  std::string dest("d0"), reg("r1");
  MemoryAccess memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i"});

  TEST_RI(0,
          to32BitMemoryValue,
          "srai",
          0b10000000000000000000000000110110,
          3,
          0b11110000000000000000000000000110);
  // shifts use only the lower 5bit of the second operand
  TEST_RI(1,
          to32BitMemoryValue,
          "srai",
          0b10000000000000000000000000110110,
          0b11100001,
          0b11000000000000000000000000011011);
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "srai", immediateFactory);
}

TEST(IntegerInstructionTest, ShiftRightInstruction_testSrai64) {
  // No real risc-v register names are used as Memory/RegisterAccess is faked
  Register destination, operand;
  std::string dest("d0"), reg("r1");
  MemoryAccess memoryAccess;
  memoryAccess.addRegister(dest, destination);
  memoryAccess.addRegister(reg, operand);
  auto immediateFactory = ImmediateNodeFactory();
  auto instructionFactory = setUpFactory({"rv32i", "rv64i"});

  TEST_RI(0,
          to64BitMemoryValue,
          "srai",
          0b1000000000000000000000000001101100000000000000000000000000000000LL,
          3,
          0b1111000000000000000000000000001101100000000000000000000000000000LL);
  // shifts use only the lower 5bit of the second operand
  TEST_RI(
      1, to64BitMemoryValue, "srai", 0b11LL << 32, 0b11100001, 0b11LL << 31);
  // test immediate boundary
  test12BitImmediateBounds(instructionFactory, "srai", immediateFactory);
}

TEST(IntegerInstructionTest, ShiftRightArithmeticInstruction_testValidation) {
  auto instructionFactory = setUpFactory({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = MemoryAccess();
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "sra", false);
  testIntegerInstructionValidation(
      memAccess, instructionFactory, immediateFactory, "srai", true);
}
