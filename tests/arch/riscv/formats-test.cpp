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

#include <gtest/gtest.h>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include "arch/common/instruction-format.hpp"
#include "arch/riscv/formats.hpp"
#include "core/memory-value.hpp"

using namespace riscv;

// testing the immediate transformations
struct ImmediateFormatTestFixture : public ::testing::Test {
  ImmediateFormatTestFixture() {
    for (int i  = 0; i < 6; i++)
      valueP[i] = new MemoryValue(
          std::vector<uint8_t>(std::begin(v[i]), std::end(v[i])), 8);
  }

  ~ImmediateFormatTestFixture() {
    for (int i = 0; i < 6; i++) delete valueP[i];
    delete[] valueP;
  }

  uint8_t v[6][4] = {{0x78, 0xEF, 0xCD, 0xAB},
                     {0xBC, 0xFA, 0xFF, 0xFF},
                     {0xBE, 0xFA, 0xFF, 0xFF},
                     {0xBE, 0xF2, 0xFF, 0xFF},
                     {0X00, 0xE0, 0xCD, 0xAB},
                     {0xBC, 0xE2, 0xFD, 0xFF}};
  MemoryValue **valueP = new MemoryValue *[6];
};

TEST_F(ImmediateFormatTestFixture, IFormat) {
  immToI(*valueP[0]);
  ASSERT_EQ(*valueP[1], *valueP[0]);
}

TEST_F(ImmediateFormatTestFixture, SFormat) {
  immToS(*valueP[0]);
  ASSERT_EQ(*valueP[2], *valueP[0]);
}

TEST_F(ImmediateFormatTestFixture, BFormat) {
  immToB(*valueP[0]);
  ASSERT_EQ(*valueP[3], *valueP[0]);
}

TEST_F(ImmediateFormatTestFixture, UFormat) {
  immToU(*valueP[0]);
  ASSERT_EQ(*valueP[4], *valueP[0]);
}

TEST_F(ImmediateFormatTestFixture, JFormat) {
  immToJ(*valueP[0]);
  ASSERT_EQ(*valueP[5], *valueP[0]);
}

// testing the different formats
TEST(InstructionFormatTest, IFormat) {
}

TEST(InstructionFormatTest, SFormat) {
}

TEST(InstructionFormatTest, SBFormat) {
}

TEST(InstructionFormatTest, UFormat) {
}

TEST(InstructionFormatTest, UJFormat) {
}
