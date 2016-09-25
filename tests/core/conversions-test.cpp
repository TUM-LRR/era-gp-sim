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

#include <algorithm>
#include <limits>
#include <random>
#include <tuple>

// Include Google Test first, do not reorder includes
// clang-format off
#include "gtest/gtest.h"
#include "core/conversions.hpp"
#include "core/memory-value.hpp"
// clang-format on

using namespace core;

namespace {

constexpr std::size_t REPETITIONS       = 1000;// Repetitions per test case
constexpr std::size_t SMALL_REPETITIONS = 50;  // ... for expensive test cases
constexpr std::size_t MAXIMUM_BYTE_SIZE = 64;// Maximum number of bits per byte
constexpr std::size_t MAXIMUM_BITS      = 64;// Maximum number of bits total

static_assert(MAXIMUM_BYTE_SIZE <= MAXIMUM_BITS,
              "Maximum bit count per byte exceeds total maximum bit count");

constexpr bool kUseSeed     = true;// Whether to use the seed below in the tests
constexpr std::size_t kSeed = 0xDEADBEEF;

// Utility functions

/**
 * \brief Manually performs a conversion from MemoryValue to uint64_t only.
 * \param data The data inside the memory value
 * \param bitsPerByte The number of bits per byte
 */
std::pair<uint64_t, uint64_t>
getIntegers(const std::vector<uint8_t>& data, std::size_t bitsPerByte) {
  // Convert to integers, little and big endian
  uint64_t littleEndian = 0;
  uint64_t bigEndian    = 0;

  std::size_t internalItemsPerByte = (bitsPerByte + 7) / 8;
  std::size_t bytes                = data.size() / internalItemsPerByte;

  for (std::size_t byte = 0; byte < bytes; ++byte) {
    uint64_t thisByteValue = 0;
    for (std::size_t item = 0; item < internalItemsPerByte; ++item) {
      thisByteValue <<= 8;
      thisByteValue |= data[byte * internalItemsPerByte + item];
      if (item == internalItemsPerByte - 1 && bitsPerByte % 8 != 0) {
        // Mask out padding
        thisByteValue >>= 8 - (bitsPerByte % 8);
      }
    }
    littleEndian <<= bitsPerByte;
    littleEndian |= thisByteValue;
    bigEndian |= (thisByteValue << (bitsPerByte * byte));
  }
  return std::pair<uint64_t, uint64_t>(bigEndian, littleEndian);
}

/**
 * \brief Interprets a uint64_t as a int64_t using various representation modes
 * \param value The input value
 * \param type The representation the signed value encoded in the input uses.
 * \param bits The number of bits used in the value
 */
int64_t toSigned(uint64_t value, SignedRepresentation type, std::size_t bits) {
  uint64_t firstBit = 1ULL << (bits - 1);
  uint64_t mask     = (firstBit - 1) | firstBit;
  if (value & firstBit) {
    switch (type) {
      case SignedRepresentation::ONES_COMPLEMENT:
        // Flip all bits, cast, and negate
        return -static_cast<int64_t>((~value) & mask);
      case SignedRepresentation::SIGN_BIT:
        // Clear the sign bit, cast, and negate
        return -static_cast<int64_t>((value & (~firstBit)) & mask);
      case SignedRepresentation::TWOS_COMPLEMENT:
        // Flip all bits, cast, negate and subtract 1
        return -static_cast<int64_t>((~value) & mask) - 1;
    }
  } else {
    return static_cast<int64_t>(value);
  }
}

/**
 * \brief Generates a random MemoryValue, along with its converted values
 * \param bytes The number of "bytes" (of the specified size)
 * \param bitsPerByte The size of a byte, in bits
 * \param engine The random number generator to be used
 * \param forceSetFirstBit Whether to forcibly set the first bit in the data
 * \returns A pair containing the conversions (itself a pair of the number
 *          interpreted as big and little endian numbers), and the memory value
 *          itself.
 */
template <typename RandomEngine>
std::pair<std::pair<uint64_t, uint64_t>, MemoryValue>
generateMemoryValue(std::size_t bytes,
                    std::size_t bitsPerByte,
                    RandomEngine& engine,
                    bool forceSetFirstBit = false) {
  // Calculate size of data vector
  std::size_t internalItemsPerByte = (bitsPerByte + 7) / 8;
  std::size_t internalItemCount    = internalItemsPerByte * bytes;

  // Create random byte generator
  std::uniform_int_distribution<> generator(0, 255);

  auto random = [ generator = std::move(generator), &engine ]() mutable {
    return static_cast<uint8_t>(generator(engine));
  };

  // Fill internal data vector
  std::vector<uint8_t> internalData;
  std::generate_n(std::back_inserter(internalData), internalItemCount, random);

  if (forceSetFirstBit) internalData[0] = true;

  // Mask padding bits to zero, if padding is present at all
  if (bitsPerByte % 8 != 0) {
    for (std::size_t item = internalItemsPerByte - 1; item < internalItemCount;
         item += internalItemsPerByte) {
      uint8_t maskWidth = 8 - (bitsPerByte % 8);
      uint8_t mask      = ~((1U << maskWidth) - 1U);// Also inverts the mask (!)
      internalData[item] &= mask;
    }
  }

  // Get integers in big and little endian
  auto integers = getIntegers(internalData, bitsPerByte);

  return std::pair<std::pair<uint64_t, uint64_t>, MemoryValue>(
      integers, MemoryValue(std::move(internalData), bitsPerByte));
}
}

