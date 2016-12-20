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

#include <cstdint>
#include <string>
#include <utility>

#include "gtest/gtest.h"

#include "arch/common/abstract-instruction-node-factory.hpp"
#include "arch/common/abstract-register-node-factory.hpp"
#include "arch/common/abstract-syntax-tree-node.hpp"
#include "arch/riscv/properties.hpp"
#include "arch/riscv/utility.hpp"
#include "tests/arch/riscv/base-fixture.hpp"

using namespace riscv;

// Test plan:
// Test all formats.
// For each format, create an instruction.
// Assemble it and compare it to hand-assembled values.
// Then change the arguments slightly and ensure changes propagate.

struct FormatTests : public riscv::BaseFixture {
  using raw_t = riscv::unsigned32_t;
  using immediate_t = std::uint32_t;
  using InstructionNode = AbstractInstructionNodeFactory::Node;
  using RegisterNode = AbstractRegisterNodeFactory::Node;
  using AbstractNode = AbstractSyntaxTreeNode::Node;

  auto createRegisterNode(const std::string& identifier) const {
    return factories.createRegisterNode(identifier);
  }

  auto createImmediateNode(immediate_t immediate) const {
    auto memoryValue = riscv::convert(immediate);
    return factories.createImmediateNode(memoryValue);
  }

  auto createInstructionNode(const std::string& identifier) const {
    return factories.createInstructionNode(identifier);
  }

  void addChild(InstructionNode& instruction, AbstractNode& child) const {
    instruction->addChild(std::move(child));
  }


  static raw_t assemble(const InstructionNode& instruction) {
    auto memoryValue = instruction->assemble();
    return riscv::convert<raw_t>(memoryValue);
  }
};

// clang-format off
#define ASSEMBLE_R(funct7, rs2, rs1, funct3, rd, opcode) \
  ((funct7) << 25) | \
  ((rs2) << 20)    | \
  ((rs1) << 15)    | \
  ((funct3) << 12) | \
  ((rd) << 7)      | \
  (opcode)

#define ASSEMBLE_I(immediate, rs1, funct3, rd, opcode) \
  ((immediate) << 20) | \
  ((rs1) << 15)       | \
  ((funct3) << 12)    | \
  ((rd) << 7)         | \
  (opcode)

#define ASSEMBLE_S(offset, source, base, funct3, opcode) \
  (((offset & (127 << 5)) >> 5) << 25) | \
  ((source) << 20)                       | \
  ((base) << 15)                         | \
  ((funct3) << 12)                       | \
  ((offset & 31) << 7)                 | \
  (opcode)

#define ASSEMBLE_SB(offset, source, base, funct3, opcode) \
  (((offset & 2048) >> 11) << 31)     | \
  (((offset & (63 << 4)) >> 4) << 25) | \
  ((source) << 20)                      | \
  ((base) << 15)                        | \
  ((funct3) << 12)                      | \
  ((offset & 15) << 8)                | \
  (((offset & 1024) >> 10) << 7)      | \
  (opcode)

#define ASSEMBLE_U(immediate, rd, opcode) \
  ((immediate) << 12) | \
  ((rd) << 7)         | \
  (opcode)

#define ASSEMBLE_UJ(immediate, rd, opcode) \
  ((((immediate) & (1 << 19)) >> 19) << 31)  | \
  (((immediate) & 1023) << 21)                 | \
  ((((immediate) & 1024) >> 10) << 20)       | \
  ((((immediate) & (255 << 11)) >> 11) << 12)  | \
  ((rd) << 7)                                | \
  (opcode)
// clang-format on

