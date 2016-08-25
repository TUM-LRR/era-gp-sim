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

#ifndef ERAGPSIM_CORE_CONVERSIONS_HPP_
#define ERAGPSIM_CORE_CONVERSIONS_HPP_

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
    result |= memoryValue.getByteAt(i * 8);
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
  if (sgn(memoryValue) ^ result < 0) return -result;
  return result;
}

MemoryValue permute(const MemoryValue& memoryValue,
                    const std::size_t byteSize,
                    const std::vector<std::size_t> permutation);

MemoryValue permute(const MemoryValue& memoryValue,
                    const std::size_t byteSize,
                    const std::function<std::size_t(std::size_t)> permutation);

bool signum(const MemoryValue& memoryValue);

MemoryValue signBit(const MemoryValue& memoryValue);

MemoryValue onesComplement(const MemoryValue& memoryValue);

MemoryValue twosComplement(const MemoryValue& memoryValue);

#endif// ERAGPSIM_CORE_CONVERSIONS_HPP_
