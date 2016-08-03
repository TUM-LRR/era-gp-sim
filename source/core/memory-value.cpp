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

#include "core/memory-value.hpp"

static std::size_t constexpr sizeOfBytesStored(const std::size_t byteSize) {
  // We round up to the next byte border.
  return (byteSize + 7) / 8;
}

static std::size_t constexpr byteAddress(const std::size_t address,
                                         const std::size_t byteSize) {
  // Address of first index + address in Byte = real address
  return (address / byteSize) * sizeOfBytesStored(byteSize) +
         (address % byteSize) / 8;
}

static std::size_t constexpr offset(const std::size_t address,
                                    const std::size_t byteSize) {
  // Offset in byte.
  return (address % byteSize) % 8;
}

static constexpr char hex[16]{'0',
                              '1',
                              '2',
                              '3',
                              '4',
                              '5',
                              '6',
                              '7',
                              '8',
                              '9',
                              'A',
                              'B',
                              'C',
                              'D',
                              'E',
                              'F'};

static std::ostream &tohex(std::ostream &stream, std::uint8_t v) {
  return stream << hex[(int)v / 16] << hex[(int)v % 16] << ';';
}


MemoryValue::MemoryValue() : MemoryValue(1u, 8u) {
}
MemoryValue::MemoryValue(const std::vector<uint8_t> &other,
                         const std::size_t byteSize)
: _byteSize(byteSize), _data(other) {
}
MemoryValue::MemoryValue(std::vector<uint8_t> &&other,
                         const std::size_t byteSize)
: _byteSize(byteSize), _data(std::move(other)) {
}
MemoryValue::MemoryValue(size_t byteAmount, std::size_t byteSize)
: _byteSize(byteSize)
, _data(std::size_t{byteAmount * sizeOfBytesStored(byteSize)}, 0) {
}


MemoryValue::MemoryValue(const MemoryValue &other,
                         const std::size_t begin,
                         const std::size_t end,
                         const std::size_t byteSize)
: _byteSize(byteSize)
, _data(std::size_t((end - begin + byteSize - 1) / byteSize *
                    sizeOfBytesStored(byteSize)),
        0) {
  assert(begin <= end);
  assert(end <= other.getSize());
  std::size_t sizeOfBytes = sizeOfBytesStored(_byteSize);
  std::size_t byteAmount  = _data.size() / sizeOfBytes;
  std::size_t pos         = 0;

  // Iteratively copying byte for byte.
  for (std::size_t i = 0; i < byteAmount; ++i) {
    for (std::size_t j = 0; j < sizeOfBytes; ++j) {
      _data[i * sizeOfBytes + j] = other.getByteAt(begin + pos + 8 * j);
    }
    pos += _byteSize;
  }
}

MemoryValue MemoryValue::subSet(const std::size_t begin,
                                const std::size_t end,
                                const std::size_t byteSize) const {
  return MemoryValue(*this, begin, end, byteSize);
}

bool MemoryValue::get(const std::size_t address) const {
  assert(address < getSize());
  assert(address >= 0);
  return _data[byteAddress(address, _byteSize)] &
         (1 << offset(address, _byteSize));
}

void MemoryValue::put(const std::size_t address, const bool value) {
  assert(address < getSize());
  assert(address >= 0);

  // Setting or clearing the value bitwise.
  if (value)
    _data[byteAddress(address, _byteSize)] |= (1 << offset(address, _byteSize));
  else
    _data[byteAddress(address, _byteSize)] &=
        ~(1 << offset(address, _byteSize));
}

bool MemoryValue::set(const std::size_t address, const bool value) {
  auto prev = get(address);
  put(address, value);
  return prev;
}

std::size_t MemoryValue::getByteSize() const {
  return _byteSize;
}

std::size_t MemoryValue::getByteAmount() const {
  return _data.size() / sizeOfBytesStored(_byteSize);
}

std::size_t MemoryValue::getSize() const {
  return _byteSize * getByteAmount();
}

const std::vector<uint8_t> &MemoryValue::internal() const {
  return _data;
}

bool MemoryValue::operator==(const MemoryValue &other) const {
  // If the sizes or byte sizes differ, we (dunno why) decide, that the memory
  // values are not equal, even if they contain the same value.
  if (_byteSize != other._byteSize || _data.size() != other._data.size()) {
    return false;
  }

  // If the _byteSize is aligned to the bytes, we can just compare the arrays.
  if (_byteSize % 8 == 0) {
    return _data == other._data;
  }

  // The size of a complete byte.
  const std::size_t byteStoredSize = sizeOfBytesStored(_byteSize);

  // To compare the rest of the two bytes.
  const std::size_t shift = _byteSize % 8;

  for (std::size_t i = 0; i < _data.size(); ++i) {
    if (i % byteStoredSize != byteStoredSize - 1) {
      // Not the last byte, we compare the two.
      if (_data[i] != other._data[i]) {
        return false;
      }
    } else {
      // The last byte, we just compare the rest of the byte.
      int testEq = ((1 << shift) - 1);
      if ((_data[i] & testEq) != (other._data[i] & testEq)) {
        return false;
      }
    }
  }
  return true;
}

bool MemoryValue::operator!=(const MemoryValue &other) const {
  // Using the == operator.
  return !((*this) == other);
}

std::uint8_t MemoryValue::getByteAt(std::size_t address) const {
  assert(address < getSize());
  assert(address >= 0);
  std::size_t pos = byteAddress(address, _byteSize);

  // Filling our byte with the available rest coming after it.
  std::uint8_t result =
      static_cast<std::uint8_t>(_data[pos] >> offset(address, _byteSize));
  std::size_t filled = std::min((_byteSize - (address % _byteSize)),
                                8 - offset(address, _byteSize));
  ++pos;

  // We look into the byte(s) which follow.
  while (filled < 8 && pos < _data.size()) {
    result &= (1 << filled) - 1;
    result |= _data[pos++] << (filled);
    filled += _byteSize - ((address + filled) % _byteSize);
  }

  return result;
}

std::ostream &operator<<(std::ostream &stream, const MemoryValue &value) {
  stream << '\'';
  constexpr std::size_t intMinPrecision = 1;

  // Printing in binary format.
  for (std::size_t i = value.getSize(); i > 0; --i) {
    if (i % value._byteSize == 0 && i != value.getSize()) stream << '|';
    if (value.get(i - intMinPrecision))
      stream << '1';
    else
      stream << '0';
  }

  stream << "' : " << value._byteSize << " -> [";

  // Printing as hex.
  for (std::size_t i = 0; i < value._data.size(); ++i)
    tohex(stream, value._data[i]);
  return stream << "]";
}
