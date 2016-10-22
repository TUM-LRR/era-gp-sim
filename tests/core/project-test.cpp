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

// clang-format off
#include "gtest/gtest.h"
#include "core/conversions.hpp"
#include "core/project-module.hpp"
#include "core/scheduler.hpp"
#include "core/servant.hpp"
#include "core/proxy.hpp"
#include "arch/common/architecture-formula.hpp"
#include "parser/final-representation.hpp"
#include "parser/parser-factory.hpp"
#include "parser/syntax-information.hpp"
// clang-format on

class Testservant : public Servant {
 public:
  Testservant(Proxy<Testservant>::WeakScheduler&& scheduler)
  : Servant(std::move(scheduler)) {
  }

  std::vector<CompileError> getErrorList() {
    return _parserErrorList;
  }

  int getLine() {
    return _line;
  }

  void setErrorList(std::vector<CompileError> error) {
    _parserErrorList = error;
  }

  void setLine(int line) {
    _line = line;
  }

 private:
  std::vector<CompileError> _parserErrorList;
  int _line;
};

class Testproxy : public Proxy<Testservant> {
 public:
  Testproxy(Proxy<Testservant>::WeakScheduler&& scheduler)
  : Proxy(std::move(scheduler)) {
  }

  POST_FUTURE(getErrorList)
  POST_FUTURE(getLine)
  POST(setErrorList)
  POST(setLine)
};

class ProjectTestFixture : public ::testing::Test {
 public:
  ProjectTestFixture()
  : formula("riscv", {"rv32i"})
  , projectModule(formula, memorySize, "riscv")
  , architectureValidator(Architecture::Brew(formula))
  , parserValidator(ParserFactory::createParser(
        architectureValidator, projectModule.getMemoryAccess(), "riscv"))
  , scheduler(std::make_shared<Scheduler>())
  , proxy(std::move(scheduler)) {
    ParserInterface parserInterface = projectModule.getParserInterface();
    parserInterface.setSetCurrentLineCallback(lineNumberCallback);
    parserInterface.setSetErrorListCallback(errorListCallback);
    architectureValidator.validate();
  }

  std::size_t findNextNode(
      MemoryAccess memoryAccess,
      std::unordered_map<MemoryAddress, std::size_t> addressCommandMapValidator,
      FinalRepresentation& finalRepresentation) {
    std::size_t nextInstructionAddress = conversions::convert<std::size_t>(
        memoryAccess.getRegisterValue("pc").get());
    if (nextInstructionAddress == 0) {
      return 0;
    }
    auto iterator = addressCommandMapValidator.find(nextInstructionAddress);
    if (iterator == addressCommandMapValidator.end()) {
      return finalRepresentation.commandList.size();
    }
    return iterator->second;
  }

  std::size_t memorySize = 1000;
  ArchitectureFormula formula;
  ProjectModule projectModule;
  Architecture architectureValidator;
  std::unique_ptr<Parser> parserValidator;
  std::shared_ptr<Scheduler> scheduler;
  Testproxy proxy;
  std::string testProgram =
      R"(lui x2, 0xDEADB

addi x0, x0, 0
addi x0, x0, 0)";
  std::function<void(int)> lineNumberCallback = [this](int line) {
    proxy.setLine(line);
  };
  std::function<void(const std::vector<CompileError>&)> errorListCallback =
      [this](const std::vector<CompileError>& errorList) {
        std::vector<CompileError> errors = errorList;
        proxy.setErrorList(errors);
      };
};

TEST_F(ProjectTestFixture, MemoryAccessTest) {
  MemoryAccess memoryAccess = projectModule.getMemoryAccess();

  EXPECT_EQ(memorySize, memoryAccess.getMemorySize().get());

  MemoryValue testValue = conversions::convert(11, 32);
  MemoryValue testValue2 = conversions::convert(1, 32);
  MemoryValue zero = conversions::convert(0, 32);
  MemoryValue testByte = conversions::convert(4, 8);
  MemoryValue testByte2 = conversions::convert(8, 8);

  EXPECT_NO_THROW(memoryAccess.putMemoryValueAt(0, testByte));
  EXPECT_EQ(testByte, memoryAccess.getMemoryValueAt(0).get());
  EXPECT_NO_THROW(memoryAccess.putMemoryValueAt(memorySize - 1, testByte2));
  EXPECT_NO_THROW(memoryAccess.putMemoryValueAt(0, testValue2));

  EXPECT_EQ(testValue2, memoryAccess.setMemoryValueAt(0, testValue).get());
  EXPECT_EQ(testByte2,
            memoryAccess.setMemoryValueAt(memorySize - 1, testByte).get());

  EXPECT_EQ(testByte, memoryAccess.getMemoryValueAt(memorySize - 1).get());
  EXPECT_EQ(testValue, memoryAccess.getMemoryValueAt(0, 4).get());

  for (int i = 0; i < 32; i++) {
    std::string registerName = std::string("x") + std::to_string(i);
    EXPECT_EQ(zero,
              memoryAccess.setRegisterValue(registerName, testValue2).get());
  }
  EXPECT_EQ(zero,
            memoryAccess.setRegisterValue(std::string("pc"), testValue2).get());

  for (int i = 0; i < 32; i++) {
    std::string registerName = std::string("x") + std::to_string(i);
    EXPECT_NO_THROW(memoryAccess.putRegisterValue(registerName, testValue));
  }
  EXPECT_NO_THROW(memoryAccess.putRegisterValue(std::string("pc"), testValue));

  for (int i = 0; i < 32; i++) {
    std::string registerName = std::string("x") + std::to_string(i);
    EXPECT_EQ(testValue, memoryAccess.getRegisterValue(registerName).get());
  }
  EXPECT_EQ(testValue, memoryAccess.getRegisterValue(std::string("pc")).get());
}