////////////////////////////////////////////////////////////////////////////////
////                      UNSIGNED INTEGER CONVERSIONS                      ////
////////////////////////////////////////////////////////////////////////////////

// Standard conversions - test the result of core::convert(...) against the
// expected
// result value

TEST(TestUnsignedConversions, memoryValueToNumber) {
  std::mt19937_64 engine{kUseSeed ? kSeed : std::random_device{}()};
  for (std::size_t iteration = 0; iteration < REPETITIONS; ++iteration) {
    for (std::size_t byteSize = 1; byteSize < MAXIMUM_BYTE_SIZE; ++byteSize) {
      for (std::size_t bytes = 1; bytes * byteSize < MAXIMUM_BITS; ++bytes) {
        // Fill MemoryValue with random data
        auto generated        = generateMemoryValue(bytes, byteSize, engine);
        auto integers         = generated.first;
        MemoryValue random    = generated.second;
        uint64_t bigEndian    = integers.first;
        uint64_t littleEndian = integers.second;

        // Convert as big and little endian
        uint64_t valueBE = convert<uint64_t>(random, Endianness::BIG);
        uint64_t valueLE = convert<uint64_t>(random, Endianness::LITTLE);

        // Verify conversions
        ASSERT_EQ(bigEndian, valueBE);
        ASSERT_EQ(littleEndian, valueLE);
      }
    }
  }
}

TEST(TestUnsignedConversions, numberToMemoryValue) {
  std::mt19937_64 engine{kUseSeed ? kSeed : std::random_device{}()};
  for (std::size_t iteration = 0; iteration < REPETITIONS; ++iteration) {
    for (std::size_t byteSize = 1; byteSize < MAXIMUM_BYTE_SIZE; ++byteSize) {
      // Get random number
      uint64_t random = engine();

      // Convert to memory values
      MemoryValue valueBE = core::convert(random, byteSize, Endianness::BIG);
      MemoryValue valueLE = core::convert(random, byteSize, Endianness::LITTLE);

      // Verify results
      ASSERT_EQ(getIntegers(valueBE.internal(), byteSize).first, random);
      ASSERT_EQ(getIntegers(valueLE.internal(), byteSize).second, random);
    }
  }
}

// "There And Back Again" - verify that the result of
// core::convert(core::convert(...)) is
// equal to the input

TEST(TestUnsignedConversions, bidirectionalViaNumber) {
  std::mt19937_64 engine{kUseSeed ? kSeed : std::random_device{}()};
  for (std::size_t iteration = 0; iteration < REPETITIONS; ++iteration) {
    for (std::size_t byteSize = 1; byteSize < MAXIMUM_BYTE_SIZE; ++byteSize) {
      for (std::size_t bytes = 1; bytes * byteSize < MAXIMUM_BITS; ++bytes) {
        // Fill MemoryValue with random data
        auto generated     = generateMemoryValue(bytes, byteSize, engine);
        MemoryValue random = generated.second;

        // Convert to numbers
        uint64_t valueBE = convert<uint64_t>(random, Endianness::BIG);
        uint64_t valueLE = convert<uint64_t>(random, Endianness::LITTLE);

        // Convert back to MemoryValues
        MemoryValue resultBE =
            core::convert(valueBE, byteSize, Endianness::BIG, bytes);
        MemoryValue resultLE =
            core::convert(valueLE, byteSize, Endianness::LITTLE, bytes);

        // Compare the internal representation of the MemoryValues
        ASSERT_EQ(random, resultBE);
        ASSERT_EQ(random, resultLE);
      }
    }
  }
}

