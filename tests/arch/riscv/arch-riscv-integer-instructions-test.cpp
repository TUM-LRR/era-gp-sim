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
#include "arch/common/architecture.hpp"
#include "arch/common/immediate-node.hpp"
#include "arch/common/register-node.hpp"
#include "arch/riscv/immediate-node-factory.hpp"
#include "arch/riscv/instruction-node-factory.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "core/conversions.hpp"

#include "arithmetic-test-utils.hpp"
#include "tests/arch/riscv/test-utils.hpp"

using namespace riscv;

struct IntegerInstructionTest : public RiscvBaseTest {
  IntegerInstructionTest() : dest("x1"), op1("x2"), op2("x3"), reg("x4") {}

  std::string dest, op1, op2, reg;
};

TEST_F(IntegerInstructionTest, ADDInstruction_testAdd32) {
  load({"rv32i"});

  MemoryAccess memoryAccess = getMemoryAccess();
  TEST_RR(0, convert<uint32_t>, "add", 41, 1, 42);
  TEST_RR(1, convert<uint32_t>, "add", 1, 41, 42);
  // test 32bit (unsigned) boundary: (2^32 -1) + 1 = 4294967295 + 1 = 0
  TEST_RR(2, convert<uint32_t>, "add", 0xFFFFFFFFU, 1, 0);
  TEST_RR(3, convert<uint32_t>, "add", 1, 0xFFFFFFFFU, 0);
}

TEST_F(IntegerInstructionTest, ADDInstruction_testAdd64) {
  load({"rv32i", "rv64i"});

  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "add", 41, 1, 42);
  TEST_RR(1, convert<uint64_t>, "add", 1, 41, 42);
  // test 64bit (unsigned) boundary: (2^64 -1) +1 = 18446744073709551615 + 1 = 0
  TEST_RR(2, convert<uint64_t>, "add", 0xFFFFFFFFFFFFFFFFULL, 1, 0);
  TEST_RR(3, convert<uint64_t>, "add", 1, 0xFFFFFFFFFFFFFFFFULL, 0);
}

