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
#include <memory>
#include <vector>

#include "arch/common/instruction-format.hpp"
#include "arch/riscv/formats.hpp"

using namespace riscv;

// testing the immediate transformations
struct ImmediateFormatTestFixture : public ::testing::Test {
}

TEST_F(ImmediateFormatTestFixture, IFormat) {
}

TEST_F(ImmediateFormatTestFixture, SFormat) {
}

TEST_F(ImmediateFormatTestFixture, BFormat) {
}

TEST_F(ImmediateFormatTestFixture, UFormat) {
}

TEST_F(ImmediateFormatTestFixture, JFormat) {
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
