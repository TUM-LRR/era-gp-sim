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

#ifndef ERAGPSIM_CORE_ADVANCED_CONVERSIONS_HPP_
#define ERAGPSIM_CORE_ADVANCED_CONVERSIONS_HPP_

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <vector>

#include "core/memory-value.hpp"
#include "common/assert.hpp"

/**
* TODO: Replace with version from memory implementation
* Indicates the byte order of a memory value
*/
enum class Endianness {
  BIG,
  LITTLE
};

/**
* TODO: Replace with version from architecture implementation
* Indicates how signed numbers are represented
*/
enum class SignedRepresentation {
  UNSIGNED,
  ONES_COMPLEMENT,
  SIGN_BIT,
  TWOS_COMPLEMENT,
  SMART
};

using signFunction = std::function<bool(const MemoryValue&)>;
using toMemoryValueFunction = std::function<MemoryValue(const std::vector<std::uint8_t>&, std::size_t, bool)>;
using toIntegralFunction = std::function<MemoryValue(const MemoryValue&, bool)>;

extern const signFunction unSignum0;
extern const signFunction Signum0;

extern const toIntegralFunction nonsigned1;
extern const toIntegralFunction signBit1;
extern const toIntegralFunction onesComplement1;
extern const toIntegralFunction twosComplement1;

extern const toMemoryValueFunction nonsigned2;
extern const toMemoryValueFunction signBit2;
extern const toMemoryValueFunction onesComplement2;
extern const toMemoryValueFunction twosComplement2;

struct Conversion {
  signFunction sgn;
  toMemoryValueFunction toMem;
  toIntegralFunction toInt;
  Conversion(const signFunction &sgn, const toIntegralFunction &toInt, const toMemoryValueFunction &toMem) :sgn{ sgn }, toMem{ toMem }, toInt{ toInt } {}
  Conversion() :Conversion(unSignum0, nonsigned1, nonsigned2) {}
  Conversion(const Conversion&) = default;
  Conversion(Conversion&&) = default;
  Conversion& operator=(const Conversion&) = default;
  Conversion& operator=(Conversion&&) = default;
  ~Conversion() = default;
};

extern const Conversion nonsigned;
extern const Conversion signBit;
extern const Conversion onesComplement;
extern const Conversion twosComplement;

// Unsigned and little endian
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
convertLE(const MemoryValue& memoryValue) {
  std::vector<std::uint8_t> raw{ memoryValue.internal() };
  //TODO::use fancy bitmask stuff and remove dependency on getByteAt
  //const std::uint8_t mask{ 0xFF };
  //T result = raw.back()&mask;
  raw.pop_back();
  T result = getByteAt(memoryValue,raw.size() * 8);
  while (raw.size() > 0) {
    result <<= 8;
    result |= raw.back();
    raw.pop_back();
  }
  return result;
}

// little endian
// true => negative
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
convert(const MemoryValue& memoryValue, const signFunction &sgn, const toIntegralFunction &abs) {
  bool sign = sgn(memoryValue);
  T result = convertLE<T>(abs(memoryValue, sign));
  if (sign ^ (result < 0)) return 0 - result;
  return result;
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
convert(const MemoryValue& memoryValue, const Conversion &con) {
  return convert<T>(memoryValue,con.sgn,con.toInt);
}

MemoryValue permute(const MemoryValue& memoryValue, const std::size_t byteSize, const std::vector<std::size_t> permutation);

MemoryValue permute(const MemoryValue& memoryValue, const std::size_t byteSize, const std::function<std::size_t(std::size_t)> permutation);

template <typename T>
typename std::enable_if<std::is_integral<T>::value, MemoryValue>::type
convert(const T value, const toMemoryValueFunction& sgn, const std::size_t size) {
  // T abs{ std::min(value) };
  T abs{value};
  if (abs < 0) abs = 0 - abs;
  std::size_t sizeInByte{(size + 7) / 8};
  std::vector<std::uint8_t> raw{};
  for (std::size_t i = 0; i < sizeInByte; ++i) {
    raw.push_back(static_cast<std::uint8_t>(abs & 0xFF));
    abs >>= 8;
  }
  return sgn(raw, size, value < 0);
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value, MemoryValue>::type
convert(T value, const Conversion con, std::size_t size) {
  return convert<T>(value, con.toMem, size);
}

bool unSignumA(const MemoryValue& memoryValue);

bool signumA(const MemoryValue& memoryValue);

MemoryValue nonsignedB(const MemoryValue& memoryValue, bool sign);

MemoryValue signBitB(const MemoryValue& memoryValue, bool sign);

MemoryValue onesComplementB(const MemoryValue& memoryValue, bool sign);

MemoryValue twosComplementB(const MemoryValue& memoryValue, bool sign);

MemoryValue nonsignedC(const std::vector<std::uint8_t>& value, std::size_t size, bool sign);

MemoryValue signBitC(const std::vector<std::uint8_t>& value, std::size_t size, bool sign);

MemoryValue onesComplementC(const std::vector<std::uint8_t>& value, std::size_t size, bool sign);

MemoryValue twosComplementC(const std::vector<std::uint8_t>& value, std::size_t size, bool sign);

namespace {
struct bigEndian {
  std::size_t _byteCount;
  bigEndian(std::size_t byteCount) :_byteCount{ byteCount } {}
  bigEndian() :bigEndian(8) {};
  bigEndian(const bigEndian&) = default;
  bigEndian& operator=(const bigEndian&) = default;
  bigEndian(bigEndian&&) = default;
  bigEndian& operator=(bigEndian&&) = default;
  std::size_t operator()(size_t x) {
    return _byteCount - x;
  }
};
}

Conversion switchConversion(SignedRepresentation representation);

MemoryValue permute(const MemoryValue& memoryValue, Endianness byteOrder, std::size_t byteSize);

template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
convert(const MemoryValue& memoryValue, SignedRepresentation representation=SignedRepresentation::SMART, std::size_t byteSize = 8, Endianness byteOrder=Endianness::LITTLE) {
  MemoryValue permuted = permute(memoryValue, byteOrder, byteSize);
  Conversion con = switchConversion(representation);
  if (representation == SignedRepresentation::SMART && std::is_signed<T>::value) {
    con = twosComplement;
  }
  return convert<T>(permuted,con);
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value, MemoryValue>::type
convert(T value, SignedRepresentation representation=SignedRepresentation::SMART, std::size_t byteSize=8, Endianness byteOrder=Endianness::LITTLE) {
  Conversion con = switchConversion(representation);
  if (representation == SignedRepresentation::SMART && std::is_signed<T>::value) {
    con = twosComplement;
  }
  MemoryValue converted = convert<T>(value, con, byteSize);
  return permute(converted, byteOrder, byteSize);
}

#endif// ERAGPSIM_CORE_ADVANCED_CONVERSIONS_HPP_
