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

#include <gtest/gtest.h>
#include "tests/arch/riscv/test-utils.hpp"

struct AssembleTest32 : RiscvBaseTest {
  using Node = std::unique_ptr<AbstractSyntaxTreeNode>;

  AssembleTest32() {
    load({"rv32i"});
    _factories = getFactories();
  }

  void fillRegisterRegister(Node& cmd, const std::string d0,
                            const std::string r0, const std::string r1) const {
    cmd->addChild(createRegister(d0));
    cmd->addChild(createRegister(r0));
    cmd->addChild(createRegister(r1));
  }

  void fillRegisterImmediate(Node& cmd, const std::string d0,
                             const std::string r0,
                             riscv::signed32_t imm) const {
    cmd->addChild(createRegister(d0));
    cmd->addChild(createRegister(r0));
    cmd->addChild(createImmediate(imm));
  }

  Node create(const std::string mnemonic) const {
    return _factories.createInstructionNode(mnemonic);
  }

  Node createImmediate(riscv::signed32_t imm) const {
    return _factories.createImmediateNode(riscv::convert(imm));
  }

  Node createRegister(const std::string r) const {
    return _factories.createRegisterNode(r);
  }

 private:
  NodeFactoryCollection _factories;
};

TEST_F(AssembleTest32, formatR) {
  MemoryAccess _memAccess = getMemoryAccess();
  auto cmdR = create("add");
  fillRegisterRegister(cmdR, "x0", "x0", "x0");
  ASSERT_TRUE(cmdR->validate(_memAccess).isSuccess());
  cmdR->assemble();
}

TEST_F(AssembleTest32, formatI) {
  MemoryAccess _memAccess = getMemoryAccess();
  auto cmdI = create("addi");
  fillRegisterImmediate(cmdI, "x0", "x0", 1);
  ASSERT_TRUE(cmdI->validate(_memAccess).isSuccess());
  cmdI->assemble();
}

TEST_F(AssembleTest32, formatU) {
  MemoryAccess _memAccess = getMemoryAccess();
  auto cmdU = create("lui");
  cmdU->addChild(createRegister("x0"));
  cmdU->addChild(createImmediate(6543));
  ASSERT_TRUE(cmdU->validate(_memAccess).isSuccess());
  cmdU->assemble();
}

TEST_F(AssembleTest32, formatUJ) {
  MemoryAccess _memAccess = getMemoryAccess();
  auto cmdUJ = create("jal");
  cmdUJ->addChild(createRegister("x0"));
  cmdUJ->addChild(createImmediate(234));
  ASSERT_TRUE(cmdUJ->validate(_memAccess).isSuccess());
  cmdUJ->assemble();
}

TEST_F(AssembleTest32, formatS) {
  MemoryAccess _memAccess = getMemoryAccess();
  auto cmdS = create("sw");
  fillRegisterImmediate(cmdS, "x0", "x0", 0);
  ASSERT_TRUE(cmdS->validate(_memAccess).isSuccess());
  cmdS->assemble();
}

TEST_F(AssembleTest32, formatSB) {
  MemoryAccess _memAccess = getMemoryAccess();
  auto cmdSB = create("beq");
  fillRegisterImmediate(cmdSB, "x0", "x0", 123);
  ASSERT_TRUE(cmdSB->validate(_memAccess).isSuccess());
  cmdSB->assemble();
}
