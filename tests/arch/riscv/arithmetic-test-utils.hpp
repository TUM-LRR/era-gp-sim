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

#ifndef ERAGPSIM_TESTS_ARCH_RISCV_ARITHMETIC_TEST_UTILS_HPP_
#define ERAGPSIM_TESTS_ARCH_RISCV_ARITHMETIC_TEST_UTILS_HPP_

#include <cassert>
#include <cstdint>
#include <unordered_map>

#include "gtest/gtest.h"

#include "arch/riscv/immediate-node-factory.hpp"
#include "arch/riscv/instruction-node-factory.hpp"

using namespace riscv;

void testIntegerInstructionValidation(MemoryAccess& memAccess,
                                      const NodeFactoryCollection &instrF,
                                      ImmediateNodeFactory& immF,
                                      std::string instructionToken,
                                      bool isImmediateInstr);

void test12BitImmediateBounds(const NodeFactoryCollection& instrF,
                              std::string instructionToken,
                              ImmediateNodeFactory& immF, MemoryAccess& access);

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
#define TEST_RR(contextNbr, memoryValueConverter, instruction, operand1,      \
                operand2, result)                                             \
  /* Put operand values into register */                                      \
  memoryAccess.putRegisterValue(op1, memoryValueConverter(operand1));         \
  memoryAccess.putRegisterValue(op2, memoryValueConverter(operand2));         \
  auto cmd_##contextNbr = getFactories().createInstructionNode(instruction);  \
  ASSERT_FALSE(cmd_##contextNbr->validate(memoryAccess).isSuccess());         \
  /*Assemble instruction with destination & operands*/                        \
  cmd_##contextNbr->addChild(std::make_unique<RegisterNode>(dest));           \
  ASSERT_FALSE(cmd_##contextNbr->validate(memoryAccess).isSuccess());         \
  cmd_##contextNbr->addChild(std::make_unique<RegisterNode>(op1));            \
  ASSERT_FALSE(cmd_##contextNbr->validate(memoryAccess).isSuccess());         \
  cmd_##contextNbr->addChild(std::make_unique<RegisterNode>(op2));            \
  ASSERT_TRUE(cmd_##contextNbr->validate(memoryAccess).isSuccess());          \
  /* Save values of operand registers to determine change*/                   \
  MemoryValue preOp1_##contextNbr = memoryAccess.getRegisterValue(op1).get(); \
  MemoryValue preOp2_##contextNbr = memoryAccess.getRegisterValue(op2).get(); \
  /* Perform instruction*/                                                    \
  MemoryValue returnValue_##contextNbr =                                      \
      cmd_##contextNbr->getValue(memoryAccess);                               \
  /* Check that operand registers stayed the same*/                           \
  ASSERT_EQ(preOp1_##contextNbr, memoryAccess.getRegisterValue(op1).get());   \
  ASSERT_EQ(preOp2_##contextNbr, memoryAccess.getRegisterValue(op2).get());   \
  /* Read result from destination register*/                                  \
  MemoryValue actualResult_##contextNbr =                                     \
      memoryAccess.getRegisterValue(dest).get();                              \
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
  memoryAccess.putRegisterValue(reg, memoryValueConverter(operand1));          \
  /* Assemble instruction node with destination, operand & immediate node*/    \
  auto cmd_##contextNbr = getFactories().createInstructionNode(instruction);   \
  ASSERT_FALSE(cmd_##contextNbr->validate(memoryAccess).isSuccess())                       \
      << "empty instruction node validation failed";                           \
  cmd_##contextNbr->addChild(std::make_unique<RegisterNode>(dest));            \
  ASSERT_FALSE(cmd_##contextNbr->validate(memoryAccess).isSuccess())                       \
      << "instruction node + destination register node validation failed";     \
  cmd_##contextNbr->addChild(std::make_unique<RegisterNode>(reg));             \
  ASSERT_FALSE(cmd_##contextNbr->validate(memoryAccess).isSuccess())                       \
      << "instruction node + 2 register nodes validation failed";              \
  cmd_##contextNbr->addChild(                                                  \
      immediateFactory.createImmediateNode(memoryValueConverter(operand2)));   \
  ASSERT_TRUE(cmd_##contextNbr->validate(memoryAccess).isSuccess())                        \
      << "instruction node + 2 register + immediate node validation failed";   \
  /* Save value of operand register to determine change */                     \
  MemoryValue preRegisterOp_##contextNbr =                                     \
      memoryAccess.getRegisterValue(reg).get();                                \
  /* Perform instruction*/                                                     \
  MemoryValue returnValue_##contextNbr =                                       \
      cmd_##contextNbr->getValue(memoryAccess);                                \
  /* Check that register operand stayed the same*/                             \
  ASSERT_EQ(preRegisterOp_##contextNbr,                                        \
            memoryAccess.getRegisterValue(reg).get());                         \
  /* Read result from destination register */                                  \
  MemoryValue result_##contextNbr = memoryAccess.getRegisterValue(dest).get(); \
  ASSERT_EQ(memoryValueConverter(result), result_##contextNbr);

#endif /* ERAGPSIM_TESTS_ARCH_RISCV_ARITHMETIC_TEST_UTILS_HPP_*/
