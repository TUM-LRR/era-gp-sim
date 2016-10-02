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

#include <cstdint>
#include <functional>
#include <random>
#include <vector>

// clang-format off
#include "gtest/gtest.h"
#include "core/memory-value.hpp"
#include "core/conversions.hpp"
// clang-format on

namespace {
//template <typename T>
struct Gen {
  std::mt19937 rand;// I need a new number
  std::uniform_int_distribution<std::uint16_t> dist;
  std::uint8_t cache{ 0 };
  bool cached{ false };

  Gen(std::uint32_t seed) : rand{ seed }, dist{} {}
  Gen() : Gen(19930626u) {}
  Gen(const Gen&) = default;
  Gen& operator=(const Gen&) = default;
  Gen(Gen&&) = default;
  Gen& operator=(Gen&&) = default;
  ~Gen() = default;

  std::uint8_t operator()() {
    cached ^= true;
    if (!cached) {//cached
      return cache;
    } else {//!cached
      std::uint16_t x{ dist(rand) };
      cache = static_cast<std::uint8_t>(x >> 8);
      return static_cast<std::uint8_t>(x & 0xFF);
    }
  }
};

struct MemGen {
  std::function<std::uint8_t()> gen;
  MemGen(std::function<std::uint8_t()> gen) :gen{ gen } {}
  MemGen() : MemGen{ Gen{} } {}
  MemGen(const MemGen&) = default;
  MemGen& operator=(const MemGen&) = default;
  MemGen(MemGen&&) = default;
  MemGen& operator=(MemGen&&) = default;
  ~MemGen() = default;

  MemoryValue operator()(std::size_t size) {
    std::size_t sizeInByte{ (size + 7) / 8 };
    std::vector<uint8_t> initializer{};
    for (std::size_t i = 0; i < sizeInByte; ++i) {
      initializer.push_back(gen());
    }
    return MemoryValue{ initializer,size };
  }
};
template <typename T,std::size_t size>
struct TGen {
  std::function<std::uint8_t()> gen;
  TGen(std::function<std::uint8_t()> gen) :gen{ gen } {}
  TGen() : TGen<T,size>( Gen{} ) {}
  TGen(const TGen&) = default;
  TGen& operator=(const TGen&) = default;
  TGen(TGen&&) = default;
  TGen& operator=(TGen&&) = default;
  ~TGen() = default;

  T operator()() {
    const std::size_t sizeInByte{ (size + 7) / 8 - 1 };
    T res = static_cast<T>(gen());
    for (std::size_t i = 0; i < sizeInByte; ++i) {
      res <<= 8;
      res |= gen();
    }
    return res;
  }
};
}

template <typename T, std::size_t size> T
testTypeMemoryValueType(const T &value, const toMemoryValueFunction &sgn1,
  const signFunction &sgn2,
  const toIntegralFunction &abs) {
  MemoryValue memoryValue{ convert(value, sgn1, size) };
  return convert<T>(memoryValue, sgn2, abs);
}

template <typename T> MemoryValue
testMemoryValueTypeMemoryValue(const MemoryValue &memoryValue,
  const toMemoryValueFunction &sgn1,
  const signFunction &sgn2,
  const toIntegralFunction &abs) {
  T value{ convert<T>(memoryValue, sgn2, abs) };
  return convert(value, sgn1, memoryValue.getSize());
}

template <typename T, std::size_t size, std::size_t testAmount> void
testConversion( std::function<T()> &generator,
  std::function<MemoryValue(std::size_t)> &memGenerator,
  const toMemoryValueFunction sgn1,
  const signFunction sgn2,
  const toIntegralFunction abs) {
  for (int i = 0; i < testAmount; ++i) {
    T instance0{ generator() };
    T instance1{ testTypeMemoryValueType<T,size>(instance0,sgn1,sgn2,abs) };
    ASSERT_EQ(instance0, instance1);
    MemoryValue instance2{ memGenerator(size) };
    MemoryValue instance3{ testMemoryValueTypeMemoryValue<T>(instance2,sgn1,sgn2,abs) };
    ASSERT_EQ(instance2, instance3);
  }
}

template <typename T, std::size_t size, std::size_t testAmount> void
testConversion(std::function<std::uint8_t()> &gen, Conversion con) {
  std::function<MemoryValue(std::size_t)> memGen = MemGen(gen);
  std::function<T()> tGen = TGen<T, size>(gen);
  testConversion<T, size, testAmount>(tGen, memGen, con.toMem, con.sgn, con.toInt);
}

TEST(TestConversions, nonsigned) {
  std::function<std::uint8_t()> gen = Gen();
  testConversion<std::uint8_t ,  8, 1024>(gen, nonsigned);
  testConversion<std::uint16_t, 16, 1024>(gen, nonsigned);
  testConversion<std::uint32_t, 32, 1024>(gen, nonsigned);
  testConversion<std::uint64_t, 64, 1024>(gen, nonsigned);
}

TEST(TestConversions, signBit) {
  std::function<std::uint8_t()> gen = Gen();
  testConversion<std::int8_t ,  8, 1024>(gen, signBit);
  testConversion<std::int16_t, 16, 1024>(gen, signBit);
  testConversion<std::int32_t, 32, 1024>(gen, signBit);
  testConversion<std::int64_t, 64, 1024>(gen, signBit);
}

TEST(TestConversions, onesComplement) {
  std::function<std::uint8_t()> gen = Gen();
  testConversion<std::int8_t ,  8, 1024>(gen, onesComplement);
  testConversion<std::int16_t, 16, 1024>(gen, onesComplement);
  testConversion<std::int32_t, 32, 1024>(gen, onesComplement);
  testConversion<std::int64_t, 64, 1024>(gen, onesComplement);
}

TEST(TestConversions, twosComplement) {
  std::function<std::uint8_t()> gen = Gen();
  testConversion<std::int8_t ,  8, 1024>(gen, twosComplement);
  testConversion<std::int16_t, 16, 1024>(gen, twosComplement);
  testConversion<std::int32_t, 32, 1024>(gen, twosComplement);
  testConversion<std::int64_t, 64, 1024>(gen, twosComplement);
}