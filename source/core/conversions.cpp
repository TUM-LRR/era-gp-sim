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

namespace {
constexpr uint8_t testEQ[8]{
    0xFF, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F,
};
}

MemoryValue permute(const MemoryValue& memoryValue,
                    const std::size_t byteSize,
                    const std::vector<std::size_t> permutation) {
  assert(byteSize > 0);
  assert(memoryValue.getSize() % byteSize ==
         0);// The value has to be made out of whole bytes
  const std::size_t byteAmount{memoryValue.getSize() / byteSize};
  assert(permutation.size() >=
         byteAmount);// We need at least byteAmount values to permute

  // permute the bytes
  std::vector<std::uint8_t> permuted{};
  std::uint8_t mask{testEQ[byteSize % 8]};
  for (std::size_t i = 0; i < byteAmount; ++i) {
    for (std::size_t j = 0; j < byteSize - 1; j += 8) {
      if (j + 8 < byteSize) {
        permuted.push_back(
            memoryValue.getByteAt(permutation[i] * byteSize + j));
      } else {
        permuted.push_back(
            memoryValue.getByteAt(permutation[i] * byteSize + j) & mask);
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
  assert(byteSize > 0);
  assert(memoryValue.getSize() % byteSize ==
         0);// The value has to be made out of whole bytes
  std::vector<std::size_t> permutationVector{};
  std::size_t byteAmount{memoryValue.getSize() / byteSize};
  for (std::size_t i = 0; i < byteAmount; ++i) {
    permutationVector.push_back(permutation(i));
  }
  return permute(memoryValue, byteSize, permutationVector);
}

bool unSignum(const MemoryValue& memoryValue) {
  return false;
}

bool signum(const MemoryValue& memoryValue) {
  return memoryValue.get(memoryValue.getSize() - 1);
}

MemoryValue nonsigned(const MemoryValue& memoryValue) {
  return memoryValue;
}

MemoryValue signBit(const MemoryValue& memoryValue) {
  assert(memoryValue.getSize() > 1);
  return memoryValue.subSet(0, memoryValue.getSize() - 1);
}

MemoryValue onesComplement(const MemoryValue& memoryValue) {
  assert(memoryValue.getSize() > 1);
  std::vector<std::uint8_t> raw{memoryValue.internal()};
  // cut last byte if necessary
  if (memoryValue.getSize() % 8 == 1) {
    raw.pop_back();
  }
  // invert all bits
  for (std::size_t i = 0; i < raw.size(); ++i) {
    raw[i] ^= 0xFF;
  }
  return MemoryValue{raw, memoryValue.getSize() - 1};
}

MemoryValue twosComplement(const MemoryValue& memoryValue) {
  // invert
  MemoryValue result{onesComplement(memoryValue)};
  // add one
  for (std::size_t i = 0; i < result.getSize() - 1; ++i) {
    if (!result.flip(i)) return result;
  }
  return result;
}

MemoryValue
nonsigned(const std::vector<std::uint8_t>& value, std::size_t size, bool sign) {
  assert(!sign);// don't convert signed values via nonsigned
  return MemoryValue{value, size};
}

MemoryValue
signBit(const std::vector<std::uint8_t>& value, std::size_t size, bool sign) {
  MemoryValue result{value, size};
  // set the sign bit
  result.set(size - 1, sign);
  return result;
}

MemoryValue onesComplement(const std::vector<std::uint8_t>& value,
                           std::size_t size,
                           bool sign) {
  std::vector<std::uint8_t> raw{value};
  // invert all bits
  for (std::size_t i = 0; i < raw.size(); ++i) {
    raw[i] ^= 0xFF;
  }
  MemoryValue result{std::move(raw), size};
  // set the sign bit
  result.set(size - 1, sign);
  return result;
}

MemoryValue twosComplement(const std::vector<std::uint8_t>& value,
                           std::size_t size,
                           bool sign) {
  // invert
  MemoryValue result{onesComplement(value, size, sign)};
  // subtract one
  for (std::size_t i = 0; i < result.getSize() - 1; ++i) {
    if (result.flip(i)) return result;
  }
  return result;
}