TEST_F(FormatTests, RFormat) {
  // The `R` format is used by register-register operations, like ADD
  // Its layout is: `funct7 | rs2 | rs1 | funct3 | rd | opcode`

  auto instruction = createInstructionNode("ADD");

  auto destinationRegister = createRegisterNode("x1");
  auto firstSourceRegister = createRegisterNode("x2");
  auto secondSourceRegister = createRegisterNode("x16");

  addChild(instruction, destinationRegister);
  addChild(instruction, firstSourceRegister);
  addChild(instruction, secondSourceRegister);

  raw_t expected = ASSEMBLE_R(0, 16u, 2u, 0, 1u, 0b0110011);

  auto assembly = assemble(instruction);
  EXPECT_EQ(assembly, expected);
  EXPECT_NE(assembly, expected + 1);

  // Now change stuff around and expect a different assembly
  instruction->setChild(2, createRegisterNode("x22"));

  assembly = assemble(instruction);
  EXPECT_NE(assembly, expected);

  expected = ASSEMBLE_R(0, 22u, 2u, 0, 1u, 0b0110011);
  EXPECT_EQ(assembly, expected);

  instruction->setChild(0, createRegisterNode("x6"));

  assembly = assemble(instruction);
  EXPECT_NE(assembly, expected);

  expected = ASSEMBLE_R(0, 22u, 2u, 0, 6u, 0b0110011);
  EXPECT_EQ(assembly, expected);
}

TEST_F(FormatTests, IFormat) {
  // The `I` format is used by register-immediate operations, like XORI
  // Its layout is: `imm[11:0] | rs1 | funct3 | rd | opcode`

  auto instruction = createInstructionNode("XORI");

  auto destinationRegister = createRegisterNode("x1");
  auto sourceRegister = createRegisterNode("x31");
  auto immediate = createImmediateNode(1234);

  addChild(instruction, destinationRegister);
  addChild(instruction, sourceRegister);
  addChild(instruction, immediate);

  raw_t expected = ASSEMBLE_I(1234u, 31u, 0b100, 1u, 0b0010011);

  auto assembly = assemble(instruction);
  EXPECT_EQ(assembly, expected);
  EXPECT_NE(assembly, expected + 1);

  // Now change stuff around and expect a different assembly
  instruction->setChild(1, createRegisterNode("x5"));

  assembly = assemble(instruction);
  EXPECT_NE(assembly, expected);

  expected = ASSEMBLE_I(1234u, 5u, 0b100, 1u, 0b0010011);
  EXPECT_EQ(assembly, expected);

  // 2047 is the last value that should fit into a 12 bit signed immediate
  instruction->setChild(2, createImmediateNode(2047));

  assembly = assemble(instruction);
  EXPECT_NE(assembly, expected);

  expected = ASSEMBLE_I(2047u, 5u, 0b100, 1u, 0b0010011);
  EXPECT_EQ(assembly, expected);
}

TEST_F(FormatTests, SFormat) {
  // The `S` format is for store operations.
  // Its layout is:
  // `imm[11] | imm[10:5] | rs2 | rs1 | funct3 | imm[4:1] | imm[0] | opcode`

  auto instruction = createInstructionNode("SH");

  auto baseRegister = createRegisterNode("x1");
  auto sourceRegister = createRegisterNode("x14");
  auto offset = createImmediateNode(1234);

  addChild(instruction, baseRegister);
  addChild(instruction, sourceRegister);
  addChild(instruction, offset);

  raw_t expected = ASSEMBLE_S(1234u, 14u, 1u, 0b001, 0b0100011);

  auto assembly = assemble(instruction);
  EXPECT_EQ(assembly, expected);
  EXPECT_NE(assembly, expected + 1);

  // Now change stuff around and expect a different assembly
  instruction->setChild(1, createRegisterNode("x5"));

  assembly = assemble(instruction);
  EXPECT_NE(assembly, expected);

  expected = ASSEMBLE_S(1234u, 5u, 1u, 0b001, 0b0100011);
  EXPECT_EQ(assembly, expected);

  instruction->setChild(2, createImmediateNode(0));

  assembly = assemble(instruction);
  EXPECT_NE(assembly, expected);

  expected = ASSEMBLE_S(0, 5u, 1u, 0b001, 0b0100011);
  EXPECT_EQ(assembly, expected);
}

