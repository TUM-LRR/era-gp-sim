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

#include <utility>

#include "gtest/gtest.h"

#include "arch/common/abstract-instruction-node-factory.hpp"
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
  using Node = AbstractInstructionNodeFactory::Node;

  static raw_t assemble(const Node& instruction) {
    auto memoryValue = instruction->assemble();
    return riscv::convert<raw_t>(memoryValue);
  }
};

// clang-format off
#define ASSEMBLE_R(funct7, rs2, rs1, funct3, rd, opcode) \
  (funct7 << 25) | \
  (rs2 << 20)    | \
  (rs1 << 15)    | \
  (funct3 << 12) | \
  (rd << 7)      | \
  opcode
// clang-format on

TEST_F(FormatTests, RFormat) {
  // The R format is used by register-register operations, like ADD

  auto instruction = factories.createInstructionNode("ADD");

  auto destinationRegister = factories.createRegisterNode("x1");
  auto firstSourceRegister = factories.createRegisterNode("x2");
  auto secondSourceRegister = factories.createRegisterNode("x16");

  instruction->addChild(std::move(destinationRegister));
  instruction->addChild(std::move(firstSourceRegister));
  instruction->addChild(std::move(secondSourceRegister));

  raw_t expected = ASSEMBLE_R(0, 16u, 2u, 0, 1u, 0b0110011);

  EXPECT_EQ(assemble(instruction), expected);
}

// // testing the different formats
// TEST_F(InstructionFormatTest, RFormat) {
//   using Operands = AddInstructionNode<uint32_t>::Operands;
//   auto addInfo = instructionSet.getInstruction("add");
//   AddInstructionNode<uint32_t> addInstr(addInfo, Operands::REGISTERS);
//
//   AbstractSyntaxTreeNode::Node r1(new RegisterNode("1"));
//   AbstractSyntaxTreeNode::Node r2(new RegisterNode("2"));
//   AbstractSyntaxTreeNode::Node rd(new RegisterNode("3"));
//   addInstr.addChild(std::move(rd));
//   addInstr.addChild(std::move(r2));
//   addInstr.addChild(std::move(r1));
//
//   std::cout << addInstr.assemble() << std::endl;
// }
//
// TEST(InstructionFormats, IFormat) {
//   using Operands = AddInstructionNode<uint32_t>::Operands;
//   auto addInfo = instructionSet.getInstruction("sub");
//   AddInstructionNode<uint32_t> addInstr(addInfo, Operands::IMMEDIATES);
//
//   MemoryValue val(4 * 8);
//   val.put(22, true);
//   val.put(23, true);
//   AbstractSyntaxTreeNode::Node imm(new ImmediateNode(val));
//   AbstractSyntaxTreeNode::Node r2(new RegisterNode("2"));
//   AbstractSyntaxTreeNode::Node rd(new RegisterNode("3"));
//   addInstr.addChild(std::move(rd));
//   addInstr.addChild(std::move(r2));
//   addInstr.addChild(std::move(imm));
//
//   std::cout << addInstr.assemble() << std::endl;
// }
//
// TEST(InstructionFormats, SBFormat) {
//   using Operands = AddInstructionNode<uint32_t>::Operands;
//   auto beqInfo = instructionSet.getInstruction("beq");
//   AddInstructionNode<uint32_t> beqInstr(beqInfo, Operands::IMMEDIATES);
//
//   MemoryValue val(4 * 8);
//   val.put(22, true);
//   val.put(23, true);
//   AbstractSyntaxTreeNode::Node imm(new ImmediateNode(val));
//   AbstractSyntaxTreeNode::Node r2(new RegisterNode("2"));
//   AbstractSyntaxTreeNode::Node r1(new RegisterNode("3"));
//   beqInstr.addChild(std::move(r1));
//   beqInstr.addChild(std::move(r2));
//   beqInstr.addChild(std::move(imm));
//
//   std::cout << beqInstr.assemble() << std::endl;
// }
//
// TEST(InstructionFormats, UFormat) {
//   using Operands = AddInstructionNode<uint32_t>::Operands;
//   auto uInfo = instructionSet.getInstruction("uinst");
//   AddInstructionNode<uint32_t> uInst(uInfo, Operands::IMMEDIATES);
//
//   MemoryValue val(4 * 8);
//   val.put(22, true);
//   val.put(23, true);
//   AbstractSyntaxTreeNode::Node imm(new ImmediateNode(val));
//   AbstractSyntaxTreeNode::Node rd(new RegisterNode("3"));
//   uInst.addChild(std::move(rd));
//   uInst.addChild(std::move(imm));
//
//   std::cout << uInst.assemble() << std::endl;
// }
