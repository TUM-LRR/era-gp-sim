#ifndef ERAGPSIM_CORE_TESTMEMORYVALUE_INC_
#define ERAGPSIM_CORE_TESTMEMORYVALUE_INC_

#include <random>

#include "core\memory-value.hpp"
#include "gtest\gtest.h"

namespace{
constexpr std::size_t scale = 1;
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
  constexpr std::size_t t = scale * s;// testAmount
  std::mt19937 rand{19930726u};       // Very important number, don't change
  for (std::size_t i = 1; i < b; ++i) {
    for (std::size_t j = 1; j < s; ++j) {
      const std::uniform_int_distribution<std::size_t> dist{};
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

#endif// ERAGPSIM_CORE_TESTMEMORYVALUE_INC_
