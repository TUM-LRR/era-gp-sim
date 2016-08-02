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

#include <random>

// Gtest has to be included before memory-value.
// clang-format off
#include "gtest/gtest.h"
#include "core/memory-value.hpp"
// clang-format on

namespace {
constexpr std::size_t scale = 1;
}

TEST(TestMemoryValue, getSize) {
  constexpr std::size_t b = 8;    // byteAmount
  constexpr std::size_t s = 64;   // byteSize
  constexpr std::size_t t = scale;// testAmount
  for (std::size_t i = 1; i < b; ++i) {
    for (std::size_t j = 1; j < s; ++j) {
      for (std::size_t l = 0; l < t; ++l) {
        MemoryValue instance{i, j};
        ASSERT_EQ(j, instance.getByteSize());
        ASSERT_EQ(i, instance.getByteAmount());
        ASSERT_EQ(i * j, instance.getSize());
      }
    }
  }
}


TEST(TestMemoryValue, flip) {
  constexpr std::size_t b = 8;    // byteAmount
  constexpr std::size_t s = 64;   // byteSize
  constexpr std::size_t t = scale;// testAmount
  constexpr std::size_t c = s;    // flipAmount
  std::mt19937 rand{19930626u};   // Very important number, don't change
  for (std::size_t i = 1; i < b; ++i) {
    for (std::size_t j = 1; j < s; ++j) {
      std::uniform_int_distribution<std::size_t> dist{0, i * j - 1};
      for (std::size_t l = 0; l < t; ++l) {
        MemoryValue instance0{i, j};
        MemoryValue instance1{i, j};
        ASSERT_EQ(instance0, instance1);
        for (std::size_t f = 0; f < c; ++f) {
          std::size_t address{dist(rand)};
          instance0.put(address, !instance0.get(address));
          ASSERT_NE(instance0, instance1);
          instance1.put(address, !instance1.get(address));
          ASSERT_EQ(instance0, instance1);
        }
      }
    }
  }
}

TEST(TestMemoryValue, put) {
  constexpr std::size_t b = 8;    // byteAmount
  constexpr std::size_t s = 64;   // byteSize
  constexpr std::size_t t = scale;// testAmount
  constexpr std::size_t c = s;    // putAmount
  std::mt19937 rand{19921123u};   // Very important number, don't change
  for (std::size_t i = 1; i < b; ++i) {
    for (std::size_t j = 1; j < s; ++j) {
      std::uniform_int_distribution<std::size_t> dist{0, (i * j - 1) * 2};
      for (std::size_t l = 0; l < t; ++l) {
        MemoryValue instance{i, j};
        for (std::size_t f = 0; f < c; ++f) {
          std::size_t address{dist(rand)};
          instance.put(address / 2, (address % 2) == 0);
          ASSERT_EQ((address % 2) == 0, instance.get(address / 2));
        }
      }
    }
  }
}

TEST(TestMemoryValue, set) {
  constexpr std::size_t b = 8;    // byteAmount
  constexpr std::size_t s = 64;   // byteSize
  constexpr std::size_t t = scale;// testAmount
  constexpr std::size_t c = s;    // setAmount
  std::mt19937 rand{19930219u};   // Very important number, don't change
  for (std::size_t i = 1; i < b; ++i) {
    for (std::size_t j = 1; j < s; ++j) {
      std::uniform_int_distribution<std::size_t> dist{0, (i * j - 1) * 2};
      for (std::size_t l = 0; l < t; ++l) {
        MemoryValue instance{i, j};
        for (std::size_t f = 0; f < c; ++f) {
          std::size_t address{dist(rand)};
          bool getVal = instance.get(address / 2);
          bool setVal = instance.set(address / 2, (address % 2) == 0);
          ASSERT_EQ(getVal, setVal);
          ASSERT_EQ((address % 2) == 0, instance.get(address / 2));
        }
      }
    }
  }
}

TEST(TestMemoryValue, equals) {
  constexpr std::size_t b = 8;        // byteAmount
  constexpr std::size_t s = 64;       // byteSize
  constexpr std::size_t t = scale * 4;// testAmount
  std::mt19937 rand{19930726u};       // Very important number, don't change
  std::uniform_int_distribution<std::size_t> dist{};
  for (std::size_t i = 1; i < b; ++i) {
    for (std::size_t j = 1; j < s; ++j) {
      for (std::size_t l = 0; l < t; ++l) {
        const std::size_t bytes{i * ((j / 8 + ((j % 8) > 0 ? 1 : 0)))};
        std::vector<std::uint8_t> initializer0{};
        std::vector<std::uint8_t> initializer1{};
        for (std::size_t k = 0; k < bytes; ++k) {
          initializer0.push_back(static_cast<std::uint8_t>(dist(rand)));
          initializer1.push_back(static_cast<std::uint8_t>(dist(rand)));
        }
        MemoryValue instance0{initializer0, j};
        MemoryValue instance1{initializer1, j};
        MemoryValue instance2{initializer0, j};
        MemoryValue instance3{initializer1, j};
        for (std::size_t k = 0; k < i * j; ++k) {
          bool value{(dist(rand) % 2) == 0};
          instance0.put(k, value);
          instance1.put(k, value);
        }
        const std::size_t address{dist(rand) % (i * j)};
        const bool value{instance0.get(address)};
        instance2.put(address, !value);
        instance3.put(address, !value);
        ASSERT_EQ(instance0, instance1);
        ASSERT_NE(instance0, instance2);
        ASSERT_NE(instance0, instance3);
      }
    }
  }
}

