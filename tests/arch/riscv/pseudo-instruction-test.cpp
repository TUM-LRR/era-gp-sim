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
#include <iostream>
#include <memory>
#include <string>

#include "gtest/gtest.h"

#include "arch/riscv/properties.hpp"
#include "arch/riscv/utility.hpp"
#include "parser/riscv/riscv-parser.hpp"
#include "common/translateable.hpp"
#include "ui/translateable-processing.hpp"
#include "tests/arch/riscv/base-fixture.hpp"

using namespace riscv;

class PseudoInstructionTest : public riscv::BaseFixture {
 public:
  /**
   * Executes instructions in a very easy way.
   * Commands are executed in the order, given by the representation.
   * Loops/Jumps etc. won't work, this is just for testing purposes!
   *
   * \param representation The instructions generated by the parser
   */
  void
  execute(FinalRepresentation& representation, MemoryAccess& memoryAccess) {
    for (auto& command : representation.commandList()) {
      auto newProgramCounter = command.node()->getValue(memoryAccess);
      memoryAccess.setRegisterValue("pc", newProgramCounter);
    }
  }

  /**
   * Tests a pseudo instruction.
   *
   * This function creates a new parser, appends the given instruction to
   * the builtin macros, parses that string, and executes it.
   */
  void testPseudoInstruction(MemoryAccess& memoryAccess,
                             const std::string& instruction,
                             std::size_t expectedInstructionAmount) {
    // Create parser, and parse the instruction
    auto architecture = getArchitecture();

    RiscvParser parser{architecture, memoryAccess};

    // We crossed this out, already added by the parser:
    // architecture.getBuiltinMacros() + "\n" + 
    auto rep =
        parser.parse(instruction);

    // Assert that no compile error occurred
    for (auto& compileError : rep.errorList().errors()) {
      ASSERT_TRUE(false) << compileError.message().getBaseString();
    }

    // Assert that the amount of parsed instructions is equal to the expected
    // amount of instructions
    ASSERT_EQ(rep.commandList().size(), expectedInstructionAmount);

    // Execute the instructions
    execute(rep, memoryAccess);
  }
};

TEST_F(PseudoInstructionTest, LA_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(0));

  testPseudoInstruction(memoryAccess, "la x6,0x12345678", 2);

  ASSERT_EQ(memoryAccess.getRegisterValue("x6").get(),
            convert<std::uint32_t>(0x12345678));
}

TEST_F(PseudoInstructionTest, NOP_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();
  testPseudoInstruction(memoryAccess, "nop", 1);

  // Well, it's hard to test if *nothing* happend :/
}

TEST_F(PseudoInstructionTest, LI_32) {
  loadArchitecture({"rv32i"});
  auto& memoryAccess = getMemoryAccess();

  // TODO Use 0xFFF.. instead of 0x111.. for this test.
  // Currently, the first one does not work due to issues with the parser.


  testPseudoInstruction(memoryAccess, "li x1,0x11111111", 2);

  ASSERT_EQ(memoryAccess.getRegisterValue("x1").get(),
            convert<std::uint32_t>(0x11111111));
}

TEST_F(PseudoInstructionTest, MV_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();
  auto testValue = 0x12345678;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));

  testPseudoInstruction(memoryAccess, "mv x2,x1", 1);

  ASSERT_EQ(memoryAccess.getRegisterValue("x2").get(),
            convert<std::uint32_t>(testValue));
}

TEST_F(PseudoInstructionTest, NOT_32) {
  loadArchitecture({"rv32i"});
  auto& memoryAccess = getMemoryAccess();

  auto testValue = 0x12345678;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));

  testPseudoInstruction(memoryAccess, "not x2,x1", 1);

  ASSERT_EQ(memoryAccess.getRegisterValue("x2").get(),
            convert<std::uint32_t>(~testValue));
}

TEST_F(PseudoInstructionTest, NEG_32) {
  loadArchitecture({"rv32i"});
  auto& memoryAccess = getMemoryAccess();

  auto testValue = 0x12345678;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));

  testPseudoInstruction(memoryAccess, "neg x2,x1", 1);

  ASSERT_EQ(memoryAccess.getRegisterValue("x2").get(),
            convert<std::uint32_t>(-testValue));
}

TEST_F(PseudoInstructionTest, NEGW_64) {
  loadArchitecture({"rv32i", "rv64i"});
  auto& memoryAccess = getMemoryAccess();

  auto testValue = 0x1234567812345678;
  memoryAccess.setRegisterValue("x1", convert<uint64_t>(testValue));

  testPseudoInstruction(memoryAccess, "negw x2,x1", 1);

  ASSERT_EQ(memoryAccess.getRegisterValue("x2").get(),
            convert<uint64_t>(-(testValue & 0xFFFFFFFF)));
}

TEST_F(PseudoInstructionTest, SEXTW_64) {
  loadArchitecture({"rv32i", "rv64i"});

  auto& memoryAccess = getMemoryAccess();
  auto testValue = 0xFFFFFFFF80000001;
  memoryAccess.setRegisterValue("x1",
                                convert<uint64_t>(testValue & 0xFFFFFFFF));

  testPseudoInstruction(memoryAccess, "sext.w x2,x1", 1);

  ASSERT_EQ(memoryAccess.getRegisterValue("x2").get(),
            convert<uint64_t>(testValue));
}