TEST_F(FormatTests, SBFormat) {
  // The `SB` format is for branch operations.
  // Its layout is:
  // `imm[12] | imm[10:5] | rs2 | rs1 | funct3 | imm[4:1] | imm[11] | opcode`

  auto instruction = createInstructionNode("BGE");

  auto firstRegister = createRegisterNode("x1");
  auto secondRegister = createRegisterNode("x14");
  auto offset = createImmediateNode(1234);

  addChild(instruction, firstRegister);
  addChild(instruction, secondRegister);
  addChild(instruction, offset);

  raw_t expected = ASSEMBLE_SB(1234u, 14u, 1u, 0b101, 0b1100011);

  auto assembly = assemble(instruction);
  EXPECT_EQ(assembly, expected);
  EXPECT_NE(assembly, expected + 1);

  // Now change stuff around and expect a different assembly
  instruction->setChild(1, createRegisterNode("x5"));

  assembly = assemble(instruction);
  EXPECT_NE(assembly, expected);

  expected = ASSEMBLE_SB(1234u, 5u, 1u, 0b101, 0b1100011);
  EXPECT_EQ(assembly, expected);

  instruction->setChild(2, createImmediateNode(0));

  assembly = assemble(instruction);
  EXPECT_NE(assembly, expected);

  expected = ASSEMBLE_SB(0, 5u, 1u, 0b101, 0b1100011);
  EXPECT_EQ(assembly, expected);
}

TEST_F(FormatTests, UFormat) {
  // The `U` format is for LUI and AUIPC instructions (in the base ISA).
  // The layout is:
  // `imm[31] | imm[30:20] | imm[19:15] | imm[14:12] | rd | opcode`

  auto instruction = createInstructionNode("LUI");

  auto destinationRegister = createRegisterNode("x7");
  auto immediate = createImmediateNode(123456);

  addChild(instruction, destinationRegister);
  addChild(instruction, immediate);

  raw_t expected = ASSEMBLE_U(123456u, 7u, 0b0110111);

  auto assembly = assemble(instruction);
  EXPECT_EQ(assembly, expected);
  EXPECT_NE(assembly, expected + 1);

  // Now change stuff around and expect a different assembly
  instruction->setChild(0, createRegisterNode("x5"));

  assembly = assemble(instruction);
  EXPECT_NE(assembly, expected);

  expected = ASSEMBLE_U(123456u, 5u, 0b0110111);
  EXPECT_EQ(assembly, expected);

  instruction->setChild(1, createImmediateNode(0));

  assembly = assemble(instruction);
  EXPECT_NE(assembly, expected);

  expected = ASSEMBLE_U(0, 5u, 0b0110111);

  EXPECT_EQ(assembly, expected);
}

TEST_F(FormatTests, UJFormat) {
  // The `UJ` format is for the JAL instruction (in the base ISA).
  // The layout is:
  // `imm[20] | imm[10:5] | imm[4:1] | imm[11] | imm[19:15] | imm[14:12] | rd |
  // opcode`

  auto instruction = createInstructionNode("JAL");

  auto destinationRegister = createRegisterNode("x20");
  auto immediate = createImmediateNode(123456);

  addChild(instruction, destinationRegister);
  addChild(instruction, immediate);

  raw_t expected = ASSEMBLE_UJ(123456u, 20u, 0b1101111);

  auto assembly = assemble(instruction);
  EXPECT_EQ(assembly, expected);
  EXPECT_NE(assembly, expected + 1);

  // Now change stuff around and expect a different assembly
  instruction->setChild(0, createRegisterNode("x5"));

  assembly = assemble(instruction);
  EXPECT_NE(assembly, expected);

  expected = ASSEMBLE_UJ(123456u, 5u, 0b1101111);
  EXPECT_EQ(assembly, expected);

  instruction->setChild(1, createImmediateNode(0));

  assembly = assemble(instruction);
  EXPECT_NE(assembly, expected);

  expected = ASSEMBLE_UJ(0, 5u, 0b1101111);

  EXPECT_EQ(assembly, expected);
}
