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

#include "arch/common/architecture-properties.hpp"
#include "common/assert.hpp"
#include "core/memory-value.hpp"

namespace conversions {

// function to detemine the sign of a MemoryValue
using SignFunction = std::function<bool(const MemoryValue&)>;
// function to convert an Integral Type to a MemoryValue
using ToMemoryValueFunction = std::function<MemoryValue(
    const std::vector<std::uint8_t>&, std::size_t, bool)>;
// function to convert a MemoryValue to an Integral Type
using ToIntegralFunction = std::function<MemoryValue(const MemoryValue&, bool)>;

/** Some standard conversions for just standard things */
namespace standardConversions {
namespace helper {
namespace nonsigned {
/** the standard sign function for unsigned conversions */
extern const conversions::SignFunction signum;
/** the standard toIntegral function for unsigned conversions */
extern const conversions::ToIntegralFunction toIntegralFunction;
/** the standard toMemoryValue function for unsigned conversions */
extern const conversions::ToMemoryValueFunction toMemoryValueFunction;
}
namespace signBit {
/** the standard sign function for signed conversions */
extern const conversions::SignFunction signum;
/** the standard toIntegral function for signed conversions using the SignBit
 * SignedRepresentation
 */
extern const conversions::ToIntegralFunction toIntegralFunction;
/** the standard toMemoryValue function for signed conversions using the SignBit
 * SignedRepresentation
 */
extern const conversions::ToMemoryValueFunction toMemoryValueFunction;
}
namespace onesComplement {
/** the standard sign function for signed conversions */
extern const conversions::SignFunction signum;
/** the standard toIntegral function for signed conversions using the
 * onesComplement SignedRepresentation
 */
extern const conversions::ToIntegralFunction toIntegralFunction;
/** the standard toMemoryValue function for signed conversions using the
 * onesComplement SignedRepresentation
 */
extern const conversions::ToMemoryValueFunction toMemoryValueFunction;
}
namespace twosComplement {
/** the standard sign function for signed conversions */
extern const conversions::SignFunction signum;
/** the standard toIntegral function for signed conversions using the
 * twosComplement SignedRepresentation
 */
extern const conversions::ToIntegralFunction toIntegralFunction;
/** the standard toMemoryValue function for signed conversions using the
 * twosComplement SignedRepresentation
 */
extern const conversions::ToMemoryValueFunction toMemoryValueFunction;
}
}
}
// wrapper containing a SignFunction, ToIntegralFunction and
// ToMemoryValueFunction
struct Conversion {
  SignFunction sgn;
  ToMemoryValueFunction toMem;
  ToIntegralFunction toInt;
  Conversion(const SignFunction& sgn,
             const ToIntegralFunction& toInt,
             const ToMemoryValueFunction& toMem)
  : sgn{sgn}, toMem{toMem}, toInt{toInt} {
  }
  Conversion()
  : Conversion(
        conversions::standardConversions::helper::nonsigned::signum,
        conversions::standardConversions::helper::nonsigned::toIntegralFunction,
        conversions::standardConversions::helper::nonsigned::
            toMemoryValueFunction) {
  }
  Conversion(const Conversion&) = default;
  Conversion(Conversion&&) = default;
  Conversion& operator=(const Conversion&) = default;
  Conversion& operator=(Conversion&&) = default;
  ~Conversion() = default;
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
        const SignFunction& sgn,
        const ToIntegralFunction& abs) {
  bool sign = sgn(memoryValue);// true => negative
  T result = conversions::detail::convertUnsigned<T>(abs(memoryValue, sign));
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
        const ToMemoryValueFunction& sgn,
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
enum class SignedRepresentation {
  UNSIGNED,
  ONES_COMPLEMENT,
  SIGN_BIT,
  TWOS_COMPLEMENT,
  SMART// Smart uses UNSIGNED/TWOS_COMPLEMENT depending whether the type is
       // signed or unsigned
};
/**
 * \brief maps SignedRepresentation -> conversion
 * \param representation the method of conversion
 * \returns a method of conversion
 */
Conversion switchConversion(SignedRepresentation representation);

/**
 * \brief maps ArchitectureProperties::SignedRepresentation ->
 * conversions::SignedRepresentation
 * \param representation some architecture SignedRepresentation
 * \returns some conversion SignedRepresentation
 */
SignedRepresentation
mapRepresentation(ArchitectureProperties::SignedRepresentation representation);
}

/**
 * \brief permutes a memoryValue
 * \param memoryValue The to be permuted MemoryValue
 * \param byteOrder enum determining the method of conversion
 * \param byteSize size of a Byte in bit
 * \returns permuted MemoryValue
 */
MemoryValue permute(const MemoryValue& memoryValue,
                    ArchitectureProperties::Endianness byteOrder,
                    std::size_t byteSize);

namespace detail {
/**
 * \brief Converts a MemoryValue into integral form, making sure to use the
 * given
 * SignedRepresentation
 * \param memoryValue The to be converted MemoryValue
 * \param representation the method of storing the value
 * \param byteSize soze of a byte in bit
 * \param byteOrder Endianess of the output
 * \tparam T The desired type of the output
 * \returns integral representation of memoryValue
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
convertForced(const MemoryValue& memoryValue,
              SignedRepresentation representation = SignedRepresentation::SMART,
              std::size_t byteSize = 8,
              ArchitectureProperties::Endianness byteOrder =
                  ArchitectureProperties::Endianness::LITTLE) {
  MemoryValue permuted = permute(memoryValue, byteOrder, byteSize);
  Conversion con = conversions::detail::switchConversion(representation);
  if (representation == SignedRepresentation::SMART &&
      std::is_signed<T>::value) {
    con = conversions::standardConversions::twosComplement;
  }
  return convert<T>(permuted, con);
}

/**
  * \brief Converts an integral value into a MemoryValue, making sure to use the
  * given SignedRepresentation
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
convertForced(T value,
              std::size_t size,
              SignedRepresentation representation = SignedRepresentation::SMART,
              std::size_t byteSize = 8,
              ArchitectureProperties::Endianness byteOrder =
                  ArchitectureProperties::Endianness::LITTLE) {
  Conversion con = conversions::detail::switchConversion(representation);
  if (representation == SignedRepresentation::SMART &&
      std::is_signed<T>::value) {
    con = conversions::standardConversions::twosComplement;
  }
  MemoryValue converted = convert<T>(value, con, size);
  return permute(converted, byteOrder, byteSize);
}
}

/**
 * \brief Converts a MemoryValue into integral form, using unsigned conversions
 * if T is unsigned
 * \param memoryValue The to be converted MemoryValue
 * \param representation the method of storing the value
 * \param byteSize soze of a byte in bit
 * \param byteOrder Endianess of the output
 * \tparam T The desired type of the output
 * \returns integral representation of memoryValue
 */
template <typename T>
typename std::
    enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, T>::type
    convert(const MemoryValue& memoryValue,
            std::size_t byteSize = 8,
            ArchitectureProperties::Endianness byteOrder =
                ArchitectureProperties::Endianness::LITTLE,
            ArchitectureProperties::SignedRepresentation representation =
                ArchitectureProperties::SignedRepresentation::TWOS_COMPLEMENT) {
  return detail::convertForced<T>(
      memoryValue, detail::SignedRepresentation::UNSIGNED, byteSize, byteOrder);
}
template <typename T>
typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value,
                        T>::type
convert(const MemoryValue& memoryValue,
        std::size_t byteSize = 8,
        ArchitectureProperties::Endianness byteOrder =
            ArchitectureProperties::Endianness::LITTLE,
        ArchitectureProperties::SignedRepresentation representation =
            ArchitectureProperties::SignedRepresentation::TWOS_COMPLEMENT) {
  return detail::convertForced<T>(memoryValue,
                                  detail::mapRepresentation(representation),
                                  byteSize,
                                  byteOrder);
}

/**
 * \brief Converts an integral value into a MemoryValue, using unsigned
 * conversions if T is unsigned
 * \param value The to be converted value
 * \param size number of bits to reserve for storing the value
 * \param representation the method of storing the value
 * \param byteSize size of a byte in bit
 * \param byteOrder Endianess of the output
 * \tparam T The type of the input
 * \returns MemoryValue representating value
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value &&
                            std::is_unsigned<T>::value,
                        MemoryValue>::type
convert(T value,
        std::size_t size,
        std::size_t byteSize = 8,
        ArchitectureProperties::Endianness byteOrder =
            ArchitectureProperties::Endianness::LITTLE,
        ArchitectureProperties::SignedRepresentation representation =
            ArchitectureProperties::SignedRepresentation::TWOS_COMPLEMENT) {
  return detail::convertForced<T>(
      value, size, detail::SignedRepresentation::UNSIGNED, byteSize, byteOrder);
}
template <typename T>
typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value,
                        MemoryValue>::type
convert(T value,
        std::size_t size,
        std::size_t byteSize = 8,
        ArchitectureProperties::Endianness byteOrder =
            ArchitectureProperties::Endianness::LITTLE,
        ArchitectureProperties::SignedRepresentation representation =
            ArchitectureProperties::SignedRepresentation::TWOS_COMPLEMENT) {
  return detail::convertForced<T>(value,
                                  size,
                                  detail::mapRepresentation(representation),
                                  byteSize,
                                  byteOrder);
}
}

#endif// ERAGPSIM_CORE_ADVANCED_CONVERSIONS_HPP_