TEST_F(PseudoInstructionTest, SEQZ_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();

  auto testValue = 1;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  testPseudoInstruction(memoryAccess, "seqz x2,x1", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("x2").get(),
            convert<std::uint32_t>(0));

  testValue = 0;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  testPseudoInstruction(memoryAccess, "seqz x2,x1", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("x2").get(),
            convert<std::uint32_t>(1));
}

TEST_F(PseudoInstructionTest, SNEZ_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();

  auto testValue = 1;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  testPseudoInstruction(memoryAccess, "snez x2,x1", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("x2").get(),
            convert<std::uint32_t>(1));

  testValue = 0;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  testPseudoInstruction(memoryAccess, "snez x2,x1", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("x2").get(),
            convert<std::uint32_t>(0));
}

TEST_F(PseudoInstructionTest, SLTZ_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();

  auto testValue = 1;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  testPseudoInstruction(memoryAccess, "sltz x2,x1", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("x2").get(),
            convert<std::uint32_t>(0));

  testValue = -1;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  testPseudoInstruction(memoryAccess, "sltz x2,x1", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("x2").get(),
            convert<std::uint32_t>(1));
}

TEST_F(PseudoInstructionTest, SGTZ_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();

  auto testValue = 1;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  testPseudoInstruction(memoryAccess, "sgtz x2,x1", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("x2").get(),
            convert<std::uint32_t>(1));

  testValue = -1;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  testPseudoInstruction(memoryAccess, "sgtz x2,x1", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("x2").get(),
            convert<std::uint32_t>(0));
}

TEST_F(PseudoInstructionTest, BEQZ_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();

  auto testValue = 1;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));
  testPseudoInstruction(memoryAccess, "beqz x1,22", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(4));

  testValue = 0;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));
  testPseudoInstruction(memoryAccess, "beqz x1,22", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(44));
}

TEST_F(PseudoInstructionTest, BNEZ_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();

  auto testValue = 1;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));
  testPseudoInstruction(memoryAccess, "bnez x1,22", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(44));

  testValue = 0;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));
  testPseudoInstruction(memoryAccess, "bnez x1,22", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(4));
}

TEST_F(PseudoInstructionTest, BLEZ_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();

  auto testValue = 1;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));
  testPseudoInstruction(memoryAccess, "blez x1,22", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(4));

  testValue = 0;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));
  testPseudoInstruction(memoryAccess, "blez x1,22", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(44));
}

TEST_F(PseudoInstructionTest, BGEZ_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();

  auto testValue = 0;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));
  testPseudoInstruction(memoryAccess, "bgez x1,22", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(44));

  testValue = -1;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));
  testPseudoInstruction(memoryAccess, "bgez x1,22", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(4));
}

TEST_F(PseudoInstructionTest, BLTZ_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();

  auto testValue = 1;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));
  testPseudoInstruction(memoryAccess, "bltz x1,22", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(4));

  testValue = -1;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));
  testPseudoInstruction(memoryAccess, "bltz x1,22", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(44));
}

TEST_F(PseudoInstructionTest, BGTZ_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();

  auto testValue = 1;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));
  testPseudoInstruction(memoryAccess, "bgtz x1,22", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(44));

  testValue = -1;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));
  testPseudoInstruction(memoryAccess, "bgtz x1,22", 1);
  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(4));
}

TEST_F(PseudoInstructionTest, J_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(8));

  testPseudoInstruction(memoryAccess, "j 18", 1);

  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(44));
}

TEST_F(PseudoInstructionTest, JAL_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(8));

  testPseudoInstruction(memoryAccess, "jal 18", 1);

  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(44));
  ASSERT_EQ(memoryAccess.getRegisterValue("x1").get(),
            convert<std::uint32_t>(12));
}

TEST_F(PseudoInstructionTest, JR_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();
  auto testValue = 42;
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));

  testPseudoInstruction(memoryAccess, "jr x1", 1);

  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(testValue));
}

TEST_F(PseudoInstructionTest, JALR_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();
  auto testValue = 42;
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));
  memoryAccess.setRegisterValue("x1", convert<std::uint32_t>(testValue));

  testPseudoInstruction(memoryAccess, "jalr x1", 1);

  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(testValue));
  ASSERT_EQ(memoryAccess.getRegisterValue("x1").get(),
            convert<std::uint32_t>(4));
}

TEST_F(PseudoInstructionTest, CALL_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));

  testPseudoInstruction(memoryAccess, "call 0x12345678", 2);

  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(0x12345678));
  // Return address must be 8, because call is translated to two instructions
  ASSERT_EQ(memoryAccess.getRegisterValue("x1").get(),
            convert<std::uint32_t>(8));
}

TEST_F(PseudoInstructionTest, TAIL_32) {
  loadArchitecture({"rv32i"});

  auto& memoryAccess = getMemoryAccess();
  memoryAccess.setRegisterValue("pc", convert<std::uint32_t>(0));

  testPseudoInstruction(memoryAccess, "tail 0x12345678", 2);

  ASSERT_EQ(memoryAccess.getRegisterValue("pc").get(),
            convert<std::uint32_t>(0x12345678));
}
