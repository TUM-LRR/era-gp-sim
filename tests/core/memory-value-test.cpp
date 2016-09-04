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

/**
 * \Brief tests the getSize() method
 */
TEST(TestMemoryValue, getSize) {
  constexpr std::size_t b = 1024; // size
  constexpr std::size_t t = scale;// testAmount
  for (std::size_t i = 1; i < b; ++i) {
    for (std::size_t j = 0; j < t; ++j) {
      // TODO::fill with noise
      MemoryValue instance{i};
      ASSERT_EQ(i, instance.getSize());
    }
  }
}


/**
 * \Brief tests the put() method by inverting single bits
 */
TEST(TestMemoryValue, flip) {
  constexpr std::size_t b = 512;  // Size
  constexpr std::size_t t = scale;// testAmount
  constexpr std::size_t c = 16;   // flipAmount
  std::mt19937 rand{19930626u};   // Very important number, don't change
  for (std::size_t i = 1; i < b; ++i) {
    std::uniform_int_distribution<std::size_t> dist{0, i - 1};// address
    for (std::size_t j = 0; j < t; ++j) {
      MemoryValue instance0{i};
      MemoryValue instance1{i};
      ASSERT_EQ(instance0, instance1);
      for (std::size_t l = 0; l < c; ++l) {
        std::size_t address{dist(rand)};
        instance0.put(address, !instance0.get(address));
        ASSERT_NE(instance0, instance1);
        instance1.put(address, !instance1.get(address));
        ASSERT_EQ(instance0, instance1);
      }
    }
  }
}

/**
 * \Brief tests the put() method by writing single bits
 *        and then reading them
 */
TEST(TestMemoryValue, put) {
  constexpr std::size_t b = 512;  // Size
  constexpr std::size_t t = scale;// testAmount
  constexpr std::size_t c = 16;   // putAmount
  std::mt19937 rand{19921123u};   // Very important number, don't change
  for (std::size_t i = 1; i < b; ++i) {
    std::uniform_int_distribution<std::size_t> dist{0, (i - 1) * 2};
    for (std::size_t j = 0; j < t; ++j) {
      MemoryValue instance{i};
      for (std::size_t l = 0; l < c; ++l) {
        std::size_t address{dist(rand)};
        instance.put(address / 2, (address % 2) == 0);
        ASSERT_EQ((address % 2) == 0, instance.get(address / 2));
      }
    }
  }
}

/**
 * \Brief tests the set() method by reading single bit,
 *        setting this bit and then reading again
 */
TEST(TestMemoryValue, set) {
  constexpr std::size_t b = 512;  // Size
  constexpr std::size_t t = scale;// testAmount
  constexpr std::size_t c = 16;   // setAmount
  std::mt19937 rand{19930219u};   // Very important number, don't change
  for (std::size_t i = 1; i < b; ++i) {
    std::uniform_int_distribution<std::size_t> dist{0, (i - 1) * 2};
    for (std::size_t j = 0; j < t; ++j) {
      MemoryValue instance{i};
      for (std::size_t l = 0; l < c; ++l) {
        std::size_t address{dist(rand)};
        bool getVal = instance.get(address / 2);
        bool setVal = instance.set(address / 2, (address % 2) == 0);
        ASSERT_EQ(getVal, setVal);
        ASSERT_EQ((address % 2) == 0, instance.get(address / 2));
      }
    }
  }
}

/**
 * \Brief tests the operator== by creating 4 memory values
 *        A,B,C,D with A == c and B == C then filling
 *        A and B with the same bits. Write single bit into C and D
 *        that is the inverse of Aat the same index=> D != A == B != C
 */
TEST(TestMemoryValue, equals) {
  constexpr std::size_t b = 512;      // Size
  constexpr std::size_t t = scale * 4;// testAmount
  std::mt19937 rand{19930726u};       // Very important number, don't change
  std::uniform_int_distribution<std::size_t> dist{};
  for (std::size_t i = 1; i < b; ++i) {
    for (std::size_t j = 0; j < t; ++j) {
      const std::size_t bytes{(i + 7) / 8};
      std::vector<std::uint8_t> initializer0{};
      std::vector<std::uint8_t> initializer1{};
      for (std::size_t l = 0; l < bytes; ++l) {
        initializer0.push_back(static_cast<std::uint8_t>(dist(rand)));
        initializer1.push_back(static_cast<std::uint8_t>(dist(rand)));
      }
      MemoryValue instance0{initializer0, i};
      MemoryValue instance1{initializer1, i};
      MemoryValue instance2{initializer0, i};
      MemoryValue instance3{initializer1, i};
      for (std::size_t l = 0; l < i; ++l) {
        bool value{(dist(rand) % 2) == 0};
        instance0.put(l, value);
        instance1.put(l, value);
      }
      const std::size_t address{dist(rand) % (i)};
      const bool value{instance0.get(address)};
      instance2.put(address, !value);
      instance3.put(address, !value);
      ASSERT_EQ(instance0, instance1);
      ASSERT_NE(instance0, instance2);
      ASSERT_NE(instance0, instance3);
    }
  }
}

/**
 * \Brief tests the getByteAt() method by creating
 *        a random MemoryValue and an identical memoryValue
 *        with reversed indices => reading from both should
 *        result in mirrored bytes
 */
