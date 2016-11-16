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

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

// Gtest has to be included before memory-value.
// clang-format off
#include "gtest/gtest.h"
#include "core/memory-value.hpp"
#include "core/memory.hpp"
// clang-format on

namespace {
constexpr std::size_t scale = 10;
}

TEST(memory, r_w) {
  constexpr std::size_t b = 256;  // byteSize
  constexpr std::size_t c = 256;  // byteCount
  constexpr std::size_t t = scale;// testAmount
  std::uniform_int_distribution<std::uint16_t> dist{0, 255};
  std::mt19937 rand(0);// I need new numbers, I'm kinda really out of ideas
  std::size_t byteCount{1};
  std::size_t inc{0};
  while (byteCount <= c) {
    std::uniform_int_distribution<std::size_t> addressDist{0, byteCount - 1};
    for (std::size_t i = 1; i < b; ++i) {
      std::size_t byteSizeInByte{(i + 7) / 8};
      for (std::size_t j = 0; j < t; ++j) {
        const std::size_t rep{(byteCount + 9) / 10};
        Memory instance{byteCount, i};
        for (std::size_t l = 0; l < rep; ++l) {
          std::vector<std::uint8_t> initializer{};
          for (std::size_t k = 0; k < byteSizeInByte; ++k) {
            initializer.push_back(static_cast<std::uint8_t>(dist(rand)));
          }
          MemoryValue instance0{initializer, i};
          std::size_t address{addressDist(rand)};
          MemoryValue previous{instance.get(address)};
          MemoryValue instance1{instance.set(address, instance0)};
          ASSERT_EQ(instance0, instance.get(address));
          ASSERT_EQ(previous, instance1);
        }
      }
    }
    byteCount += ++inc;
  }
}
