/*
* ERASIM Assembler Interpreter
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

#include <cstdint>
#include <random>

#include "gtest/gtest.h"

#include "common/assert.hpp"
#include "common/bit-builder.hpp"

std::uint64_t maskOfN(std::uint64_t width) {
  return (std::uint64_t(1) << width) - 1;
}

TEST(BitBuilderTest, CanBuildBits) {
  const int NUMBER_OF_ITERATIONS = 100;
  const std::uint64_t MAGIC_NUMBER = 123'456'789;

  std::random_device seed;
  std::mt19937 generator(seed());
  std::uniform_int_distribution<std::uint64_t> distribution;
  auto random = [&generator, &distribution]() -> std::uint64_t {
    return distribution(generator);
  };

  for (int iteration = 0; iteration < NUMBER_OF_ITERATIONS; ++iteration) {
    auto first = random();
    auto second = random();
    auto third = random();
    auto fourth = random();

    auto bits = BitBuilder<std::uint64_t>(MAGIC_NUMBER)
                    .addBit(first, 2)
                    .addUpTo(second, 8)
                    .addStartingAt(third, 14)
                    .addBit(fourth, 1)
                    .addBit(fourth, 2)
                    .addBit(fourth, 3)
                    .addBit(fourth, 4)
                    .build();

    // clang-format off
    std::uint64_t expected
      = MAGIC_NUMBER
      | ((first & 4) >> 2)
      | ((second & 511) << 1)
      | (((third & (maskOfN(50) << 14)) >> 14) << 10)
      | (((fourth & 30) >> 1) << 60);
    // clang-format on

    // Using ASSERT instead of EXPECT so the loop terminates early
    // clang-format off
    ASSERT_EQ(bits, expected)
      << first << " "
      << second << " "
      << third << " "
      << fourth;
    // clang-format on
  }
}

TEST(BitBuilderTest, FailsWhenExceedingLength) {
  EXPECT_THROW(BitBuilder<std::uint8_t>().add(std::uint64_t(500)),
               assert::AssertionError);
}

TEST(BitBuilderTest, FaislWhenAccessingInvalidBit) {
  EXPECT_THROW(BitBuilder<std::uint64_t>().addUpTo(std::uint8_t(500), 10),
               assert::AssertionError);

  EXPECT_THROW(BitBuilder<std::uint64_t>().addStartingAt(std::uint8_t(500), 10),
               assert::AssertionError);

  EXPECT_THROW(BitBuilder<std::uint64_t>().addRange(std::uint8_t(500), 30, 40),
               assert::AssertionError);

  EXPECT_THROW(BitBuilder<std::uint64_t>().addBit(std::uint64_t(500), 69),
               assert::AssertionError);
}