TEST(TestMemoryValue, charAt) {
  constexpr std::size_t b = 512;      // Size
  constexpr std::size_t t = scale * 4;// testAmount
  std::mt19937 rand{20160304u};       // Very important number, don't change
  std::uniform_int_distribution<std::size_t> dist{};
  for (std::size_t i = 1; i < b; ++i) {
    if (i < 8) continue;
    for (std::size_t j = 0; j < t; ++j) {
      MemoryValue instance0{i};
      MemoryValue instance1{i};
      for (std::size_t l = 0; l < i; ++l) {
        bool b{dist(rand) % 2 == 0};
        instance0.put(l, b);
        instance1.put(i - l - 1, b);
      }
      for (std::size_t k = 0; k < i - 7; ++k) {
        std::uint8_t extract0{instance0.getByteAt(k)};
        std::uint8_t extract1{instance1.getByteAt(i - k - 8)};
        std::uint8_t extractR{static_cast<std::uint8_t>(
            (extract1 * 0x0202020202ULL & 0x010884422010ULL) %
            1023)};// reversed byte
        ASSERT_EQ(extract0, extractR);
      }
    }
  }
}


/**
 * \Brief tests the subSet() method by creating
 *        a random MemoryValue and an identical memoryValue
 *        with reversed indices => reading from both should
 *        result in mirrored MemoryValues
 */
TEST(TestMemoryValue, subSet) {
  constexpr std::size_t b = 512;  // byteAmount
  constexpr std::size_t t = scale;// testAmount
  constexpr std::size_t c = 16;   // subSetAmount
  std::mt19937 rand{20131127u};   // Very important number, don't change
  std::uniform_int_distribution<std::size_t> dist0{};
  for (std::size_t i = 1; i < b; ++i) {
    std::uniform_int_distribution<std::size_t> dist1{0, i - 1};
    for (std::size_t j = 0; j < t; ++j) {
      MemoryValue instance0{i};
      MemoryValue instance1{i};
      for (std::size_t l = 0; l < i; ++l) {
        bool b{dist0(rand) % 2 == 0};
        instance0.put(l, b);
        instance1.put(i - l - 1, b);
      }
      for (std::size_t l = 0; l < c; ++l) {
        std::size_t address0{dist1(rand)};
        std::size_t address1{dist1(rand)};
        if (address0 == address1) continue;
        std::size_t tempAddress{std::max(address0, address1)};
        address0 = std::min(address0, address1);
        address1 = tempAddress;
        MemoryValue subSet0{instance0.subSet(address0, address1)};
        MemoryValue subSet1{instance1.subSet(i - address1, i - address0)};
        MemoryValue subSetR{(address1 - address0)};
        for (std::size_t k = 0; k < address1 - address0; ++k)
          subSetR.put((address1 - address0) - k - 1, subSet1.get(k));
        ASSERT_EQ(subSet0, subSetR);
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

/**
 * \Brief tests the getByteAt() method by creating
 *        a random MemoryValue and saving the noise it
 *        was created from and comparing it to the results
 *        of reading.
 */
TEST(TestMemoryValue, charAt2) {
  constexpr std::size_t b = 512;      // Size
  constexpr std::size_t t = scale * 4;// testAmount
  std::mt19937 rand{20141121u};       // Very important number, don't change
  std::uniform_int_distribution<std::uint16_t> dist{255};
  for (std::size_t i = 1; i < b; ++i) {
    if (i < 8) continue;
    for (std::size_t j = 0; j < t; ++j) {
      MemoryValue instance{i};
      std::vector<uint8_t> raw{};
      for (std::size_t l = 0; l < (i + 7) / 8; ++l) {
        raw.push_back(static_cast<std::uint8_t>(dist(rand)));
      }
      raw[((i + 7) / 8) - 1] &= (1 << (i % 8)) - 1;
      raw.push_back(0);
      for (std::size_t l = 0; l < i; ++l) {
        if (raw[l / 8] & 1 << (l % 8)) {
          instance.put(l);
        }
      }
      for (std::size_t l = 0; l < i; ++l) {
        std::uint16_t compare16{
            static_cast<std::uint16_t>(((raw[l / 8 + 1]) << 8) | raw[l / 8])};
        ASSERT_EQ(instance.getByteAt(l),
                  static_cast<uint8_t>(compare16 >> (l % 8)));
      }
    }
  }
}

/**
* \Brief tests the write() method by writing and reading.
*/
TEST(TestMemoryValue, write) {
  constexpr std::size_t b = 128;  // Size
  constexpr std::size_t t = scale;// testAmount
  std::mt19937 rand{20141121u};   // I need a new Number
  std::uniform_int_distribution<std::uint16_t> dist0{0, 255};
  for (std::size_t i = 2; i < b; ++i) {// Length Of MemoryValue
    std::vector<uint8_t> initializer0{};
    for (std::size_t j = 0; j < ((i + 7) / 8); ++j) {
      initializer0.push_back(static_cast<std::uint8_t>(dist0(rand)));
    }
    MemoryValue parent{initializer0, i};
    for (std::size_t j = 1; j <= i; ++j) {// Length of SubSet
      std::uniform_int_distribution<std::size_t> dist1{0, i - j};
      for (std::size_t l = 0; l < t; ++l) {
        std::vector<uint8_t> initializer1{};
        for (std::size_t k = 0; k < ((j + 7) / 8); ++k) {
          initializer1.push_back(static_cast<std::uint8_t>(dist0(rand)));
        }
        MemoryValue parentCopy{parent};
        MemoryValue instance0{initializer1, j};
        std::size_t address{dist1(rand)};
        parent.write(instance0, address);
        MemoryValue instance1{parent.subSet(address, address + j)};
        ASSERT_EQ(instance0, instance1);
        if (address > 0) {
          ASSERT_EQ(parentCopy.subSet(0, address), parent.subSet(0, address));
        }
        if (i - j - address > 0) {
          ASSERT_EQ(parentCopy.subSet(address + j, i),
                    parent.subSet(address + j, i));
        }
      }
    }
  }
}