TEST(TestUnsignedConversions, bidirectionalViaMemoryValue) {
  std::mt19937_64 engine{kUseSeed ? kSeed : std::random_device{}()};
  for (std::size_t iteration = 0; iteration < REPETITIONS; ++iteration) {
    for (std::size_t byteSize = 1; byteSize < MAXIMUM_BYTE_SIZE; ++byteSize) {
      // Generate random number
      uint64_t random = engine();

      // Convert to memory values
      MemoryValue valueBE = core::convert(random, byteSize, Endianness::BIG);
      MemoryValue valueLE = core::convert(random, byteSize, Endianness::LITTLE);

      // Convert memory values back to numbers
      uint64_t resultBE = convert<uint64_t>(valueBE, Endianness::BIG);
      uint64_t resultLE = convert<uint64_t>(valueLE, Endianness::LITTLE);

      // Verify conversions
      ASSERT_EQ(random, resultBE);
      ASSERT_EQ(random, resultLE);
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
////                       SIGNED INTEGER CONVERSIONS                       ////
////////////////////////////////////////////////////////////////////////////////

// Standard conversions - test the result of core::convert(...) against the
// expected
// result value

TEST(TestSignedConversions, memoryValueToNumber) {
  std::mt19937_64 engine{kUseSeed ? kSeed : std::random_device{}()};
  for (std::size_t iteration = 0; iteration < REPETITIONS; ++iteration) {
    for (auto representation : {SignedRepresentation::ONES_COMPLEMENT,
                                SignedRepresentation::SIGN_BIT,
                                SignedRepresentation::TWOS_COMPLEMENT}) {
      for (std::size_t byteSize = 1; byteSize < MAXIMUM_BYTE_SIZE; ++byteSize) {
        for (std::size_t bytes = 1; bytes * byteSize < MAXIMUM_BITS; ++bytes) {
          // Fill MemoryValue with random data, setting the first bit
          auto generated = generateMemoryValue(bytes, byteSize, engine, true);
          auto integers  = generated.first;
          MemoryValue random    = generated.second;
          uint64_t bigEndian    = integers.first;
          uint64_t littleEndian = integers.second;

          // Convert as big and little endian
          int64_t valueBE =
              convert<int64_t>(random, Endianness::BIG, representation);
          int64_t valueLE =
              convert<int64_t>(random, Endianness::LITTLE, representation);

          // Verify conversions
          ASSERT_EQ(toSigned(bigEndian, representation, bytes * byteSize),
                    valueBE);
          ASSERT_EQ(toSigned(littleEndian, representation, bytes * byteSize),
                    valueLE);
        }
      }
    }
  }
}

TEST(TestSignedConversions, numberToMemoryValue) {
  std::mt19937_64 engine{kUseSeed ? kSeed : std::random_device{}()};
  for (std::size_t iteration = 0; iteration < SMALL_REPETITIONS; ++iteration) {
    for (auto representation : {SignedRepresentation::ONES_COMPLEMENT,
                                SignedRepresentation::SIGN_BIT,
                                SignedRepresentation::TWOS_COMPLEMENT}) {
      for (std::size_t byteSize = 1; byteSize < MAXIMUM_BYTE_SIZE; ++byteSize) {
        for (std::size_t width = 1; width < MAXIMUM_BITS; ++width) {
          // Get random number
          uint64_t randomUnsigned = engine();
          // Set width'th least significant bit
          randomUnsigned &= (1ULL << width) - 1;
          randomUnsigned |= 1ULL << (width - 1);

          // Calculate number of bytes used
          uint64_t clone        = randomUnsigned;
          std::size_t bytesUsed = 0;
          while (clone > 0) {
            clone >>= byteSize;
            ++bytesUsed;
          }

          // Convert to signed
          int64_t random =
              toSigned(randomUnsigned, representation, bytesUsed * byteSize);

          // Convert to memory values
          MemoryValue valueBE =
              core::convert(random, byteSize, Endianness::BIG, representation);
          MemoryValue valueLE = core::convert(
              random, byteSize, Endianness::LITTLE, representation);

          // Manually convert to numbers
          uint64_t compareBE = getIntegers(valueBE.internal(), byteSize).first;
          uint64_t compareLE = getIntegers(valueLE.internal(), byteSize).second;

          int64_t big = toSigned(compareBE, representation, valueBE.getSize());
          int64_t little =
              toSigned(compareLE, representation, valueLE.getSize());

          // Verify results
          ASSERT_EQ(big, random);
          ASSERT_EQ(little, random);
        }
      }
    }
  }
}

// "There And Back Again" - verify that the result of
// core::convert(core::convert(...)) is
// equal to the input

TEST(TestSignedConversions, bidirectionalViaNumber) {
  std::mt19937_64 engine{kUseSeed ? kSeed : std::random_device{}()};
  for (std::size_t iteration = 0; iteration < SMALL_REPETITIONS; ++iteration) {
    for (auto representation : {SignedRepresentation::ONES_COMPLEMENT,
                                SignedRepresentation::SIGN_BIT,
                                SignedRepresentation::TWOS_COMPLEMENT}) {
      for (std::size_t byteSize = 1; byteSize < MAXIMUM_BYTE_SIZE; ++byteSize) {
        for (std::size_t bytes = 1; bytes * byteSize < MAXIMUM_BITS; ++bytes) {
          // Fill MemoryValue with random data, setting the first bit
          auto generated = generateMemoryValue(bytes, byteSize, engine, true);
          MemoryValue random = generated.second;

          // Convert as big and little endian
          int64_t valueBE =
              convert<int64_t>(random, Endianness::BIG, representation);
          int64_t valueLE =
              convert<int64_t>(random, Endianness::LITTLE, representation);

          // Convert back to MemoryValues
          MemoryValue resultBE = core::convert(
              valueBE, byteSize, Endianness::BIG, representation, bytes);
          MemoryValue resultLE = core::convert(
              valueLE, byteSize, Endianness::LITTLE, representation, bytes);

          // Compare the MemoryValues
          // We manually treat individual test instances here if the integer
          // value is zero and the SIGN_BIT or ONES_COMPLEMENT representations
          // are used because +0 and -0 do not compare as equal.
          if (valueBE != 0 ||
              representation == SignedRepresentation::TWOS_COMPLEMENT) {
            // Simply compare the two MemoryValues
            ASSERT_EQ(random, resultBE);
          } else {
            // Determine the most significant bit (= sign bit), it will not be
            // compared
            std::size_t mostSignificant = (bytes - 1) * byteSize;
            for (std::size_t bit = 0; bit < bytes * byteSize; ++bit) {
              if (bit == mostSignificant) continue;
              ASSERT_FALSE(resultBE.get(bit));
            }
          }

          if (valueLE != 0 ||
              representation == SignedRepresentation::TWOS_COMPLEMENT) {
            // Simply compare the two MemoryValues
            ASSERT_EQ(random, resultLE);
          } else {
            // In little-endian MemoryValues, the most significant bit is at
            // index 0, so the loop begins at 1
            for (std::size_t bit = 1; bit < bytes * byteSize; ++bit) {
              ASSERT_FALSE(resultLE.get(bit));
            }
          }
        }
      }
    }
  }
}

TEST(TestSignedConversions, bidirectionalViaMemoryValue) {
  std::mt19937_64 engine{kUseSeed ? kSeed : std::random_device{}()};
  for (std::size_t iteration = 0; iteration < SMALL_REPETITIONS; ++iteration) {
    for (auto representation : {SignedRepresentation::ONES_COMPLEMENT,
                                SignedRepresentation::SIGN_BIT,
                                SignedRepresentation::TWOS_COMPLEMENT}) {
      for (std::size_t byteSize = 1; byteSize < MAXIMUM_BYTE_SIZE; ++byteSize) {
        for (std::size_t width = 1; width < MAXIMUM_BITS; ++width) {
          // Get random number
          uint64_t randomUnsigned = engine();
          // Set width'th least significant bit
          randomUnsigned &= (1ULL << width) - 1;
          randomUnsigned |= 1ULL << (width - 1);

          // Calculate number of bytes used
          uint64_t clone        = randomUnsigned;
          std::size_t bytesUsed = 0;
          while (clone > 0) {
            clone >>= byteSize;
            ++bytesUsed;
          }

          // Convert to signed
          int64_t random =
              toSigned(randomUnsigned, representation, bytesUsed * byteSize);

          // Convert to memory values
          MemoryValue valueBE =
              core::convert(random, byteSize, Endianness::BIG, representation);
          MemoryValue valueLE = core::convert(
              random, byteSize, Endianness::LITTLE, representation);

          // Convert back to numbers
          int64_t resultBE =
              convert<int64_t>(valueBE, Endianness::BIG, representation);
          int64_t resultLE =
              convert<int64_t>(valueLE, Endianness::LITTLE, representation);

          // Verify results
          ASSERT_EQ(random, resultBE);
          ASSERT_EQ(random, resultLE);
        }
      }
    }
  }
}
