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

#ifndef ERAGPSIM_CORE_CONVERSIONS_HPP
#define ERAGPSIM_CORE_CONVERSIONS_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <type_traits>
#include <vector>

#include "memory-value.hpp"

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


/*
 * These functions convert a MemoryValue into the specified type.
 * To define additional conversions, use template specialization or
 * appropriate std::enable_if guards.
 */


/**
 * \brief Converts the \c MemoryValue into the specified unsigned integer type.
 * \tparam T The integer type that the MemoryValue should be converted to
 *           (verified via \c std::is_integral and \c std::is_unsigned)
 * \param byteOrder The byte order (endianness) of the data
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value && 
                        std::is_unsigned<T>::value, T>::type
convert(const MemoryValue& memoryValue, Endianness byteOrder) {
  // Grab the number of bits we need to read
  std::size_t bits = memoryValue.getSize();
  
  // Grab the number of bits in a memory cell from the memory value
  std::size_t bitsPerByte = memoryValue.getByteSize();
  
  // Calculate the number of items in the vector inside the memory value for a
  // single memory cell. The extra +7 causes the value to be rounded up (with
  // padding) instead of down.
  std::size_t internalItemsPadded = (bitsPerByte + 7) / 8;
  // This version ignores the padding
  std::size_t internalItemsUnpadded = bitsPerByte / 8;
  // Calculate the bit size of the padding
  std::size_t paddingBits = (bitsPerByte % 8 == 0) ? 0 : 8 - (bitsPerByte % 8);
  
  // The final result
  T result = 0;
  
  // Iterate over the cells in the memoryValue
  for (std::size_t cell = 0, bitsConverted = 0;
       cell < memoryValue.internal().size();
       cell += internalItemsPadded) {
    // Transform the index for big-endian use
    std::size_t index = (byteOrder == Endianness::BIG)
      ? memoryValue.internal().size() - internalItemsPadded - cell
      : cell;
    // Iterate over that cell's internal data
    std::size_t offset = 0;
    for (; offset < internalItemsUnpadded; ++offset) {
      if (bitsConverted <= bits - 8) {
        // Convert the entire item
        result <<= 8;
        result |= memoryValue.internal()[index + offset];
        bitsConverted += 8;
      } else if (bitsConverted < bits) {
        // Convert only the leftmost bits
        std::size_t bitsNeeded = std::min(8UL, bits - bitsConverted);
        result <<= bitsNeeded;
        result |= memoryValue.internal()[index + offset] >> (8 - bitsNeeded);
        bitsConverted += bitsNeeded;
      } else {
        // We are already done converting
        ++offset;
        break;
      }
    }
    // Check the padded item if present and needed
    if (paddingBits > 0 && bitsConverted < bits) {
      std::size_t bitsNeeded = std::min(bits - bitsConverted,
                                        8 - paddingBits);
      result <<= bitsNeeded;
      result |= memoryValue.internal()[index + offset] >> (8 - bitsNeeded);
      bitsConverted += bitsNeeded;
    }
  }
  
  return result;
}


/**
 * \brief Converts the \c MemoryValue into the specified signed integer type.
 * \tparam T The integer type that the MemoryValue should be converted to
 *           (verified via \c std::is_integral and \c std::is_signed)
 * \tparam U An unsigned integer type capable of holding the same number of bits
 *           as T (defaults to \c std::uintmax_t)
 * \param byteOrder The byte order (endianness) of the data
 * \param representation Describes the encoding of the signed integer
 */
template <typename T, typename U = std::uintmax_t>
typename std::enable_if<std::is_integral<T>::value && 
                        std::is_signed<T>::value &&
                        std::is_integral<U>::value &&
                        std::is_unsigned<U>::value, T>::type
