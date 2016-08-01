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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.*/

#ifndef ERAGPSIM_CORE_MEMORYVALUE_HPP
#define ERAGPSIM_CORE_MEMORYVALUE_HPP

#include <iostream>
#include <vector>
#include <cstdint>

enum class ByteOrder {
  kBigEndian,
  kLittleEndian
};

// Set of bits for in/out-putting to/from memory/registers
class MemoryValue {
private:
    uint64_t _value;
public:
    MemoryValue() : _value(0) {}
    explicit MemoryValue(uint64_t i);

    uint64_t getValue()const;
};

template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
convert(const MemoryValue& memoryValue, ByteOrder byteOrder) {
    uint64_t v= memoryValue.getValue();
    return T(v);
}
template <typename T>
typename std::enable_if<std::is_integral<T>::value, MemoryValue>::type
convert(T value, std::size_t bitsPerByte, ByteOrder byteOrder,
std::size_t byteCount = 0) {
    return MemoryValue(value);
}

#endif// ERAGPSIM_CORE_MEMORYVALUE_HPP