TEST_F(ProjectTestFixture, MemoryManagerTest) {
  MemoryManager memoryManager = projectModule.getMemoryManager();
  MemoryAccess memoryAccess = projectModule.getMemoryAccess();

  memoryManager.resetMemory();
  for (int i = 0; i < memorySize; i++) {
    EXPECT_EQ(
        0, conversions::convert<int>(memoryAccess.getMemoryValueAt(i).get()));
  }
  memoryManager.resetRegisters();
  for (int i = 0; i < 32; i++) {
    std::string registerName = std::string("x") + std::to_string(i);
    EXPECT_EQ(0,
              conversions::convert<int>(
                  memoryAccess.getRegisterValue(registerName).get()));
  }
  EXPECT_EQ(
      0, conversions::convert<int>(memoryAccess.getRegisterValue("pc").get()));
}

TEST_F(ProjectTestFixture, ArchitectureAccessTest) {
  ArchitectureAccess architectureAccess = projectModule.getArchitectureAccess();

  EXPECT_EQ(architectureValidator.getUnits(),
            architectureAccess.getRegisterUnits().get());
  // EXPECT_EQ(architectureValidator.getByteSize(),
  // architectureAccess.getByteSize().get());
  EXPECT_EQ(architectureValidator.getInstructions(),
            architectureAccess.getInstructionSet().get());
}

TEST_F(ProjectTestFixture, CommandInterfaceTest) {
  CommandInterface commandInterface = projectModule.getCommandInterface();
  MemoryAccess memoryAccess = projectModule.getMemoryAccess();

  FinalRepresentation finalRepresentationValidator =
      parserValidator->parse(testProgram, ParserMode::COMPILE);
  auto addressCommandMapValidator =
      finalRepresentationValidator.createMapping();

  commandInterface.parse(testProgram);

  // wait for commandInterfaceThread
  commandInterface.setBreakpoint(0).get();

  EXPECT_EQ(finalRepresentationValidator.errorList.size(),
            proxy.getErrorList().get().size());
  EXPECT_EQ(0, finalRepresentationValidator.errorList.size());

  // test if correct values for the assembled program are written into memory
  for (auto&& command : finalRepresentationValidator.commandList) {
    // MemoryValue assembledValidator = command.node->assemble();
    MemoryValue assembledValidator;
    MemoryValue assembledInMemory =
        memoryAccess
            .getMemoryValueAt(command.address, assembledValidator.getSize() / 8)
            .get();
    EXPECT_EQ(assembledValidator, assembledInMemory);
  }

  // test executing the next line
  std::size_t firstNode = findNextNode(
      memoryAccess, addressCommandMapValidator, finalRepresentationValidator);
  std::size_t nextNode = firstNode;
  std::size_t lastNode = firstNode;
  while (nextNode < finalRepresentationValidator.commandList.size()) {
    FinalCommand& currentCommand =
        finalRepresentationValidator.commandList.at(nextNode);
    memoryAccess.putRegisterValue("pc",
                                  currentCommand.node->getValue(memoryAccess));
    nextNode = findNextNode(
        memoryAccess, addressCommandMapValidator, finalRepresentationValidator);
    commandInterface.executeNextLine().get();
    if (nextNode < finalRepresentationValidator.commandList.size()) {
      lastNode = nextNode;
      FinalCommand& nextCommand =
          finalRepresentationValidator.commandList.at(nextNode);
      // EXPECT_EQ(nextCommand.position.lineStart, proxy.getLine().get());
    }
  }
  commandInterface.setExecutionPoint(1);
  // another command to make sure setExecutionPoint was already executed
  commandInterface.setBreakpoint(0).get();

  projectModule.reset();

  EXPECT_EQ(1, proxy.getLine().get());
  // test executing the whole program
  commandInterface.execute();
  // placeholder
  commandInterface.setBreakpoint(0).get();
  commandInterface.deleteBreakpoint(0);

  EXPECT_EQ(5, proxy.getLine().get());

  commandInterface.setExecutionPoint(1);

  commandInterface.setBreakpoint(3);
  commandInterface.executeToBreakpoint();
  commandInterface.setBreakpoint(3).get();

  EXPECT_EQ(3, proxy.getLine().get());

  commandInterface.executeToBreakpoint();
  commandInterface.setBreakpoint(3).get();

  EXPECT_EQ(5, proxy.getLine().get());
}

TEST_F(ProjectTestFixture, ParserInterfaceTest) {
  ParserInterface parserInterface = projectModule.getParserInterface();
  SyntaxInformation syntaxInformation = parserValidator->getSyntaxInformation();
  EXPECT_NO_THROW(
      parserInterface.getSyntaxRegex(SyntaxInformation::Token::Register));
  EXPECT_NO_THROW(
      parserInterface.getSyntaxRegex(SyntaxInformation::Token::Instruction));
  EXPECT_NO_THROW(
      parserInterface.getSyntaxRegex(SyntaxInformation::Token::Comment));
  EXPECT_NO_THROW(
      parserInterface.getSyntaxRegex(SyntaxInformation::Token::Label));
  EXPECT_NO_THROW(
      parserInterface.getSyntaxRegex(SyntaxInformation::Token::Immediate));
}