convert(const MemoryValue& memoryValue, Endianness byteOrder,
        SignedRepresentation representation) {
  std::size_t bitsInT = std::numeric_limits<T>::digits; // Bits in type T
  std::size_t bitsInU = std::numeric_limits<U>::digits; // Bits in type U
  assert(bitsInT >= bitsInU - 1);
  std::size_t numberOfBits = std::min(bitsInU, memoryValue.getSize());
  U unsignedRawValue = convert<U>(memoryValue, byteOrder);

  // Bitmasks to extract the right values
  U firstBit = static_cast<U>(1) << (numberOfBits - 1);
  U exceptFirstBit = firstBit - 1;
  U entireValue = exceptFirstBit | firstBit;
  // The result
  T result;
  // Depending on the representation, convert the unsigned value into the signed
  // value
  switch (representation) {
    case SignedRepresentation::ONES_COMPLEMENT:
      // If the first bit is set, flip all the bits prior to conversion, then
      // negate the result
      if (unsignedRawValue & firstBit) {
        result = -static_cast<T>((~unsignedRawValue) & exceptFirstBit);
      } else {
        result = static_cast<T>(unsignedRawValue & exceptFirstBit);
      }
      break;
    case SignedRepresentation::SIGN_BIT:
      // Remove the sign bit from the output, then negate if needed
      result = static_cast<T>(unsignedRawValue & exceptFirstBit);
      if (unsignedRawValue & firstBit) result = -result;
      break;
    case SignedRepresentation::TWOS_COMPLEMENT:
      // If the first bit is set, flip all the bits prior to conversion, then
      // negate the result (like one's complement) and subtract one
      if (unsignedRawValue & firstBit) {
        result = -static_cast<T>((~unsignedRawValue) & exceptFirstBit) - 1;
      } else {
        result = static_cast<T>(unsignedRawValue & exceptFirstBit);
      }
      break;
  }
  // Return the result
  return result;
}


/*
 * These functions convert the specified type into a MemoryValue.
 * They do not need to be specialized (unless using enable_if). Instead,
 * they may simply be overloaded.
 */


/**
 * \brief Converts an unsigned integer into a MemoryValue
 * \tparam T The unsigned integer type (verified via \c std::is_integral and
 *           \c std::is_unsigned)
 * \param value The value to convert
 * \param bitsPerByte The number of bits in a byte
 * \param byteOrder The byte order (endianness)
 * \param byteCount If specified, pad the MemoryValue with zeroes, or truncate 
 *                  it to the specified size. Defaults to 0 (no effect).
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value && 
                        std::is_unsigned<T>::value, MemoryValue>::type
convert(T value, std::size_t bitsPerByte, Endianness byteOrder,
        std::size_t byteCount = 0) {
  // Grab the number of bits T can contain
  std::size_t bitsInType = std::numeric_limits<T>::digits;
  // First, split the value into the individual "bytes"
  std::vector<T> bytes;
  if (bitsInType > bitsPerByte && value >= (static_cast<T>(1) << bitsPerByte)) {
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
  if (byteOrder == Endianness::LITTLE) {
    std::reverse(bytes.begin(), bytes.end());
  }
  // Pad the bytes if necessary
  if (bytes.size() < byteCount) {
    switch (byteOrder) {
      case Endianness::BIG:
        bytes.insert(bytes.end(), byteCount - bytes.size(), 0);
        break;
      case Endianness::LITTLE:
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
    T byteMask;
    std::size_t leftShift = 8 - (bitsPerByte % 8);
    std::size_t rightShift = bitsPerByte - 8;
    if (bitsPerByte >= 8) {
      // Shift the 8 bit mask far enough to the left
      byteMask = static_cast<T>(0xFF) << (bitsPerByte - 8);
    } else {
      // The bitmask is smaller than the 8 bits the uint8_t offers.
      byteMask = (1 << bitsPerByte) - 1;
      // Set rightShift to 0 (because of the overflow in bitsPerByte - 8)
      rightShift = 0;
    }
    for (std::size_t offset = 0; offset < chunksPerByte; ++offset) {
      T chunk = byte & byteMask;
      if (offset == chunksPerByte - 1 && bitsPerByte % 8 != 0) {
        // Add padding
        chunk <<= leftShift;
      } else {
        // Shift the chunk back to the lowest 8 bits
        chunk >>= rightShift;
      }
      byteData.push_back(static_cast<uint8_t>(chunk));
      byteMask >>= 8;
      rightShift -= 8;
    }
    // Insert the data
    raw.insert(raw.end(), byteData.begin(), byteData.end());
  }
  // Create a memory value, and return it
  return MemoryValue(std::move(raw), bitsPerByte);
}


/**
 * \brief Converts a signed integer into a MemoryValue
 * \tparam T The signed integer type (verified via \c std::is_integral and
 *           \c std::is_signed)
 * \tparam U An unsigned integer type capable of holding the same number of bits
 *           as T (defaults to \c std::uintmax_t)
 * \param value The value to convert
 * \param bitsPerByte The number of bits in a byte
 * \param byteOrder The byte order (endianness)
 * \param representation Describes the encoding of the signed integer
 * \param byteCount If specified, pad the MemoryValue with zeroes, or truncate 
 *                  it to the specified size. Defaults to 0 (no effect).
 */
