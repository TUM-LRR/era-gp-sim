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

#include "gtest/gtest.h"

#include "tests/arch/riscv/base-fixture.hpp"

using namespace riscv;

// Test plan:
// Test all formats.
// For each format, create an instruction.
// Assemble it and compare it to hand-assembled values.
// Then change the arguments slightly and ensure changes propagate.

struct InstructionFormatsFixture : public riscv::BaseFixture {
  InstructionFormatsFixture();
};

// R-Format, for register-register instructions (like `add`)
// TEST(InstructionFormats, RFormat) {
//   auto
// }

// TEST(InstructionFormats, RFormat) {
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
