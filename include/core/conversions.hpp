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
  ONES_COMPLEMENT,
  SIGN_BIT,
  TWOS_COMPLEMENT
};

// Unsigned and little endian
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
convert(const MemoryValue& memoryValue) {
  T result = 0;
  for (std::size_t i = memoryValue.internal().size(); i > 0; --i) {
    result <<= 8;
    result |= getByteAt(memoryValue, (i - 1) * 8);
    // result |= memoryValue.getByteAt((i-1) * 8);
  }
  return result;
}

// little endian
// true => negative
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
convert(const MemoryValue& memoryValue, const std::function<bool(const MemoryValue&)> sgn, const std::function<MemoryValue(const MemoryValue&)> abs) {
  T result = convert<T>(abs(memoryValue));
  if (sgn(memoryValue) ^ (result < 0)) return 0 - result;
  return result;
}

MemoryValue permute(const MemoryValue& memoryValue, const std::size_t byteSize, const std::vector<std::size_t> permutation);

MemoryValue permute(const MemoryValue& memoryValue, const std::size_t byteSize, const std::function<std::size_t(std::size_t)> permutation);

template <typename T>
typename std::enable_if<std::is_integral<T>::value, MemoryValue>::type
convert(T value, const std::function<MemoryValue(const std::vector<std::uint8_t>&, std::size_t, bool)> sgn, std::size_t size) {
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

bool unSignumA(const MemoryValue& memoryValue);

bool signumA(const MemoryValue& memoryValue);

MemoryValue nonsignedB(const MemoryValue& memoryValue);

MemoryValue signBitB(const MemoryValue& memoryValue);

MemoryValue onesComplementB(const MemoryValue& memoryValue);

MemoryValue twosComplementB(const MemoryValue& memoryValue);

MemoryValue nonsignedC(const std::vector<std::uint8_t>& value, std::size_t size, bool sign);

MemoryValue signBitC(const std::vector<std::uint8_t>& value, std::size_t size, bool sign);

MemoryValue onesComplementC(const std::vector<std::uint8_t>& value, std::size_t size, bool sign);

MemoryValue twosComplementC(const std::vector<std::uint8_t>& value, std::size_t size, bool sign);

extern const std::function<bool(const MemoryValue&)> unSignum0;
extern const std::function<bool(const MemoryValue&)> Signum0;

extern const std::function<MemoryValue(const MemoryValue&)> nonsigned1;
extern const std::function<MemoryValue(const MemoryValue&)> signBit1;
extern const std::function<MemoryValue(const MemoryValue&)> onesComplement1;
extern const std::function<MemoryValue(const MemoryValue&)> twosComplement1;

extern const std::function<MemoryValue(const std::vector<std::uint8_t>&, std::size_t, bool)> nonsigned2;
extern const std::function<MemoryValue(const std::vector<std::uint8_t>&, std::size_t, bool)> signBit2;
extern const std::function<MemoryValue(const std::vector<std::uint8_t>&, std::size_t, bool)> onesComplement2;
extern const std::function<MemoryValue(const std::vector<std::uint8_t>&, std::size_t, bool)> twosComplement2;

namespace {
class bigEndian {
public:
  bigEndian(std::size_t byteCount) :_byteCount{ byteCount } {}
  bigEndian() :bigEndian(8) {};
  bigEndian(const bigEndian&) = default;
  bigEndian& operator=(const bigEndian&) = default;
  bigEndian(bigEndian&&) = default;
  bigEndian& operator=(bigEndian&&) = default;
  std::size_t operator()(size_t x) {
    return _byteCount - x;
  }
private:
  std::size_t _byteCount;
};
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
convert(const MemoryValue& memoryValue, SignedRepresentation representation=SignedRepresentation::TWOS_COMPLEMENT, std::size_t byteSize = 8, Endianness byteOrder=Endianness::LITTLE) {
  MemoryValue permuted;
  switch (byteOrder){
  case Endianness::LITTLE:
    permuted = memoryValue;
    break;
  case Endianness::BIG:
    permuted = permute(memoryValue, byteSize, bigEndian{ memoryValue.getSize()/byteSize });
    break;
  default:
    assert::that(false);
  }
  if (!std::is_signed<T>::value) {
    return convert<T>(permuted, unSignum0, nonsigned1);
  }
  switch (representation){
  case SignedRepresentation::SIGN_BIT:
    return convert<T>(permuted, Signum0, signBit1);
  case SignedRepresentation::ONES_COMPLEMENT:
    return convert<T>(permuted, Signum0, onesComplement1);
  case SignedRepresentation::TWOS_COMPLEMENT:
    return convert<T>(permuted, Signum0, twosComplement1);
  default:
    assert::that(false);
  }
  return T{};
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value, MemoryValue>::type
convert(T value, SignedRepresentation representation=SignedRepresentation::TWOS_COMPLEMENT, std::size_t byteSize=8, Endianness byteOrder=Endianness::LITTLE) {
  MemoryValue converted;
  if (!std::is_signed<T>::value) {
    convert = convert<T>(value, nonsigned2, byteSize);
  } else {
    switch (representation) {
    case SignedRepresentation::SIGN_BIT:
      convert = convert<T>(value, signBit2, byteSize);
      break;
    case SignedRepresentation::ONES_COMPLEMENT:
      convert = convert<T>(value, onesComplement2, byteSize);
      break;
    case SignedRepresentation::TWOS_COMPLEMENT:
      convert = convert<T>(value, twosComplement2, byteSize);
      break;
    default:
      assert::that(false);
    }
  }
  switch (byteOrder) {
  case Endianness::LITTLE:
    return converted;
  case Endianness::BIG:
    return permute(converted, byteSize, bigEndian{ converted.getSize() / byteSize });
  default:
    assert::that(false);
  }
  return MemoryValue{};
}

#endif// ERAGPSIM_CORE_ADVANCED_CONVERSIONS_HPP_