template <typename T, typename U = std::uintmax_t>
typename std::enable_if<std::is_integral<T>::value && 
                        std::is_signed<T>::value &&
                        std::is_integral<U>::value &&
                        std::is_unsigned<U>::value, MemoryValue>::type
convert(T value, std::size_t bitsPerByte, Endianness byteOrder,
        SignedRepresentation representation, std::size_t byteCount = 0) {
  // Get the number of bits T and U can contain
  std::size_t bitsInSignedType = std::numeric_limits<T>::digits;
  std::size_t bitsInUnsignedType = std::numeric_limits<U>::digits;
  // Positive values need no special treatment
  if (value >= 0) {
//std::cerr << static_cast<U>(value) << ", " << bitsPerByte << ", " << (int) byteOrder << ", " << (int) byteCount << std::endl;
    return convert(static_cast<U>(value), bitsPerByte, byteOrder, byteCount);
  } else {
    if (-value == value) {
      // This is two's complement, value is the minimum possible value.
      // Verify that the unsigned type can actually hold that value
      assert(bitsInUnsignedType > bitsInSignedType + 1);
    }
    // Get the number of bytes we need to represent this value, if no byteCount
    // is set
    if (byteCount == 0) {
      U positiveValue = static_cast<U>(-value);
      bool usedMostSignificantBit = false;
      while (positiveValue > 0) {
        usedMostSignificantBit = positiveValue & (static_cast<U>(1) << (bitsPerByte - 1));
        positiveValue >>= bitsPerByte;
        ++byteCount;
      }
      if (usedMostSignificantBit) {
        // We are using the most significant bit to encode this value, so we
        // need an additional bit (in the next byte)
        ++byteCount;
      }
    }
    // Convert the negative value to an appropriately encoded positive value
    U encoded;
    MemoryValue converted;
    switch (representation) {
      case SignedRepresentation::ONES_COMPLEMENT:
        // Flip all bits to get to the negative value from its magnitude
        encoded = ~static_cast<U>(-value);
        // Any bits except the last bitsInUnsignedType need to be set to 1
        //TODO
        break;
      case SignedRepresentation::SIGN_BIT:
        // Add a negative sign bit in front of the magnitude in postprocessing!
        encoded = static_cast<U>(-value);
        break;
      case SignedRepresentation::TWOS_COMPLEMENT:
        // Subtract one from the magnitude before flipping all bits
        encoded = ~static_cast<U>(-value - 1);
        break;
    }
    // Convert that positive value to a MemoryValue
    converted = convert(encoded, bitsPerByte, byteOrder, byteCount);
    // Do any necessary postprocessing
    switch (representation) {
      case SignedRepresentation::SIGN_BIT:
        // Set the most significant bit to 1
        {
          std::size_t mostSignificantBit = (byteOrder == Endianness::BIG)
            ? converted.getSize() - bitsPerByte
            : 0;
          converted.put(mostSignificantBit, true);
        }
        break;
      case SignedRepresentation::ONES_COMPLEMENT:
      case SignedRepresentation::TWOS_COMPLEMENT:
        // Set all the bits up to the most significant bit in the MemoryValue.
        // In particular, the highest bit that contains actual data already is a
        // one (via the bit flip from before the conversion to MemoryValue).
        // Therefore, just start at the MSB and work your way downwards
        {
          // The most significant bit
          std::size_t bit = (byteOrder == Endianness::BIG)
            ? converted.getSize() - bitsPerByte
            : 0;
          // Iterate until we hit a 1
          while (!converted.get(bit)) {
            converted.put(bit, true);
            ++bit;
            // On little endian MemoryValues, just loop until done
            // On big endian MemoryValues, wrap down every byte
            if (byteOrder == Endianness::BIG) {
              if (bit == bitsPerByte) {
                // Done writing
                break;
              }
              if (bit % bitsPerByte == 0) {
                // Wrap downwards to the next byte
                bit -= 2 * bitsPerByte;
              }
            } else {
              if (bit == converted.getSize()) {
                // Done writing
                break;
              }
            }
          }
        }
        break;
    }
    // Return the memory value
    return converted;
  }
}

#endif // ERAGPSIM_CORE_CONVERSIONS_HPP
