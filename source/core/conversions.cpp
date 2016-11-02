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

#include "core/conversions.hpp"

namespace conversions {
namespace {
constexpr uint8_t testEQ[8]{
    0xFF, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F,
};
}

MemoryValue permute(const MemoryValue& memoryValue,
                    const std::size_t byteSize,
                    const std::vector<std::size_t> permutation) {
  assert::that(byteSize > 0);
  assert::that(memoryValue.getSize() % byteSize ==
               0);// The value has to be made out of whole bytes
  const std::size_t byteAmount{memoryValue.getSize() / byteSize};
  assert::that(permutation.size() >=
               byteAmount);// We need at least byteAmount values to permute

  // permute the bytes
  std::vector<std::uint8_t> permuted{};
  std::uint8_t mask{testEQ[byteSize % 8]};
  for (std::size_t i = 0; i < byteAmount; ++i) {
    for (std::size_t j = 0; j < byteSize - 1; j += 8) {
      if (j + 8 < byteSize) {
        permuted.push_back(
            getByteAt(memoryValue, permutation[i] * byteSize + j));
      } else {
        permuted.push_back(
            getByteAt(memoryValue, permutation[i] * byteSize + j) & mask);
      }
    }
  }

  // pull together
  if (byteSize % 8 == 0) {
    // this is so much easier
    return MemoryValue{permuted, permuted.size() * 8};
  }

  // Size of A Virtual Byte in byte
  const std::size_t sizeOfByte{(byteSize + 7) / 8};
  // The raw result vector
  std::vector<std::uint8_t> raw{};
  // The number of bits in the buffer
  std::size_t bitsInBuffer{0};
  // the Buffer
  std::uint8_t bufferByte{0};

  // for each byte
  for (std::size_t i = 0; i < byteAmount; ++i) {
    for (std::size_t j = 0; j < sizeOfByte; ++j) {
      // take byte from permuted vector and put into buffer
      bufferByte |= permuted[i * sizeOfByte + j] << bitsInBuffer;
      // Add size of this byte to bitsInBuffer
      if (j + 1 < sizeOfByte) {
        // inner byte => full byte
        bitsInBuffer += 8;
      } else {
        // last byte in virtual byte
        bitsInBuffer += byteSize % 8;
      }
      // Buffer Filled
      if (bitsInBuffer >= 8) {
        // push onto result vector
        raw.push_back(bufferByte);
        // the part of the current byte that didn'tfit into the Buffer become
        // the new Buffer
        bufferByte = permuted[i * sizeOfByte + j] >>
                     (16 - bitsInBuffer);//(8 - (bIB % 8))
        bitsInBuffer -= 8;
      }
    }
  }
  // push final Byte
  raw.push_back(bufferByte);
  return MemoryValue(raw, memoryValue.getSize());
}

MemoryValue permute(const MemoryValue& memoryValue,
                    const std::size_t byteSize,
                    const std::function<std::size_t(std::size_t)> permutation) {
  assert::that(byteSize > 0);
  assert::that(memoryValue.getSize() % byteSize ==
               0);// The value has to be made out of whole bytes
  std::vector<std::size_t> permutationVector{};
  std::size_t byteAmount{memoryValue.getSize() / byteSize};
  for (std::size_t i = 0; i < byteAmount; ++i) {
    permutationVector.push_back(permutation(i));
  }
  return permute(memoryValue, byteSize, permutationVector);
}

namespace {
bool unSignumA(const MemoryValue& memoryValue) {
  return false;
}

bool signumA(const MemoryValue& memoryValue) {
  return memoryValue.get(memoryValue.getSize() - 1);
}

MemoryValue nonsignedB(const MemoryValue& memoryValue, bool sign) {
  return memoryValue;
}

MemoryValue signBitB(const MemoryValue& memoryValue, bool sign) {
  assert::that(memoryValue.getSize() > 1);
  return memoryValue.subSet(0, memoryValue.getSize() - 1);
}

MemoryValue onesComplementB(const MemoryValue& memoryValue, bool sign) {
  if (sign) {
    // invert all bits
    assert::that(memoryValue.getSize() > 1);
    std::vector<std::uint8_t> raw{memoryValue.internal()};
    // cut last byte if necessary
    if (memoryValue.getSize() % 8 == 1) {
      raw.pop_back();
    }
    for (std::size_t i = 0; i < raw.size(); ++i) {
      const std::uint8_t x = raw[i];
      raw[i] ^= 0xFF;
    }
    return MemoryValue{std::move(raw), memoryValue.getSize() - 1};
  } else {
    return signBitB(memoryValue, sign);
  }
}

MemoryValue twosComplementB(const MemoryValue& memoryValue, bool sign) {
  if (sign) {
    // invert
    MemoryValue result{onesComplementB(memoryValue, sign)};
    // add one
    for (std::size_t i = 0; i < result.getSize(); ++i) {
      if (!result.flip(i)) return result;
    }
    // overflow
    MemoryValue inc{result.getSize() + 1};
    inc.put(result.getSize(), true);
    return inc;
  } else {
    return signBitB(memoryValue, sign);
  }
}

MemoryValue nonsignedC(const std::vector<std::uint8_t>& value,
                       const std::size_t size,
                       const bool sign) {
  assert::that(!sign);// don't convert signed values via nonsigned
  return MemoryValue{value, size};
}

MemoryValue signBitC(const std::vector<std::uint8_t>& value,
                     const std::size_t size,
                     const bool sign) {
  MemoryValue result{value, size};
  // set the sign bit
  result.set(size - 1, sign);
  return result;
}

MemoryValue onesComplementC(const std::vector<std::uint8_t>& value,
                            const std::size_t size,
                            const bool sign) {
  std::vector<std::uint8_t> raw{value};
  // invert all bits
  if (sign) {
    for (std::size_t i = 0; i < raw.size(); ++i) {
      raw[i] ^= 0xFF;
    }
  }
  MemoryValue result{std::move(raw), size};
  // set the sign bit
  result.set(size - 1, sign);
  return result;
}

MemoryValue twosComplementC(const std::vector<std::uint8_t>& value,
                            const std::size_t size,
                            const bool sign) {
  // invert
  MemoryValue result{onesComplementC(value, size, sign)};
  if (sign) {// add one
    for (std::size_t i = 0; i < result.getSize() - 1; ++i) {
      if (!result.flip(i)) return result;
    }
  }
  return result;
}
}

namespace standardConversions {
namespace helper {
namespace nonsigned {
/** the standard sign function for unsigned conversions */
const conversions::SignFunction signum{unSignumA};
/** the standard toIntegral function for unsigned conversions */
const conversions::ToIntegralFunction toIntegralFunction{nonsignedB};
/** the standard toMemoryValue function for unsigned conversions */
const conversions::ToMemoryValueFunction toMemoryValueFunction{nonsignedC};
}
namespace signBit {
/** the standard sign function for signed conversions */
const conversions::SignFunction signum{signumA};
/** the standard toIntegral function for signed conversions using the SignBit
* SignedRepresentation
*/
const conversions::ToIntegralFunction toIntegralFunction{signBitB};
/** the standard toMemoryValue function for signed conversions using the SignBit
* SignedRepresentation
*/
const conversions::ToMemoryValueFunction toMemoryValueFunction{signBitC};
}
namespace onesComplement {
/** the standard sign function for signed conversions */
const conversions::SignFunction signum{signumA};
/** the standard toIntegral function for signed conversions using the
* onesComplement SignedRepresentation
*/
const conversions::ToIntegralFunction toIntegralFunction{onesComplementB};
/** the standard toMemoryValue function for signed conversions using the
* onesComplement SignedRepresentation
*/
const conversions::ToMemoryValueFunction toMemoryValueFunction{onesComplementC};
}
namespace twosComplement {
/** the standard sign function for signed conversions */
const conversions::SignFunction signum{signumA};
/** the standard toIntegral function for signed conversions using the
* twosComplement SignedRepresentation
*/
const conversions::ToIntegralFunction toIntegralFunction{twosComplementB};
/** the standard toMemoryValue function for signed conversions using the
* twosComplement SignedRepresentation
*/
const conversions::ToMemoryValueFunction toMemoryValueFunction{twosComplementC};
}
}
}
namespace standardConversions {
const Conversion nonsigned{
    conversions::standardConversions::helper::nonsigned::signum,
    conversions::standardConversions::helper::nonsigned::toIntegralFunction,
    conversions::standardConversions::helper::nonsigned::toMemoryValueFunction};
const Conversion signBit{
    conversions::standardConversions::helper::signBit::signum,
    conversions::standardConversions::helper::signBit::toIntegralFunction,
    conversions::standardConversions::helper::signBit::toMemoryValueFunction};
const Conversion onesComplement{
    conversions::standardConversions::helper::onesComplement::signum,
    conversions::standardConversions::helper::onesComplement::
        toIntegralFunction,
    conversions::standardConversions::helper::onesComplement::
        toMemoryValueFunction};
const Conversion twosComplement{
    conversions::standardConversions::helper::twosComplement::signum,
    conversions::standardConversions::helper::twosComplement::
        toIntegralFunction,
    conversions::standardConversions::helper::twosComplement::
        toMemoryValueFunction};
}
namespace detail {
Conversion switchConversion(SignedRepresentation representation) {
  switch (representation) {
    case SignedRepresentation::UNSIGNED:
      return conversions::standardConversions::nonsigned;
    case SignedRepresentation::SIGN_BIT:
      return conversions::standardConversions::signBit;
    case SignedRepresentation::ONES_COMPLEMENT:
      return conversions::standardConversions::onesComplement;
    case SignedRepresentation::TWOS_COMPLEMENT:
      return conversions::standardConversions::twosComplement;
    case SignedRepresentation::SMART:
      return conversions::standardConversions::nonsigned;
    default: assert::that(false);
  }
  return conversions::standardConversions::nonsigned;
}
SignedRepresentation
mapRepresentation(ArchitectureProperties::SignedRepresentation representation) {
  switch (representation) {
    case ArchitectureProperties::SignedRepresentation::SIGN_BIT:
      return SignedRepresentation::SIGN_BIT;
    case ArchitectureProperties::SignedRepresentation::ONES_COMPLEMENT:
      return SignedRepresentation::ONES_COMPLEMENT;
    case ArchitectureProperties::SignedRepresentation::TWOS_COMPLEMENT:
      return SignedRepresentation::TWOS_COMPLEMENT;
    default: assert::that(false);
  }
  return SignedRepresentation::SMART;
}
}

MemoryValue permute(const MemoryValue& memoryValue,
                    ArchitectureProperties::Endianness byteOrder,
                    std::size_t byteSize) {
  const std::size_t byteCount{memoryValue.getSize() / byteSize};
  std::function<std::size_t(std::size_t)> lambdaFunction;
  switch (byteOrder) {
    case ArchitectureProperties::Endianness::LITTLE:
      return memoryValue;
    case ArchitectureProperties::Endianness::BIG:
      lambdaFunction = [byteCount](std::size_t x) { return byteCount - x; };
      return permute(memoryValue, byteSize, lambdaFunction);
    default: assert::that(false);
  }
  return MemoryValue{};
}
}