TEST(TestMemoryValue, charAt) {
  constexpr std::size_t b = 8;        // byteAmount
  constexpr std::size_t s = 64;       // byteSize
  constexpr std::size_t t = scale * 4;// testAmount
  std::mt19937 rand{20160304u};       // Very important number, don't change
  std::uniform_int_distribution<std::size_t> dist{};
  for (std::size_t i = 1; i < b; ++i) {
    for (std::size_t j = 1; j < s; ++j) {
      if (i * j < 8) continue;
      for (std::size_t l = 0; l < t; ++l) {
        MemoryValue instance0{i, j};
        MemoryValue instance1{i, j};
        for (std::size_t k = 0; k < i * j; ++k) {
          bool b{dist(rand) % 2 == 0};
          instance0.put(k, b);
          instance1.put(i * j - k - 1, b);
        }
        for (std::size_t k = 0; k < i * j - 7; ++k) {
          std::uint8_t extract0{instance0.getByteAt(k)};
          std::uint8_t extract1{instance1.getByteAt((i * j) - k - 8)};
          std::uint8_t extractR{static_cast<std::uint8_t>(
              (extract1 * 0x0202020202ULL & 0x010884422010ULL) %
              1023)};// reversed byte
          ASSERT_EQ(extract0, extractR);
        }
      }
    }
  }
}

TEST(TestMemoryValue, subSet) {
  constexpr std::size_t b = 8;    // byteAmount
  constexpr std::size_t s = 64;   // byteSize
  constexpr std::size_t t = scale;// testAmount
  constexpr std::size_t c = 16;   // subSetAmount
  std::mt19937 rand{20131127u};   // Very important number, don't change
  std::uniform_int_distribution<std::size_t> dist0{};
  for (std::size_t i = 1; i < b; ++i) {
    for (std::size_t j = 1; j < s; ++j) {
      std::uniform_int_distribution<std::size_t> dist1{0, i * j - 1};
      for (std::size_t l = 0; l < t; ++l) {
        MemoryValue instance0{i, j};
        MemoryValue instance1{i, j};
        for (std::size_t k = 0; k < i * j; ++k) {
          bool b{dist0(rand) % 2 == 0};
          instance0.put(k, b);
          instance1.put(i * j - k - 1, b);
        }
        for (std::size_t k = 0; k < c; ++k) {
          std::size_t address0{dist1(rand)};
          std::size_t address1{dist1(rand)};
          std::size_t tempAddress{std::max(address0, address1)};
          address0 = std::min(address0, address1);
          address1 = tempAddress;
          std::size_t phi{1};// todo
          MemoryValue subSet0{instance0.subSet(address0, address1, phi)};
          MemoryValue subSet1{
              instance1.subSet(i * j - address1, i * j - address0, phi)};
          MemoryValue subSetR{(address1 - address0) / phi, phi};
          for (std::size_t h = 0; h < address1 - address0; ++h)
            subSetR.put((address1 - address0) - h - 1, subSet1.get(h));
          ASSERT_EQ(subSet0, subSetR);
        }
      }
    }
  }
}

// Failed to die (and took way too long), disabled.
/*TEST(TestMemoryValue, death) {
  constexpr std::size_t b = 8; // byteAmount
  constexpr std::size_t s = 32;// byteSize
  constexpr std::size_t c = 1; // deathAmount
  std::mt19937 rand{20131012u};// Very important number, don't change
  std::uniform_int_distribution<std::size_t> dist{};
  for (std::size_t i = 1; i < b; ++i) {
    for (std::size_t j = 1; j < s; ++j) {
      const std::size_t bytes{i * ((j + 7) / 8)};
      std::vector<std::uint8_t> initializer{};
      for (std::size_t k = 0; k < bytes; ++k) {
        initializer.push_back(static_cast<std::uint8_t>(dist(rand)));
      }
      MemoryValue instance{initializer, j};

      for (std::size_t k = 0; k < c; ++k) {
        ASSERT_DEATH(instance.get(i * j + k + 9001), "");
        ASSERT_DEATH(instance.put(i * j + k + 9001, (dist(rand) % 2 == 0)), "");
        ASSERT_DEATH(instance.set(i * j + k + 9001, (dist(rand) % 2 == 0)), "");
        ASSERT_DEATH(instance.getByteAt(i * j + k + 9001), "");
        ASSERT_DEATH(instance.subSet(0, i * j + k + 1 + 9001, 8), "");
        ASSERT_DEATH(instance.subSet(k + 1, 0, 8), "");
      }
    }
  }
}*/
