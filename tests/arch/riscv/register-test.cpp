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

#include <climits>


#include "common/assert.hpp"
#include "gtest/gtest.h"

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/riscv/register-node-factory.hpp"
#include "arch/riscv/utility.hpp"
#include "core/memory-access.hpp"

#include "tests/arch/riscv/test-utils.hpp"

using namespace riscv;

struct RISCVRegisterTestFixture : RiscvBaseTest {
  void set(const std::string id, MemoryValue value) {
    getMemoryAccess().putRegisterValue(id, value);
  }

  void set(std::string prefix, int number, MemoryValue value) {
    set(prefix + std::to_string(number), value);
  }

  MemoryValue get(const std::string& id) {
    return getMemoryAccess().getRegisterValue(id).get();
  }

  MemoryValue get(std::string prefix, int number) {
    return get(prefix + std::to_string(number));
  }
};

TEST_F(RISCVRegisterTestFixture, testRV32I) {
  load({"rv32i"});

  MemoryAccess memoryAccess = getMemoryAccess();
  // x0 is read-only with 0
  ASSERT_TRUE(get("x0").isZero());
  set("x0", convert<uint32_t>(42));
  ASSERT_TRUE(get("x0").isZero());

  // test all other 31 registers
  for (auto i = 1; i < 32; ++i) {
    set("x", i, convert<uint32_t>(42));
    ASSERT_EQ(convert<uint32_t>(42), get("x", i));

    // factory works
    ASSERT_EQ(convert<uint32_t>(42),
              getFactories()
                  .createRegisterNode("x" + std::to_string(i))
                  ->getValue(memoryAccess));

    // is capable of holding 32bit
    set("x", i, convert<uint32_t>(UINT32_MAX));
    ASSERT_EQ(convert<uint32_t>(UINT32_MAX), get("x", i));
  }
}

TEST_F(RISCVRegisterTestFixture, testRV64I) {
  load({"rv32i", "rv64i"});
  MemoryAccess memoryAccess = getMemoryAccess();

  // x0 is read-only with 0
  ASSERT_TRUE(get("x0").isZero());
  set("x0", convert<uint64_t>(42));
  ASSERT_TRUE(get("x0").isZero());

  // test all other 31 registers
  for (auto i = 1; i < 32; ++i) {
    set("x", i, convert<uint64_t>(42));
    ASSERT_EQ(convert<uint64_t>(42), get("x", i));

    // factory works
    ASSERT_EQ(convert<uint64_t>(42),
              getFactories()
                  .createRegisterNode("x" + std::to_string(i))
                  ->getValue(memoryAccess));

    // is capable of holding 64bit
    set("x", i, convert<uint64_t>(UINT64_MAX));
    ASSERT_EQ(convert<uint64_t>(UINT64_MAX), get("x", i));
  }
}
