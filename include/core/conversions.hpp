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

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <type_traits>
#include <vector>

/**
 * TODO: Dummy class until MemoryValue rework per feature/emptyMemoryValue
 *       is done.
 * Replace with #include "memory-value.hpp"
 */
class MemoryValue;

/**
 * TODO: Replace with version from memory implementation
 * Indicates the byte order of a memory value
 */
enum class ByteOrder {
  kBigEndian,
  kLittleEndian
};

/*
 * These functions convert a MemoryValue into the specified type.
 * To define additional conversions, use template specialization or
 * appropriate std::enable_if guards.
 */

/**
 * \brief Converts the \c MemoryValue into the specified integer type.
 * \tparam T The integer type that the MemoryValue should be converted to
 *           (verified via \c std::is_integral)
 * \param byteOrder The byte order (endianness) of the data
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
convert(const MemoryValue& memoryValue, ByteOrder byteOrder) {
  // Grab the number of bits in a memory cell from the memory value
  std::size_t bitsPerByte = memoryValue.getByteSize();
  
  // Calculate the number of items in the vector inside the memory value for a
  // single memory cell. The extra +7 causes the value to be rounded up (with
  // padding) instead of down.
  std::size_t internalItemsPadded = (bitsPerByte + 7) / 8;
  // This version ignores the padding
  std::size_t internalItemsUnpadded = bitsPerByte / 8;
  // Calculate the bit size of the padding
  std::size_t paddingBits = bitsPerByte - 8 * internalItemsUnpadded;
  
  // The final result
  T result = 0;
  
  // Iterate over the cells in the memoryValue
  for (std::size_t cell = 0, bitsConverted = 0;
       cell < memoryValue.internal().size();
       cell += internalItemsPadded) {
    // Transform the index for big-endian use
    std::size_t index = (byteOrder == ByteOrder::kBigEndian)
      ? memoryValue.internal().size() - internalItemsPadded - cell;
      : cell;
    // Iterate over that cell's internal data
    std::size_t offset = 0;
    for (; offset < internalItemsUnpadded; ++offset) {
      if (bitsConverted <= sizeof(T) - 8) {
        // Convert the entire item
        result <<= 8;
        result |= memoryValue.internal()[index + offset];
        bitsConverted += 8;
      } else if (bitsConverted < sizeof(T)) {
        // Convert only the leftmost bits
        std::size_t bitsNeeded = sizeof(T) - bitsConverted;
        result <<= bitsNeeded;
        result |= memoryValue.internal()[index + offset] >> (8 - bitsNeeded);
        bitsConverted += bitsNeeded;
      } else {
        // We are already done converting
        break;
      }
    }
    // Check the padded item if present and needed
    if (paddingBits > 0 && bitsConverted < sizeof(T)) {
      ++offset; // Offset now correctly points to the padded item
      std::size_t bitsNeeded = std::min(sizeof(T) - bitsConverted, paddingBits);
      result <<= bitsNeeded;
      result |= memoryValue.internal()[index + offset] >> (8 - bitsNeeded);
      bitsConverted += bitsNeeded;
    }
  }
  
  return result;
}

/*
 * These functions convert the specified type into a MemoryValue.
 * They do not need to be specialized (unless using enable_if). Instead,
 * they may simply be overloaded.
 */

/**
 * \brief Converts an integer into a MemoryValue with the specified settings
 * \tparam The integer type (verified via \c std::is_integral)
 * \param value The value to convert
 * \param bitsPerByte The number of bits in a byte
 * \param byteOrder The byte order (endianness)
 * \param byteCount If specified, pad the MemoryValue with zeroes, or truncate 
 *                  it to the specified size. Defaults to 0.
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value, MemoryValue>::type
convert(T value, std::size_t bitsPerByte, ByteOrder byteOrder,
        std::size_t byteCount = 0) {
  // First, split the value into the individual "bytes"
  std::vector<T> bytes;
  if (8 * sizeof(T) > bitsPerByte && value >= (1 << bitsPerByte)) {
    // The type is large enough to contain multiple bytes, and the data needs
    // more than one byte. In particular, this means that T can represent a bit
    // string of bitsPerByte ones.
    T mask = ((static_cast<T>(1) << bitsPerByte) - 1);
    while (value > 0 && (byteCount == 0 || bytes.size() < byteCount)) {
      // Read the last bitsPerByte bits from the value, and add them to the data
      bytes.push_back(value & mask);
      value >>= bitsPerByte;
    }
  } else {
    // The value will fit into a single byte.
    bytes.push_back(value);
  }
  // Apply byte order
  if (byteOrder == ByteOrder::kLittleEndian) {
    std::reverse(bytes.begin(), bytes.end());
  }
  // Pad the bytes if necessary
  if (bytes.size() < byteCount) {
    switch (byteOrder) {
      case ByteOrder::kBigEndian:
        bytes.insert(bytes.end(), byteCount - bytes.size(), 0);
        break;
      case ByteOrder::kLittleEndian:
        bytes.insert(bytes.begin(), byteCount - bytes.size(), 0);
        break;
    }
  }
  // Now, turn the bytes into the uint8_t representation used internally
  std::vector<uint8_t> raw;
  std::size_t chunksPerByte = (bitsPerByte + 7) / 8;
  for (T byte : bytes) {
    std::deque<uint8_t> byteData; // Data for this byte only
    // Split the byte into actual uint8_t chunks
    for (std::size_t offset = 0; offset < chunksPerByte; ++offset) {
      byteData.push_front(byte & 0xFF);
      byte >>= 8;
    }
    // Insert the data
    raw.insert(raw.end(), byteData.begin(), byteData.end());
  }
  // Create a memory value, and return it
  return MemoryValue(std::move(raw), bitsPerByte);
}

#endif // ERAGPSIM_CORE_CONVERSIONS_HPP_