TEST_F(IntegerInstructionTest, ADDInstruction_testAddi32) {
  load({"rv32i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  auto immediateFactory = ImmediateNodeFactory();

  TEST_RI(0, convert<uint32_t>, "addi", 41, 1, 42);
  TEST_RI(1, convert<uint32_t>, "addi", 1, 41, 42);
  // test 32bit (unsigned) boundary: (2^32 -1) + 1 = 4294967295 + 1 = 0
  TEST_RI(2, convert<uint32_t>, "addi", 0xFFFFFFFFU, 1, 0);
  // test immediate boundary
  test12BitImmediateBounds(getFactories(), "addi", immediateFactory, memoryAccess);
}
TEST_F(IntegerInstructionTest, ADDInstruction_testAddi64) {
  load({"rv32i", "rv64i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  auto immediateFactory = ImmediateNodeFactory();

  TEST_RI(0, convert<uint64_t>, "addi", 41, 1, 42);
  TEST_RI(1, convert<uint64_t>, "addi", 1, 41, 42);
  // test 64bit (unsigned) boundary: (2^64 -1) +1 = 18446744073709551615 + 1 =
  // 0
  TEST_RI(2, convert<uint64_t>, "addi", 0xFFFFFFFFFFFFFFFFULL, 1, 0);
  // test immediate boundary
  test12BitImmediateBounds(getFactories(), "addi", immediateFactory, memoryAccess);
}

TEST_F(IntegerInstructionTest, ADDInstruction_testValidation) {
  load({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = getMemoryAccess();
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "add", false);
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "addi", true);
}

TEST_F(IntegerInstructionTest, SUBInstruction_testSub32) {
  load({"rv32i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "sub", 43, 1, 42);
  // test 32bit (unsigned) boundary: 0 - 1 = (2^32 -1)= 4294967295
  TEST_RR(1, convert<uint32_t>, "sub", 0, 1, 0xFFFFFFFFU);
}

TEST_F(IntegerInstructionTest, SUBInstruction_testSub64) {
  load({"rv32i", "rv64i"});

  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "sub", 43, 1, 42);
  // test 64bit (unsigned) boundary: 0-1 = (2^64 -1) = 18446744073709551615
  TEST_RR(1, convert<uint64_t>, "sub", 0, 1, 0xFFFFFFFFFFFFFFFFULL);
}

TEST_F(IntegerInstructionTest, SUBInstruction_testValidation) {
  load({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = getMemoryAccess();
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "sub", false);
}

TEST_F(IntegerInstructionTest, ANDInstruction_testAnd32) {
  load({"rv32i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "and", 0b110110, 0b100100, 0b100100);
  TEST_RR(1, convert<uint32_t>, "and", 0b100100, 0b110110, 0b100100);
  TEST_RR(2, convert<uint32_t>, "and", 0b101010, 0b010101, 0b000000);
  TEST_RR(3, convert<uint32_t>, "and", 0b010101, 0b101010, 0b000000);
}

TEST_F(IntegerInstructionTest, ANDInstruction_testAnd64) {
  load({"rv32i", "rv64i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "and", 0b110110LL << 32, 0b100100LL << 32,
          0b100100LL << 32);
  TEST_RR(1, convert<uint64_t>, "and", 0b100100LL << 32, 0b110110LL << 32,
          0b100100LL << 32);
  TEST_RR(2, convert<uint64_t>, "and", 0b101010LL << 32, 0b010101LL << 32, 0);
  TEST_RR(3, convert<uint64_t>, "and", 0b010101LL << 32, 0b101010LL << 32, 0);
}

TEST_F(IntegerInstructionTest, ANDInstruction_testAndi32) {
  load({"rv32i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  auto immediateFactory = ImmediateNodeFactory();

  TEST_RI(0, convert<uint32_t>, "andi", 0b110110, 0b100100, 0b100100);
  TEST_RI(1, convert<uint32_t>, "andi", 0b100100, 0b110110, 0b100100);
  TEST_RI(2, convert<uint32_t>, "andi", 0b101010, 0b010101, 0);
  TEST_RI(3, convert<uint32_t>, "andi", 0b010101, 0b101010, 0);

  // test immediate boundary
  test12BitImmediateBounds(getFactories(), "andi", immediateFactory, memoryAccess);
}

TEST_F(IntegerInstructionTest, ANDInstruction_testAndi64) {
  load({"rv32i", "rv64i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  auto immediateFactory = ImmediateNodeFactory();

  // it is difficult to test 64bit version, because immediate values cannot be
  // greater than 20bit
  // therefore no upper 32bit testing can be done, except for testing against
  // 0
  TEST_RI(0, convert<uint64_t>, "andi", 0b110110LL << 32, 0b100100, 0);
  TEST_RI(1, convert<uint64_t>, "andi", 0b100100LL, 0b110110LL, 0b100100LL);

  // test immediate boundary
  test12BitImmediateBounds(getFactories(), "andi", immediateFactory, memoryAccess);
}

TEST_F(IntegerInstructionTest, ANDInstruction_testValidation) {
  load({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = getMemoryAccess();
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "and", false);
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "andi", true);
}

TEST_F(IntegerInstructionTest, ORInstruction_testOr32) {
  load({"rv32i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "or", 0b110110, 0b100100, 0b110110);
  TEST_RR(1, convert<uint32_t>, "or", 0b100100, 0b110110, 0b110110);
  TEST_RR(2, convert<uint32_t>, "or", 0b101010, 0b010101, 0b111111);
  TEST_RR(3, convert<uint32_t>, "or", 0b010101, 0b101010, 0b111111);
}

TEST_F(IntegerInstructionTest, ORINstruction_testOr64) {
  load({"rv32i", "rv64i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "or", 0b110110LL << 32, 0b100100LL << 32,
          0b110110LL << 32);
  TEST_RR(1, convert<uint64_t>, "or", 0b100100LL << 32, 0b110110LL << 32,
          0b110110LL << 32);
  TEST_RR(2, convert<uint64_t>, "or", 0b101010LL << 32, 0b010101LL << 32,
          0b111111LL << 32);
  TEST_RR(3, convert<uint64_t>, "or", 0b010101LL << 32, 0b101010LL << 32,
          0b111111LL << 32);
}

TEST_F(IntegerInstructionTest, ORInstruction_testOri32) {
  load({"rv32i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  auto immediateFactory = ImmediateNodeFactory();

  TEST_RI(0, convert<uint32_t>, "ori", 0b110110, 0b100100, 0b110110);
  TEST_RI(1, convert<uint32_t>, "ori", 0b100100, 0b110110, 0b110110);
  TEST_RI(2, convert<uint32_t>, "ori", 0b101010, 0b010101, 0b111111);
  TEST_RI(3, convert<uint32_t>, "ori", 0b010101, 0b101010, 0b111111);
  // test immediate boundary
  test12BitImmediateBounds(getFactories(), "ori", immediateFactory, memoryAccess);
}
TEST_F(IntegerInstructionTest, ORInstruction_testOri64) {
  load({"rv32i", "rv64i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  auto immediateFactory = ImmediateNodeFactory();

  TEST_RI(0, convert<uint64_t>, "ori", (0b1010110LL << 32), 0b10,
          ((0b1010110LL << 32) + 2));
  // test immediate boundary
  test12BitImmediateBounds(getFactories(), "ori", immediateFactory, memoryAccess);
}

TEST_F(IntegerInstructionTest, ORInstruction_testValidation) {
  load({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = getMemoryAccess();
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "or", false);
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "ori", true);
}

TEST_F(IntegerInstructionTest, XORInstruction_testXor32) {
  load({"rv32i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "xor", 0b110110, 0b100100, 0b010010);
  TEST_RR(1, convert<uint32_t>, "xor", 0b100100, 0b110110, 0b010010);
  TEST_RR(2, convert<uint32_t>, "xor", 0b101010, 0b010101, 0b111111);
  TEST_RR(3, convert<uint32_t>, "xor", 0b010101, 0b101010, 0b111111);
}
TEST_F(IntegerInstructionTest, XORInstruction_testXor64) {
  load({"rv32i", "rv64i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "xor", 0b110110LL << 32, 0b100100LL << 32,
          0b010010LL << 32);
  TEST_RR(1, convert<uint64_t>, "xor", 0b100100LL << 32, 0b110110LL << 32,
          0b010010LL << 32);
  TEST_RR(2, convert<uint64_t>, "xor", 0b101010LL << 32, 0b010101LL << 32,
          0b111111LL << 32);
  TEST_RR(3, convert<uint64_t>, "xor", 0b010101LL << 32, 0b101010LL << 32,
          0b111111LL << 32);
}

TEST_F(IntegerInstructionTest, XORInstruction_testXori32) {
  load({"rv32i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  auto immediateFactory = ImmediateNodeFactory();

  TEST_RI(0, convert<uint32_t>, "xori", 0b110110, 0b100100, 0b010010);
  TEST_RI(1, convert<uint32_t>, "xori", 0b100100, 0b110110, 0b010010);
  TEST_RI(2, convert<uint32_t>, "xori", 0b101010, 0b010101, 0b111111);
  TEST_RI(3, convert<uint32_t>, "xori", 0b010101, 0b101010, 0b111111);
  // test immediate boundary
  test12BitImmediateBounds(getFactories(), "xori", immediateFactory, memoryAccess);
}

TEST_F(IntegerInstructionTest, XORInstruction_testXori64) {
  load({"rv32i", "rv64i"});

  MemoryAccess memoryAccess = getMemoryAccess();

  auto immediateFactory = ImmediateNodeFactory();

  TEST_RI(0, convert<uint64_t>, "xori", (0b100000001010110LL << 5),
          (0b10LL << 5), (0b100000001010100LL << 5));
  // test immediate boundary
  test12BitImmediateBounds(getFactories(), "xori", immediateFactory, memoryAccess);
}

TEST_F(IntegerInstructionTest, XORInstruction_testValidation) {
  load({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = getMemoryAccess();
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "xor", false);
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "xori", true);
}

TEST_F(IntegerInstructionTest, ShiftLeftInstruction_testSll32) {
  load({"rv32i"});

  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "sll", 0b110110, 3, 0b110110000);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1, convert<uint32_t>, "sll", 3, 0b110110, 3 << 22);
}

TEST_F(IntegerInstructionTest, ShiftLeftInstruction_testSll64) {
  load({"rv32i", "rv64i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "sll", 0b110110LL << 32, 3, 0b110110000LL
                                                                << 32);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1, convert<uint64_t>, "sll", 3, 0b110110LL << 32, 3);
}

TEST_F(IntegerInstructionTest, ShiftLeftInstruction_testSlli32) {
  load({"rv32i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  auto immediateFactory = ImmediateNodeFactory();

  TEST_RI(0, convert<uint32_t>, "slli", 0b110110, 3, 0b110110000);
  // shifts use only the lower 5bit of the second operand
  TEST_RI(1, convert<uint32_t>, "slli", 1, 0b11100001, 2);
  // test immediate boundary
  test12BitImmediateBounds(getFactories(), "slli", immediateFactory, memoryAccess);
}
TEST_F(IntegerInstructionTest, ShiftLeftInstruction_testSlli64) {
  load({"rv32i", "rv64i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  auto immediateFactory = ImmediateNodeFactory();

  TEST_RI(0, convert<uint64_t>, "slli", (0b110110LL << 32), 3,
          (0b110110000LL << 32));
  // shifts use only the lower 5bit of the second operand
  TEST_RI(1, convert<uint64_t>, "slli", (0b1LL << 32), 0b11100001, 0b10LL
                                                                       << 32);
  // test immediate boundary
  test12BitImmediateBounds(getFactories(), "slli", immediateFactory, memoryAccess);
}

TEST_F(IntegerInstructionTest, ShiftLeftInstruction_testValidation) {
  load({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = getMemoryAccess();
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "sll", false);
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "slli", true);
}

TEST_F(IntegerInstructionTest, ShiftRightInstruction_testSrl32) {
  load({"rv32i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "srl", 0b110110, 3, 0b110);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1, convert<uint32_t>, "srl", 3, 0b110110, 0);
}

TEST_F(IntegerInstructionTest, ShiftRightInstruction_testSrl64) {
  load({"rv32i", "rv64i"});

  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "srl", 0b110110LL << 32, 3, 0b110110LL << 29);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1, convert<uint64_t>, "srl", 3, 0b110110LL << 32, 3);
}

TEST_F(IntegerInstructionTest, ShiftRightInstruction_testSrli32) {
  load({"rv32i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  auto immediateFactory = ImmediateNodeFactory();

  TEST_RI(0, convert<uint32_t>, "srli", 0b110110, 3, 0b110);
  // shifts use only the lower 5bit of the second operand
  TEST_RI(1, convert<uint32_t>, "srli", 0b11, 0b11100001, 0b1);
  // test immediate boundary
  test12BitImmediateBounds(getFactories(), "srli", immediateFactory, memoryAccess);
}
TEST_F(IntegerInstructionTest, ShiftRightInstruction_testSrli64) {
  load({"rv32i", "rv64i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  auto immediateFactory = ImmediateNodeFactory();

  TEST_RI(0, convert<uint64_t>, "srli", 0b110110LL << 32, 3, 0b110110LL << 29);
  // shifts use only the lower 5bit of the second operand
  TEST_RI(1, convert<uint64_t>, "srli", 0b11LL << 32, 0b11100001, 0b11LL << 31);
  // test immediate boundary
  test12BitImmediateBounds(getFactories(), "srli", immediateFactory, memoryAccess);
}

TEST_F(IntegerInstructionTest, ShiftRightInstruction_testValidation) {
  load({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = getMemoryAccess();
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "srl", false);
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "srli", true);
}

TEST_F(IntegerInstructionTest, ShiftRightArithmeticInstruction_testSra32) {
  load({"rv32i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "sra", 0b10000000000000000000000000110110, 3,
          0b11110000000000000000000000000110);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1, convert<uint32_t>, "sra", 3, 0b10000000000000000000000000110110,
          0);
}

TEST_F(IntegerInstructionTest, ShiftRightArithmeticInstruction_testSra64) {
  load({"rv32i", "rv64i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "sra",
          0b1000000000000000000000000001101100000000000000000000000000000000LL,
          3,
          0b1111000000000000000000000000001101100000000000000000000000000000LL);
  // shifts use only the lower 5bit of the second operand
  TEST_RR(1, convert<uint64_t>, "sra", 3,
          0b1000000000000000000000000001101100000000000000000000000000000000LL,
          3);
}

TEST_F(IntegerInstructionTest, ShiftRightInstruction_testSrai32) {
  load({"rv32i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  auto immediateFactory = ImmediateNodeFactory();

  TEST_RI(0, convert<uint32_t>, "srai", 0b10000000000000000000000000110110, 3,
          0b11110000000000000000000000000110);
  // shifts use only the lower 5bit of the second operand
  TEST_RI(1, convert<uint32_t>, "srai", 0b10000000000000000000000000110110,
          0b11100001, 0b11000000000000000000000000011011);
  // test immediate boundary
  test12BitImmediateBounds(getFactories(), "srai", immediateFactory, memoryAccess);
}

TEST_F(IntegerInstructionTest, ShiftRightInstruction_testSrai64) {
  load({"rv32i", "rv64i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  auto immediateFactory = ImmediateNodeFactory();

  TEST_RI(0, convert<uint64_t>, "srai",
          0b1000000000000000000000000001101100000000000000000000000000000000LL,
          3,
          0b1111000000000000000000000000001101100000000000000000000000000000LL);
  // shifts use only the lower 5bit of the second operand
  TEST_RI(1, convert<uint64_t>, "srai", 0b11LL << 32, 0b11100001, 0b11LL << 31);
  // test immediate boundary
  test12BitImmediateBounds(getFactories(), "srai", immediateFactory, memoryAccess);
}

TEST_F(IntegerInstructionTest, ShiftRightArithmeticInstruction_testValidation) {
  load({"rv32i"});
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = getMemoryAccess();
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "sra", false);
  testIntegerInstructionValidation(memAccess, getFactories(), immediateFactory,
                                   "srai", true);
}
