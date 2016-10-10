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

#include "common/assert.hpp"
#include "core/memory-value.hpp"

namespace conversions {

/**
* TODO: Replace with version from memory implementation
* Indicates the byte order of a memory value
*/
enum class Endianness { BIG, LITTLE };

/**
* TODO: Replace with version from architecture implementation
* Indicates how signed numbers are represented
*/
enum class SignedRepresentation {
  UNSIGNED,
  ONES_COMPLEMENT,
  SIGN_BIT,
  TWOS_COMPLEMENT,
  SMART// Smart uses UNSIGNED/TWOS_COMPLEMENT depending whether the type is
       // signed or unsigned
};

// function to detemine the sign of a MemoryValue
using signFunction = std::function<bool(const MemoryValue&)>;
// function to convert an Integral Type to a MemoryValue
using toMemoryValueFunction = std::function<MemoryValue(
    const std::vector<std::uint8_t>&, std::size_t, bool)>;
// function to convert a MemoryValue to an Integral Type
using toIntegralFunction = std::function<MemoryValue(const MemoryValue&, bool)>;

/** Some standard conversions for just standard things */
namespace standardConversions {
/** the standard sign function for unsigned conversions */
extern const signFunction unSignum0;
/** the standard sign function for signed conversions */
extern const signFunction Signum0;

/** the standard toIntegral function for unsigned conversions */
extern const toIntegralFunction nonsigned1;
/** the standard toIntegral function for signed conversions using the SignBit
 * SignedRepresentation
 */
extern const toIntegralFunction signBit1;
/** the standard toIntegral function for signed conversions using the
 * onesComplement SignedRepresentation
 */
extern const toIntegralFunction onesComplement1;
/** the standard toIntegral function for signed conversions using the
 * twosComplement SignedRepresentation
 */
extern const toIntegralFunction twosComplement1;

/** the standard toMemoryValue function for unsigned conversions */
extern const toMemoryValueFunction nonsigned2;
/** the standard toMemoryValue function for signed conversions using the SignBit
 * SignedRepresentation
 */
extern const toMemoryValueFunction signBit2;
/** the standard toMemoryValue function for signed conversions using the
 * onesComplement SignedRepresentation
 */
extern const toMemoryValueFunction onesComplement2;
/** the standard toMemoryValue function for signed conversions using the
 * twosComplement SignedRepresentation
 */
extern const toMemoryValueFunction twosComplement2;
}
// wrapper containing a signFunction, toIntegralFunction and
// toMemoryValueFunction
struct Conversion {
  signFunction sgn;
  toMemoryValueFunction toMem;
  toIntegralFunction toInt;
  Conversion(const signFunction& sgn,
             const toIntegralFunction& toInt,
             const toMemoryValueFunction& toMem)
  : sgn{sgn}, toMem{toMem}, toInt{toInt} {
  }
  Conversion()
  : Conversion(conversions::standardConversions::unSignum0,
               conversions::standardConversions::nonsigned1,
               conversions::standardConversions::nonsigned2) {
  }
  Conversion(const Conversion&) = default;
  Conversion(Conversion&&)      = default;
  Conversion& operator=(const Conversion&) = default;
  Conversion& operator=(Conversion&&) = default;
  ~Conversion()                       = default;
};

namespace standardConversions {
/** converts unsigned */
extern const Conversion nonsigned;
/** converts signed using the SignBit SignedRepresentation*/
extern const Conversion signBit;
/** converts signed using the onesComplement SignedRepresentation*/
extern const Conversion onesComplement;
/** converts signed using the twosComplement SignedRepresentation*/
extern const Conversion twosComplement;
}
namespace detail {
/**
 * \brief Converts an unsigned MemoryValue (little endian) into integral form
 * \param memoryValue The tobe converted MemoryValue
 * \tparam T The desired type of the output
 * \returns integral representation of memoryValue
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
convertUnsigned(const MemoryValue& memoryValue) {
  std::vector<std::uint8_t> raw{memoryValue.internal()};
  // TODO::use fancy bitmask stuff and remove dependency on getByteAt
  // const std::uint8_t mask{ 0xFF };
  // T result = raw.back()&mask;
  raw.pop_back();
  T result = getByteAt(memoryValue, raw.size() * 8);
  while (raw.size() > 0) {
    result <<= 4;
    result <<= 4;
    result |= raw.back();
    raw.pop_back();
  }
  return result;
}
}

/**
 * \brief Converts a MemoryValue (little endian) into integral form
 * \param memoryValue The to be converted MemoryValue
 * \param sgn a signFunction to determine the sign of the output
 * \param abs a toIntegralFunction converting a signed MemoryValue into it's
 * absolute value
 * \tparam T The desired type of the output
 * \returns integral representation of memoryValue
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
convert(const MemoryValue& memoryValue,
        const signFunction& sgn,
        const toIntegralFunction& abs) {
  bool sign = sgn(memoryValue);// true => negative
  T result  = conversions::detail::convertUnsigned<T>(abs(memoryValue, sign));
  if (sign ^ (result < 0)) return 0 - result;
  return result;
}

/**
 * \brief Converts a MemoryValue (little endian) into integral form
 * \param memoryValue The to be converted MemoryValue
 * \param con the method of storing the value
 * \tparam T The desired type of the output
 * \returns integral representation of memoryValue
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
convert(const MemoryValue& memoryValue, const Conversion& con) {
  return convert<T>(memoryValue, con.sgn, con.toInt);
}

/**
 * \brief permutes a memoryValue
 * \param memoryValue The to be permuted MemoryValue
 * \param byteSize size of a Byte in bit
 * \param permutation a permutation of the integers
 * [0;memoryValue.getSize()/byteSize[ mapping resulting index -> origin index
 * \returns permuted MemoryValue
 */
MemoryValue permute(const MemoryValue& memoryValue,
                    const std::size_t byteSize,
                    const std::vector<std::size_t> permutation);

/**
 * \brief permutes a memoryValue
 * \param memoryValue The to be permuted MemoryValue
 * \param byteSize size of a Byte in bit
 * \param permutation a permutation of the integers
 * [0;memoryValue.getSize()/byteSize[ mapping resulting index -> origin index
 * \returns permuted MemoryValue
 */
MemoryValue permute(const MemoryValue& memoryValue,
                    const std::size_t byteSize,
                    const std::function<std::size_t(std::size_t)> permutation);

/**
 * \brief Converts an integral value into a MemoryValue (little endian)
 * \param value The to be converted value
 * \param sgn function storing the sign
 * \param size number of bits to reserve for storing the value
 * \tparam T The type of the input
 * \returns MemoryValue representating value
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value, MemoryValue>::type
convert(const T value,
        const toMemoryValueFunction& sgn,
        const std::size_t size) {
  // T abs{ std::min(value) };
  T abs{value};
  if (abs < 0) abs = 0 - abs;
  std::size_t sizeInByte{(size + 7) / 8};
  std::vector<std::uint8_t> raw{};
  for (std::size_t i = 0; i < sizeInByte; ++i) {
    raw.push_back(static_cast<std::uint8_t>(abs & 0xFF));
    abs >>= 4;
    abs >>= 4;
  }
  return sgn(raw, size, value < 0);
}

/**
 * \brief Converts an integral value into a MemoryValue (little endian)
 * \param value The to be converted value
 * \param con the method of storing the value
 * \param size number of bits to reserve for storing the value
 * \tparam T The type of the input
 * \returns MemoryValue representating value
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value, MemoryValue>::type
convert(T value, const Conversion con, std::size_t size) {
  return convert<T>(value, con.toMem, size);
}

namespace detail {
// signFunction
bool unSignumA(const MemoryValue& memoryValue);
bool signumA(const MemoryValue& memoryValue);

// toIntegralFunction
MemoryValue nonsignedB(const MemoryValue& memoryValue, bool sign);
MemoryValue signBitB(const MemoryValue& memoryValue, bool sign);
MemoryValue onesComplementB(const MemoryValue& memoryValue, bool sign);
MemoryValue twosComplementB(const MemoryValue& memoryValue, bool sign);

// toMemoryValueFunction
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
}

namespace {
// brief function used as BigEndian permutation
struct bigEndian {
  std::size_t _byteCount;
  bigEndian(std::size_t byteCount) : _byteCount{byteCount} {
  }
  bigEndian() : bigEndian(8){};
  bigEndian(const bigEndian&) = default;
  bigEndian& operator=(const bigEndian&) = default;
  bigEndian(bigEndian&&)                 = default;
  bigEndian& operator=(bigEndian&&) = default;
  std::size_t operator()(size_t x) {
    return _byteCount - x;
  }
};
}

namespace detail {
/**
 * \brief maps SignedRepresentation -> conversion
 * \param representation the method of conversion
 * \returns a methid of conversion
 */
Conversion switchConversion(SignedRepresentation representation);
}

/**
 * \brief permutes a memoryValue
 * \param memoryValue The to be permuted MemoryValue
 * \param byteOrder enum determining the method of conversion
 * \param byteSize size of a Byte in bit
 * \returns permuted MemoryValue
 */
MemoryValue permute(const MemoryValue& memoryValue,
                    Endianness byteOrder,
                    std::size_t byteSize);

/**
 * \brief Converts a MemoryValue into integral form
 * \param memoryValue The to be converted MemoryValue
 * \param representation the method of storing the value
 * \param byteSize soze of a byte in bit
 * \param byteOrder Endianess of the output
 * \tparam T The desired type of the output
 * \returns integral representation of memoryValue
 */

template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
convert(const MemoryValue& memoryValue,
        SignedRepresentation representation = SignedRepresentation::SMART,
        std::size_t byteSize                = 8,
        Endianness byteOrder                = Endianness::LITTLE) {
  MemoryValue permuted = permute(memoryValue, byteOrder, byteSize);
  Conversion con       = conversions::detail::switchConversion(representation);
  if (representation == SignedRepresentation::SMART &&
      std::is_signed<T>::value) {
    con = conversions::standardConversions::twosComplement;
  }
  return convert<T>(permuted, con);
}

/**
 * \brief Converts an integral value into a MemoryValue
 * \param value The to be converted value
 * \param size number of bits to reserve for storing the value
 * \param representation the method of storing the value
 * \param byteSize size of a byte in bit
 * \param byteOrder Endianess of the output
 * \tparam T The type of the input
 * \returns MemoryValue representating value
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value, MemoryValue>::type
convert(T value,
        std::size_t size,
        SignedRepresentation representation = SignedRepresentation::SMART,
        std::size_t byteSize                = 8,
        Endianness byteOrder                = Endianness::LITTLE) {
  Conversion con = conversions::detail::switchConversion(representation);
  if (representation == SignedRepresentation::SMART &&
      std::is_signed<T>::value) {
    con = conversions::standardConversions::twosComplement;
  }
  MemoryValue converted = convert<T>(value, con, size);
  return permute(converted, byteOrder, byteSize);
}
}

#endif// ERAGPSIM_CORE_ADVANCED_CONVERSIONS_HPP_
