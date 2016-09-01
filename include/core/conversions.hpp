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
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <vector>

#include "core/memory-value.hpp"

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
convert(const MemoryValue& memoryValue,
        const std::function<bool(const MemoryValue&)> sgn,
        const std::function<MemoryValue(const MemoryValue&)> abs) {
  T result = convert<T>(abs(memoryValue));
  if (sgn(memoryValue) ^ (result < 0)) return 0 - result;
  return result;
}

MemoryValue permute(const MemoryValue& memoryValue,
                    const std::size_t byteSize,
                    const std::vector<std::size_t> permutation);

MemoryValue permute(const MemoryValue& memoryValue,
                    const std::size_t byteSize,
                    const std::function<std::size_t(std::size_t)> permutation);

template <typename T>
typename std::enable_if<std::is_integral<T>::value, MemoryValue>::type
convert(T value,
        const std::function<MemoryValue(
            const std::vector<std::uint8_t>&, std::size_t, bool)> sgn,
        std::size_t size) {
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

MemoryValue
nonsignedC(const std::vector<std::uint8_t>& value, std::size_t size, bool sign);

MemoryValue
signBitC(const std::vector<std::uint8_t>& value, std::size_t size, bool sign);

MemoryValue onesComplementC(const std::vector<std::uint8_t>& value,
                            std::size_t size,
                            bool sign);

MemoryValue twosComplementC(const std::vector<std::uint8_t>& value,
                            std::size_t size,
                            bool sign);

const std::function<bool(const MemoryValue&)> unSignum0{unSignumA};
const std::function<bool(const MemoryValue&)> Signum0{signumA};

const std::function<MemoryValue(const MemoryValue&)> nonsigned1{nonsignedB};
const std::function<MemoryValue(const MemoryValue&)> signBit1{signBitB};
const std::function<MemoryValue(const MemoryValue&)> onesComplement1{
    onesComplementB};
const std::function<MemoryValue(const MemoryValue&)> twosComplement1{
    twosComplementB};

const std::function<MemoryValue(
    const std::vector<std::uint8_t>&, std::size_t, bool)>
    nonsigned2{nonsignedC};
const std::function<MemoryValue(
    const std::vector<std::uint8_t>&, std::size_t, bool)>
    signBit2{signBitC};
const std::function<MemoryValue(
    const std::vector<std::uint8_t>&, std::size_t, bool)>
    onesComplement2{onesComplementC};
const std::function<MemoryValue(
    const std::vector<std::uint8_t>&, std::size_t, bool)>
    twosComplement2{twosComplementC};

#endif// ERAGPSIM_CORE_ADVANCED_CONVERSIONS_HPP_
