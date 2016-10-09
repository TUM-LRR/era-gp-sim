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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include <functional>
#include <iostream>

// clang-format off
#include "gtest/gtest.h"
#include "core/project-module.hpp"
#include "arch/common/architecture-formula.hpp"
// clang-format on

class ProjectTestFixture : public ::testing::Test {
 public:
  ProjectTestFixture()
  : formula("riscv", {"rv32i"})
  , projectModule(std::move(formula), 1000, "riscv")
  , lineValue(0)
  , lineExpected(0) {
    ParserInterface parserInterface = projectModule.getParserInterface();
    parserInterface.setSetCurrentLineCallback(
        _lineNumberCallback);
  }

  ArchitectureFormula formula;
  ProjectModule projectModule;
  int lineValue;
  int lineExpected;
  std::string testProgram =
      "";// TODO example program with normal instruction and jump
  std::function<void(int)> _lineNumberCallback = [this](int line) {
    lineValue = line;
    EXPECT_EQ(lineValue, lineExpected);
  };
};

TEST_F(ProjectTestFixture, MemoryAccessTest) {
    MemoryAccess memoryAccess = projectModule.getMemoryAccess();
    EXPECT_NO_THROW(memoryAccess.getMemory(0));
    EXPECT_NO_THROW(memoryAccess.getMemory(999));
    EXPECT_NO_THROW(memoryAccess.getMemory(0, 4));
    EXPECT_ANY_THROW(memoryAccess.getMemory(1000));
    EXPECT_ANY_THROW(memoryAccess.getMemory(-1));
    EXPECT_ANY_THROW(memoryAccess.getMemory(999, 4));

    MemoryValue testValue;

    EXPECT_NO_THROW(memoryAccess.putMemoryCell(0, testValue));
    EXPECT_NO_THROW(memoryAccess.putMemoryCell(999, testValue));
    EXPECT_NO_THROW(memoryAccess.putMemoryCell(0, testValue));
    EXPECT_ANY_THROW(memoryAccess.putMemoryCell(1000, testValue));
    EXPECT_ANY_THROW(memoryAccess.putMemoryCell(-1, testValue));
    EXPECT_ANY_THROW(memoryAccess.putMemoryCell(999, testValue));

    EXPECT_NO_THROW(memoryAccess.setMemoryCell(0, testValue));
    EXPECT_NO_THROW(memoryAccess.setMemoryCell(999, testValue));
    EXPECT_NO_THROW(memoryAccess.setMemoryCell(0, testValue));
    EXPECT_ANY_THROW(memoryAccess.setMemoryCell(1000, testValue));
    EXPECT_ANY_THROW(memoryAccess.setMemoryCell(-1, testValue));
    EXPECT_ANY_THROW(memoryAccess.setMemoryCell(999, testValue));

    for(int i = 0; i < 32; i++) {
        std::string registerName = std::string("x") + std::to_string(i);
        EXPECT_NO_THROW(memoryAccess.getRegisterValue(registerName));
    }
    EXPECT_NO_THROW(memoryAccess.getRegisterValue(std::string("pc")));

    for(int i = 0; i < 32; i++) {
        std::string registerName = std::string("x") + std::to_string(i);
        EXPECT_NO_THROW(memoryAccess.putRegisterValue(registerName, testValue));
    }
    EXPECT_NO_THROW(memoryAccess.putRegisterValue(std::string("pc"), testValue));

    for(int i = 0; i < 32; i++) {
        std::string registerName = std::string("x") + std::to_string(i);
        EXPECT_NO_THROW(memoryAccess.setRegisterValue(registerName, testValue));
    }
    EXPECT_NO_THROW(memoryAccess.setRegisterValue(std::string("pc"), testValue));
}

TEST_F(ProjectTestFixture, testExecuteLine) {
  CommandInterface commandInterface = projectModule.getCommandInterface();
}

TEST_F(ProjectTestFixture, testReset) {
}
