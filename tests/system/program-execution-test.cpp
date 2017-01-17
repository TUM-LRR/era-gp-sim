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

#include "arch/riscv/utility.hpp"
#include "common/utility.hpp"
#include "core/memory-access.hpp"
#include "core/project-module.hpp"

#include "gtest/gtest.h"

using InitializerList = ArchitectureFormula::InitializerList;

static const std::string TEST_FILE_DIR = "tests/system/testfiles/";
static long timeout = 10000;  // 10s

template <typename SizeType, bool hasMulEnabled = false, size_t memory = 256>
struct ProgramExecutionFixture : public ::testing::Test {
  using ErrorCallback = ParsingAndExecutionUnit::Callback<Translateable>;

  ProgramExecutionFixture(const std::string& file) {
    loadArchitecture();
    loadFile(file);
    _project->getCommandInterface().setSyncCallback(
        [this] { this->_project->guiReady(); });
    _project->getCommandInterface().setExecutionStoppedCallback(
        [this] { running = false; });
    setCallbackOnError([](auto msg) {
      ASSERT_TRUE(false) << "Received an error: " << msg.getBaseString();
    });
    _project->getParserInterface().setFinalRepresentationCallback([](auto finalRep){
          if (finalRep.errorList().hasErrors()) {
              ASSERT_TRUE(false) << "Program generates errors!";
            }
      });
    _project->getCommandInterface().parse(_input);
  }

  void loadArchitecture() {
    using ModuleList = std::vector<std::string>;
    ModuleList modList;
    modList.emplace_back("rv32i");
    if (hasMulEnabled) {
      modList.emplace_back("rv32m");
    }
    if (sizeof(SizeType) == 64) {
      modList.emplace_back("rv64i");
      if (hasMulEnabled) {
        modList.emplace_back("rv64m");
      }
    }
    ArchitectureFormula formula("riscv", modList);
    _project = std::make_unique<ProjectModule>(formula, memory, "riscv");
  }

  void loadFile(const std::string& name) {
    auto path = Utility::joinToRoot(Utility::joinPaths(TEST_FILE_DIR, name));
    _input = Utility::loadFromFile(path);
  }

  MemoryAccess& getMemoryAccess() { return _project->getMemoryAccess(); }

  Architecture getArchitecture() {
    return _project->getArchitectureAccess().getArchitecture().get();
  }

  void executeAll() {
    running = true;
    _project->getCommandInterface().execute();
  }

  void executeNext() {
    running = true;
    _project->getCommandInterface().executeNextLine();
  }

  void waitUntilExecutionFinished() {
    using namespace std::chrono;
    auto start =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch())
            .count();
    while (running) {
      auto end =
          duration_cast<milliseconds>(system_clock::now().time_since_epoch())
              .count();
      ASSERT_TRUE(end - start < timeout) << "Test timouted (" << timeout
                                         << "ms)";
    }
  }

  void setCallbackOnError(const ErrorCallback& ecb) {
    _project->getParserInterface().setThrowErrorCallback(ecb);
  }

  void assertRegisterValue(const std::string& reg, SizeType exp) {
    MemoryValue value = getMemoryAccess().getRegisterValue(reg).get();
    SizeType resultValue = riscv::convert<SizeType>(value);
    EXPECT_EQ(exp, resultValue) << "For Register " << reg;
  }

  void assertForAllRegisters(
      const std::function<void(const std::string&, SizeType)>& f) {
    using Order = UnitInformation::AlphabeticOrder;
    using RegisterRef = std::reference_wrapper<const RegisterInformation>;
    MemoryAccess access = getMemoryAccess();
    for (auto unit : getArchitecture().getUnits()) {
      for (RegisterRef registerInfo : unit.getAllRegisterSorted(Order{})) {
        auto registerName = registerInfo.get().getName();
        MemoryValue val = access.getRegisterValue(registerName).get();
        SizeType regValue = riscv::convert<SizeType>(val);
        f(registerName, regValue);
      }
    }
  }

  void assertMemory(size_t begin, size_t length, SizeType expectedValue) {
    MemoryValue memoryValue =
        getMemoryAccess().getMemoryValueAt(begin, length).get();
    SizeType value = riscv::convert<SizeType>(memoryValue);
    EXPECT_EQ(expectedValue, value) << "For memory [" << begin << "; "
                                    << (begin + length) << "[";
  }

  std::unique_ptr<ProjectModule> _project;
  std::string _input;
  bool running = false;
};

