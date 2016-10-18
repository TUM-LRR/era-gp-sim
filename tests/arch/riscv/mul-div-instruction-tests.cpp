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
* along with this program. If not, see <http://www.gnu.org/licenses/>.*/

#include "arithmetic-test-utils.hpp"

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"

#include "tests/arch/riscv/test-utils.hpp"

using namespace riscv;

struct MulDivInstructionTest : RiscvBaseTest {
  std::string dest{"x1"}, op1{"x2"}, op2{"x3"};
};

TEST_F(MulDivInstructionTest, Multiplication_testMUL32) {
  load({"rv32i", "rv32m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "mul", 42, 0, 0);
  TEST_RR(1, convert<uint32_t>, "mul", 0, 42, 0);

  TEST_RR(2, convert<uint32_t>, "mul", 13, 17, 221);
  TEST_RR(3, convert<uint32_t>, "mul", -13, 17, -221);
  TEST_RR(4, convert<uint32_t>, "mul", 13, -17, -221);
  TEST_RR(5, convert<uint32_t>, "mul", -13, -17, 221);

  // test 32bit bounds
  // 2^30 x 2 => 2^31
  // 2^31 x 2 => 0
  TEST_RR(6, convert<uint32_t>, "mul", 1 << 30, 2, 1 << 31);
  TEST_RR(7, convert<uint32_t>, "mul", 1 << 31, 2, 0);
}

TEST_F(MulDivInstructionTest, Multiplication_testMUL64) {
  load({"rv32i", "rv32m", "rv64i", "rv64m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "mul", 0x0000000000007e00, 0x6db6db6db6db6db7,
          0x0000000000001200);
  TEST_RR(1, convert<uint64_t>, "mul", 0x0000000000007fc0, 0x6db6db6db6db6db7,
          0x0000000000001240);

  TEST_RR(2, convert<uint64_t>, "mul", 0x00000000, 0x00000000, 0x00000000);
  TEST_RR(3, convert<uint64_t>, "mul", 0x00000001, 0x00000001, 0x00000001);
  TEST_RR(4, convert<uint64_t>, "mul", 0x00000003, 0x00000007, 0x00000015);

  TEST_RR(5, convert<uint64_t>, "mul", 0x0000000000000000, 0xffffffffffff8000,
          0x0000000000000000);
  TEST_RR(6, convert<uint64_t>, "mul", 0xffffffff80000000, 0x00000000,
          0x0000000000000000);
  TEST_RR(7, convert<uint64_t>, "mul", 0xffffffff80000000ULL,
          0xffffffffffff8000ULL, 0x400000000000ULL);

  TEST_RR(30, convert<uint64_t>, "mul", 0xaaaaaaaaaaaaaaab, 0x000000000002fe7d,
          0x000000000000ff7f);
  TEST_RR(31, convert<uint64_t>, "mul", 0x000000000002fe7d, 0xaaaaaaaaaaaaaaab,
          0x000000000000ff7f);
}

TEST_F(MulDivInstructionTest, Multiplication_testMULH32) {
  load({"rv32i", "rv32m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "mulh", 0x00000000, 0x00000000, 0x00000000);
  TEST_RR(1, convert<uint32_t>, "mulh", 0x00000001, 0x00000001, 0x00000000);
  TEST_RR(2, convert<uint32_t>, "mulh", 0x00000003, 0x00000007, 0x00000000);

  TEST_RR(3, convert<uint32_t>, "mulh", 0x00000000, 0xffff8000, 0x00000000);
  TEST_RR(4, convert<uint32_t>, "mulh", 0x00000000, 0x80000000, 0x00000000);

  TEST_RR(5, convert<uint32_t>, "mulh", 0xaaaaaaab, 0x0002fe7d, 0xffff0081);
  TEST_RR(6, convert<uint32_t>, "mulh", 0x0002fe7d, 0xaaaaaaab, 0xffff0081);

  TEST_RR(7, convert<uint32_t>, "mulh", 0xff000000, 0xff000000, 0x00010000);

  TEST_RR(8, convert<uint32_t>, "mulh", 0xffffffff, 0xffffffff, 0x00000000);
  TEST_RR(9, convert<uint32_t>, "mulh", 0xffffffff, 0x00000001, 0xffffffff);
  TEST_RR(10, convert<uint32_t>, "mulh", 0x00000001, 0xffffffff, 0xffffffff);
}

TEST_F(MulDivInstructionTest, Multiplication_testMULH64) {
  load({"rv32i", "rv32m", "rv64i", "rv64m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "mulh", 0x00000000, 0x00000000, 0x00000000);
  TEST_RR(1, convert<uint64_t>, "mulh", 0x00000001, 0x00000001, 0x00000000);
  TEST_RR(2, convert<uint64_t>, "mulh", 0x00000003, 0x00000007, 0x00000000);

  TEST_RR(3, convert<uint64_t>, "mulh", 0x0000000000000000, 0xffffffffffff8000,
          0x0000000000000000);
  TEST_RR(4, convert<uint64_t>, "mulh", 0xffffffff80000000, 0x00000000,
          0x0000000000000000);
  TEST_RR(5, convert<uint64_t>, "mulh", 0xffffffff80000000, 0xffffffffffff8000,
          0x0000000000000000);
}

// https://github.com/riscv/riscv-tests

TEST_F(MulDivInstructionTest, Multiplication_testMULHU32) {
  load({"rv32i", "rv32m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "mulhu", 0x00000000, 0x00000000, 0x00000000);
  TEST_RR(1, convert<uint32_t>, "mulhu", 0x00000001, 0x00000001, 0x00000000);
  TEST_RR(2, convert<uint32_t>, "mulhu", 0x00000003, 0x00000007, 0x00000000);

  TEST_RR(3, convert<uint32_t>, "mulhu", 0x00000000, 0xffff8000, 0x00000000);
  TEST_RR(4, convert<uint32_t>, "mulhu", 0x80000000, 0xffff8000, 0x7fffc000);

  TEST_RR(5, convert<uint32_t>, "mulhu", 0xaaaaaaab, 0x0002fe7d, 0x0001fefe);
  TEST_RR(6, convert<uint32_t>, "mulhu", 0x0002fe7d, 0xaaaaaaab, 0x0001fefe);

  TEST_RR(7, convert<uint32_t>, "mulhu", 0xff000000, 0xff000000, 0xfe010000);

  TEST_RR(8, convert<uint32_t>, "mulhu", 0xffffffff, 0xffffffff, 0xfffffffe);
  TEST_RR(9, convert<uint32_t>, "mulhu", 0xffffffff, 0x00000001, 0x00000000);
  TEST_RR(10, convert<uint32_t>, "mulhu", 0x00000001, 0xffffffff, 0x00000000);
}

TEST_F(MulDivInstructionTest, Multiplication_testMULHU64) {
  load({"rv32i", "rv32m", "rv64i", "rv64m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "mulhu", 0x00000000, 0x00000000, 0x00000000);
  TEST_RR(1, convert<uint64_t>, "mulhu", 0x00000001, 0x00000001, 0x00000000);
  TEST_RR(2, convert<uint64_t>, "mulhu", 0x00000003, 0x00000007, 0x00000000);

  TEST_RR(3, convert<uint64_t>, "mulhu", 0x0000000000000000, 0xffffffffffff8000,
          0x0000000000000000);
  TEST_RR(4, convert<uint64_t>, "mulhu", 0xffffffff80000000, 0x00000000,
          0x0000000000000000);
  TEST_RR(5, convert<uint64_t>, "mulhu", 0xffffffff80000000, 0xffffffffffff8000,
          0xffffffff7fff8000);

  TEST_RR(6, convert<uint64_t>, "mulhu", 0xaaaaaaaaaaaaaaab, 0x000000000002fe7d,
          0x000000000001fefe);
  TEST_RR(7, convert<uint64_t>, "mulhu", 0x000000000002fe7d, 0xaaaaaaaaaaaaaaab,
          0x000000000001fefe);
}

TEST_F(MulDivInstructionTest, Multiplication_testMULHSU32) {
  load({"rv32i", "rv32m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "mulhsu", 0x00000000, 0x00000000, 0x00000000);
  TEST_RR(1, convert<uint32_t>, "mulhsu", 0x00000001, 0x00000001, 0x00000000);
  TEST_RR(2, convert<uint32_t>, "mulhsu", 0x00000003, 0x00000007, 0x00000000);

  TEST_RR(3, convert<uint32_t>, "mulhsu", 0x00000000, 0xffff8000, 0x00000000);
  TEST_RR(4, convert<uint32_t>, "mulhsu", 0x80000000, 0xffff8000, 0x80004000);

  TEST_RR(5, convert<uint32_t>, "mulhsu", 0xaaaaaaab, 0x0002fe7d, 0xffff0081);
  TEST_RR(6, convert<uint32_t>, "mulhsu", 0x0002fe7d, 0xaaaaaaab, 0x0001fefe);

  TEST_RR(7, convert<uint32_t>, "mulhsu", 0xff000000, 0xff000000, 0xff010000);

  TEST_RR(8, convert<uint32_t>, "mulhsu", 0xffffffff, 0xffffffff, 0xffffffff);
  TEST_RR(9, convert<uint32_t>, "mulhsu", 0xffffffff, 0x00000001, 0xffffffff);
  TEST_RR(10, convert<uint32_t>, "mulhsu", 0x00000001, 0xffffffff, 0x00000000);
}

TEST_F(MulDivInstructionTest, Multiplication_testMULHSU64) {
  load({"rv32i", "rv32m", "rv64i", "rv64m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "mulhsu", 0x00000000, 0x00000000, 0x00000000);
  TEST_RR(1, convert<uint64_t>, "mulhsu", 0x00000001, 0x00000001, 0x00000000);
  TEST_RR(2, convert<uint64_t>, "mulhsu", 0x00000003, 0x00000007, 0x00000000);

  TEST_RR(3, convert<uint64_t>, "mulhsu", 0x0000000000000000,
          0xffffffffffff8000, 0x0000000000000000);
  TEST_RR(4, convert<uint64_t>, "mulhsu", 0xffffffff80000000, 0x00000000,
          0x0000000000000000);
  TEST_RR(5, convert<uint64_t>, "mulhsu", 0xffffffff80000000,
          0xffffffffffff8000, 0xffffffff80000000);
}

TEST_F(MulDivInstructionTest, Multiplication_testMULW) {
  load({"rv32i", "rv32m", "rv64i", "rv64m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "mulw", 0x00000000, 0x00000000, 0x00000000);
  TEST_RR(1, convert<uint64_t>, "mulw", 0x00000001, 0x00000001, 0x00000001);
  TEST_RR(2, convert<uint64_t>, "mulw", 0x00000003, 0x00000007, 0x00000015);
  TEST_RR(6, convert<uint64_t>, "mulw", 2, -1LL, -2LL);

  TEST_RR(3, convert<uint64_t>, "mulw", 0x0000000000000000, 0xffffffffffff8000,
          0x0000000000000000);
  TEST_RR(4, convert<uint64_t>, "mulw", 0xffffffff80000000, 0x00000000,
          0x0000000000000000);
  TEST_RR(5, convert<uint64_t>, "mulw", 0xffffffff80000000, 0xffffffffffff8000,
          0x0000000000000000);
}

TEST_F(MulDivInstructionTest, Multiplication_testValidation) {
  load({"rv32i", "rv32m"});
  auto instructionFactory = getFactories();
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = getMemoryAccess();
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "mul", false);
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "mulh", false);
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "mulhu", false);
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "mulhsu", false);
  load({"rv32i", "rv64i", "rv32m", "rv64m"});
  auto instructionFactory64 = getFactories();
  memAccess = getMemoryAccess();
  testIntegerInstructionValidation(memAccess, instructionFactory64,
                                   immediateFactory, "mulw", false);
}

TEST_F(MulDivInstructionTest, Division_testDIV32) {
  load({"rv32i", "rv32m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "div", 20, 6, 3);
  TEST_RR(1, convert<uint32_t>, "div", -20, 6, -3);
  TEST_RR(2, convert<uint32_t>, "div", 20, -6, -3);
  TEST_RR(3, convert<uint32_t>, "div", -20, -6, 3);
  // These test cases do not seem to make sense (for a 32bit instruction)?
  //  TEST_RR(4, convert<uint32_t>, "div", -1ULL << 63, 1, -1ULL << 63);
  //  TEST_RR(5, convert<uint32_t>, "div", -1UL << 63, -1, -1UL << 63);

  //  TEST_RR(6, convert<uint32_t>, "div", -1UL << 63, 0, -1UL);
  TEST_RR(7, convert<uint32_t>, "div", 1, 0, -1);
  TEST_RR(8, convert<uint32_t>, "div", 0, 0, -1);
}

TEST_F(MulDivInstructionTest, Division_testDIV64) {
  load({"rv32i", "rv32m", "rv64i", "rv64m"});

  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "div", 20, 6, 3);
  TEST_RR(1, convert<uint64_t>, "div", -20, 6, -3);
  TEST_RR(2, convert<uint64_t>, "div", 20, -6, -3);
  TEST_RR(3, convert<uint64_t>, "div", -20, -6, 3);

  TEST_RR(4, convert<uint64_t>, "div", UINT64_C(-1) << 63, 1, UINT64_C(-1)
                                                                  << 63);
  TEST_RR(5, convert<uint64_t>, "div", UINT64_C(-1) << 63, -1, UINT64_C(-1)
                                                                   << 63);

  TEST_RR(6, convert<uint64_t>, "div", UINT64_C(-1) << 63, 0, -1);
  TEST_RR(7, convert<uint64_t>, "div", 1, 0, -1);
  TEST_RR(8, convert<uint64_t>, "div", 0, 0, -1);
}

TEST_F(MulDivInstructionTest, Division_testDIVU32) {
  load({"rv32i", "rv32m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "divu", 20, 6, 3);
  TEST_RR(1, convert<uint32_t>, "divu", -20, 6, 715827879);
  TEST_RR(2, convert<uint32_t>, "divu", 20, -6, 0);
  TEST_RR(3, convert<uint32_t>, "divu", -20, -6, 0);

  TEST_RR(4, convert<uint32_t>, "divu", UINT32_C(-1) << 31, 1, UINT32_C(-1)
                                                                   << 31);
  TEST_RR(5, convert<uint32_t>, "divu", UINT32_C(-1) << 31, -1, 0);

  TEST_RR(6, convert<uint32_t>, "divu", UINT32_C(-1) << 31, 0, -1);
  TEST_RR(7, convert<uint32_t>, "divu", 1, 0, -1);
  TEST_RR(8, convert<uint32_t>, "divu", 0, 0, -1);
}

TEST_F(MulDivInstructionTest, Division_testDIVU64) {
  load({"rv32i", "rv32m", "rv64i", "rv64m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "divu", 20, 6, 3);
  TEST_RR(1, convert<uint64_t>, "divu", -20, 6, 3074457345618258599);
  TEST_RR(2, convert<uint64_t>, "divu", 20, -6, 0);
  TEST_RR(3, convert<uint64_t>, "divu", -20, -6, 0);

  TEST_RR(4, convert<uint64_t>, "divu", UINT64_C(-1) << 63, 1LL, UINT64_C(-1)
                                                                     << 63);
  TEST_RR(5, convert<uint64_t>, "divu", UINT64_C(-1) << 63, -1LL, 0);

  TEST_RR(6, convert<uint64_t>, "divu", UINT64_C(-1) << 63, 0, -1LL);
  TEST_RR(7, convert<uint64_t>, "divu", 1, 0, -1LL);
  TEST_RR(8, convert<uint64_t>, "divu", 0, 0, -1LL);
}

TEST_F(MulDivInstructionTest, Division_testDIVW) {
  load({"rv32i", "rv32m", "rv64i", "rv64m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "divw", 20, 6, 3);
  TEST_RR(1, convert<uint64_t>, "divw", -20, 6, -3);
  TEST_RR(2, convert<uint64_t>, "divw", 20, -6, -3);
  TEST_RR(3, convert<uint64_t>, "divw", -20, -6, 3);

  TEST_RR(4, convert<uint64_t>, "divw", UINT64_C(-1) << 31, 1, UINT64_C(-1)
                                                                   << 31);
  TEST_RR(5, convert<uint64_t>, "divw", UINT64_C(-1) << 31, -1, UINT64_C(-1)
                                                                    << 31);

  TEST_RR(6, convert<uint64_t>, "divw", UINT64_C(-1) << 31, 0, -1LL);
  TEST_RR(7, convert<uint64_t>, "divw", 1, 0, -1);
  TEST_RR(8, convert<uint64_t>, "divw", 0, 0, -1);
}

TEST_F(MulDivInstructionTest, Division_testDIVUW) {
  load({"rv32i", "rv32m", "rv64i", "rv64m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "divuw", 20, 6, 3);
  TEST_RR(1, convert<uint64_t>, "divuw", 0xFFFFFFEC, 6, 715827879);
  TEST_RR(2, convert<uint64_t>, "divuw", 20, -6, 0);
  TEST_RR(3, convert<uint64_t>, "divuw", -20, -6, 0);

  TEST_RR(4, convert<uint64_t>, "divuw", UINT64_C(-1) << 31, 1, UINT64_C(-1)
                                                                    << 31);
  TEST_RR(5, convert<uint64_t>, "divuw", UINT64_C(-1) << 31, -1, 0);

  TEST_RR(6, convert<uint64_t>, "divuw", UINT64_C(-1) << 31, 0, -1);
  TEST_RR(7, convert<uint64_t>, "divuw", 1, 0, -1);
  TEST_RR(8, convert<uint64_t>, "divuw", 0, 0, -1);
}

TEST_F(MulDivInstructionTest, Division_testValidation) {
  load({"rv32i", "rv32m"});
  auto instructionFactory = getFactories();
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = getMemoryAccess();
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "div", false);
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "divu", false);

  load({"rv32i", "rv64i", "rv32m", "rv64m"});
  auto instructionFactory64 = getFactories();
  memAccess = getMemoryAccess();
  testIntegerInstructionValidation(memAccess, instructionFactory64,
                                   immediateFactory, "divw", false);
  testIntegerInstructionValidation(memAccess, instructionFactory64,
                                   immediateFactory, "divuw", false);
}

TEST_F(MulDivInstructionTest, Remainder_testREM32) {
  load({"rv32i", "rv32m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "rem", 20, 6, 2);
  TEST_RR(1, convert<uint32_t>, "rem", -20, 6, -2);
  TEST_RR(2, convert<uint32_t>, "rem", 20, -6, 2);
  TEST_RR(3, convert<uint32_t>, "rem", -20, -6, -2);

  // These test do not seem to make sense for a 32bit instruction?
  //  TEST_RR(4, convert<uint32_t>, "rem", -1 << 63, 1, 0);
  //  TEST_RR(5, convert<uint32_t>, "rem", -1 << 63, -1, 0);

  //  TEST_RR(6, convert<uint32_t>, "rem", -1 << 63, 0, -1 << 63);
  TEST_RR(7, convert<uint32_t>, "rem", 1, 0, 1);
  TEST_RR(8, convert<uint32_t>, "rem", 0, 0, 0);
}

TEST_F(MulDivInstructionTest, Remainder_testREM64) {
  load({"rv32i", "rv32m", "rv64i", "rv64m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "rem", 20, 6, 2);
  TEST_RR(1, convert<uint64_t>, "rem", -20, 6, -2);
  TEST_RR(2, convert<uint64_t>, "rem", 20, -6, 2);
  TEST_RR(3, convert<uint64_t>, "rem", -20, -6, -2);

  TEST_RR(4, convert<uint64_t>, "rem", UINT64_C(-1) << 63, 1, 0);
  TEST_RR(5, convert<uint64_t>, "rem", UINT64_C(-1) << 63, -1, 0);

  TEST_RR(6, convert<uint64_t>, "rem", UINT64_C(-1) << 63, 0, UINT64_C(-1)
                                                                  << 63);
  TEST_RR(7, convert<uint64_t>, "rem", 1, 0, 1);
  TEST_RR(8, convert<uint64_t>, "rem", 0, 0, 0);
}

TEST_F(MulDivInstructionTest, Remainder_testREMU32) {
  load({"rv32i", "rv32m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint32_t>, "remu", 20, 6, 2);
  TEST_RR(1, convert<uint32_t>, "remu", -20, 6, 2);
  TEST_RR(2, convert<uint32_t>, "remu", 20, -6, 20);
  TEST_RR(3, convert<uint32_t>, "remu", -20, -6, -20);
  // These tests do not seem to make sense for a 32bit instruction?
  //  TEST_RR(4, convert<uint32_t>, "remu", -1 << 63, 1, 0);
  //  TEST_RR(5, convert<uint32_t>, "remu", -1 << 63, -1, -1 << 63);

  //  TEST_RR(6, convert<uint32_t>, "remu", -1 << 63, 0, -1 << 63);
  TEST_RR(7, convert<uint32_t>, "remu", 1, 0, 1);
  TEST_RR(8, convert<uint32_t>, "remu", 0, 0, 0);
}

TEST_F(MulDivInstructionTest, Remainder_testREMU64) {
  load({"rv32i", "rv32m", "rv64i", "rv64m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "remu", 20, 6, 2);
  TEST_RR(1, convert<uint64_t>, "remu", -20, 6, 2);
  TEST_RR(2, convert<uint64_t>, "remu", 20, -6, 20);
  TEST_RR(3, convert<uint64_t>, "remu", -20, -6, -20);

  TEST_RR(4, convert<uint64_t>, "remu", UINT64_C(-1) << 63, 1, 0);
  TEST_RR(5, convert<uint64_t>, "remu", UINT64_C(-1) << 63, -1LL, UINT64_C(-1)
                                                                      << 63);

  TEST_RR(6, convert<uint64_t>, "remu", UINT64_C(-1) << 63, 0, UINT64_C(-1)
                                                                   << 63);
  TEST_RR(7, convert<uint64_t>, "remu", 1, 0, 1);
  TEST_RR(8, convert<uint64_t>, "remu", 0, 0, 0);
}

TEST_F(MulDivInstructionTest, Remainder_testREMW) {
  load({"rv32i", "rv32m", "rv64i", "rv64m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "remw", 20, 6, 2);
  TEST_RR(1, convert<uint64_t>, "remw", -20, 6, -2);
  TEST_RR(2, convert<uint64_t>, "remw", 20, -6, 2);
  TEST_RR(3, convert<uint64_t>, "remw", -20, -6, -2);

  TEST_RR(4, convert<uint64_t>, "remw", UINT64_C(-1) << 31, 1, 0);
  TEST_RR(5, convert<uint64_t>, "remw", UINT64_C(-1) << 31, -1, 0);

  TEST_RR(6, convert<uint64_t>, "remw", UINT64_C(-1) << 31, 0, UINT64_C(-1)
                                                                   << 31);
  TEST_RR(7, convert<uint64_t>, "remw", 1, 0, 1);
  TEST_RR(8, convert<uint64_t>, "remw", 0, 0, 0);
  TEST_RR(9, convert<uint64_t>, "remw", 0xfffffffffffff897LL, 0,
          0xfffffffffffff897LL);
}

TEST_F(MulDivInstructionTest, Remainder_testREMUW) {
  load({"rv32i", "rv32m", "rv64i", "rv64m"});
  MemoryAccess memoryAccess = getMemoryAccess();

  TEST_RR(0, convert<uint64_t>, "remuw", 20, 6, 2);
  TEST_RR(1, convert<uint64_t>, "remuw", -20, 6, 2);
  TEST_RR(2, convert<uint64_t>, "remuw", 20, -6, 20);
  TEST_RR(3, convert<uint64_t>, "remuw", -20, -6, -20);

  TEST_RR(4, convert<uint64_t>, "remuw", UINT64_C(-1) << 31, 1, 0);
  TEST_RR(5, convert<uint64_t>, "remuw", UINT64_C(-1) << 31, -1, UINT64_C(-1)
                                                                     << 31);

  TEST_RR(6, convert<uint64_t>, "remuw", UINT64_C(-1) << 31, 0, UINT64_C(-1)
                                                                    << 31);
  TEST_RR(7, convert<uint64_t>, "remuw", 1, 0, 1);
  TEST_RR(8, convert<uint64_t>, "remuw", 0, 0, 0);
}

TEST_F(MulDivInstructionTest, Remainder_testValidation) {
  load({"rv32i", "rv32m"});
  auto instructionFactory = getFactories();
  auto immediateFactory = ImmediateNodeFactory();
  auto memAccess = getMemoryAccess();
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "rem", false);
  testIntegerInstructionValidation(memAccess, instructionFactory,
                                   immediateFactory, "remu", false);

  load({"rv32i", "rv64i", "rv32m", "rv64m"});
  auto instructionFactory64 = getFactories();
  memAccess = getMemoryAccess();
  testIntegerInstructionValidation(memAccess, instructionFactory64,
                                   immediateFactory, "remw", false);
  testIntegerInstructionValidation(memAccess, instructionFactory64,
                                   immediateFactory, "remuw", false);
}
