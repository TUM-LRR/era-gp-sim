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
#include <memory>
#include <vector>

#include "gtest/gtest.h"

#include "arch/riscv/utility.hpp"
#include "core/memory-access.hpp"

TEST(UtilityTest, TestConversionsConvertWell) {
  uint32_t unsignedValue = 0xDEADBEEF;
  int16_t signedValue    = -0x1337;

  MemoryValue unsignedIntermediary = riscv::convert(unsignedValue);
  MemoryValue signedIntermediary   = riscv::convert(signedValue);

  uint32_t unsignedResult = riscv::convert<uint32_t>(unsignedIntermediary);
  int16_t signedResult    = riscv::convert<int16_t>(signedIntermediary);

  EXPECT_EQ(unsignedResult, unsignedValue);
  EXPECT_EQ(signedResult, signedValue);
}

TEST(UtilityTest, TestStoreLoadUtilitiesWorkWell) {
  uint32_t unsignedValue = 0xDEADBEEF;
  int16_t signedValue    = -0x1337;

  MemoryAccess memoryAccess;

  riscv::storeRegister(memoryAccess, "r0", unsignedValue);
  riscv::storeRegister(memoryAccess, "r1", signedValue);

  uint32_t unsignedResult = riscv::loadRegister<uint32_t>(memoryAccess, "r0");
  int16_t signedResult    = riscv::loadRegister<int16_t>(memoryAccess, "r1");

  EXPECT_EQ(unsignedValue, unsignedResult);
  EXPECT_EQ(signedValue, signedResult);
}