template <typename SizeType>
struct FactorialTest : public ProgramExecutionFixture<SizeType, true> {
  FactorialTest() : ProgramExecutionFixture<SizeType, true>("factorial.txt") {}

  void factorialTest() {
    this->assertForAllRegisters([](auto reg, SizeType value) {
      ASSERT_EQ(0, value) << "In Register " << reg;
    });
    this->executeAll();
    this->waitUntilExecutionFinished();
    this->assertForAllRegisters([](auto reg, SizeType value) {
      if (reg != "x1") {
        EXPECT_EQ(0, value) << "In Register " << reg;
      }
    });
    this->assertRegisterValue("x1", 479001600);
  }
};

using FactorialTest32 = FactorialTest<int32_t>;

TEST_F(FactorialTest32, factorial32) { factorialTest(); }

using FactorialTest64 = FactorialTest<int64_t>;

TEST_F(FactorialTest64, factorial64) { factorialTest(); }

template <typename SizeType>
struct FactorialRecTest : public ProgramExecutionFixture<SizeType, true> {
  FactorialRecTest()
      : ProgramExecutionFixture<SizeType, true>("factorial_rec.txt") {}
  void factorialRecTest() {
    this->assertForAllRegisters([](auto reg, auto value) {
      ASSERT_EQ(0, value) << "In Register " << reg;
    });
    this->executeAll();
    this->waitUntilExecutionFinished();
    this->assertRegisterValue("x5", 479001600);
  }
};

using FactorialRecTest32 = FactorialRecTest<int32_t>;
TEST_F(FactorialRecTest32, factorialRec32) { factorialRecTest(); }

using FactorialRecTest64 = FactorialRecTest<int64_t>;
TEST_F(FactorialRecTest64, factorialRec64) { factorialRecTest(); }

template <typename SizeType>
struct MemoryIOTest : public ProgramExecutionFixture<SizeType, false, 1024> {
  MemoryIOTest()
      : ProgramExecutionFixture<SizeType, false, 1024>("memoryio.txt") {}
  void memoryioTest() {
    this->assertForAllRegisters([](auto reg, auto value) {
      ASSERT_EQ(0, value) << "In Register " << reg;
    });
    this->executeAll();
    this->waitUntilExecutionFinished();
    this->assertRegisterValue("x1", 0x489);
    this->assertMemory(0, 4, 0x489);
  }
};

using MemoryIOTest32 = MemoryIOTest<int32_t>;
TEST_F(MemoryIOTest32, memoryio32) { memoryioTest(); }

using MemoryIOTest64 = MemoryIOTest<int64_t>;
TEST_F(MemoryIOTest64, memoryio64) { memoryioTest(); }

template <typename SizeType>
struct SuperSumTest : public ProgramExecutionFixture<SizeType> {
  SuperSumTest() : ProgramExecutionFixture<SizeType>("super_sum.txt") {}

  void superSumTest() {
    this->assertForAllRegisters(
        [](auto reg, auto val) { ASSERT_EQ(0, val) << "In Register " << reg; });
    this->executeAll();
    this->waitUntilExecutionFinished();
    this->assertRegisterValue(
        "x5",
        819);  // See
               // http://www.wolframalpha.com/input/?i=sum+k%3D1+to+13+sum+l%3D1+to+k+k
  }
};

using SuperSumTest32 = SuperSumTest<int32_t>;
TEST_F(SuperSumTest32, supersum32) { superSumTest(); }

using SuperSumTest64 = SuperSumTest<int64_t>;
TEST_F(SuperSumTest64, supersum64) { superSumTest(); }
