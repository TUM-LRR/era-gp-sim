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
namespace {
std::size_t constexpr SizeOfByte(const std::size_t byteSize) {
  return (byteSize + 7) / 8;
}
}

MemoryValue::MemoryValue() : MemoryValue(1u, 8u) {
}
MemoryValue::MemoryValue(const std::vector<uint8_t> &other,
                         const std::size_t byteSize)
: _byteSize{byteSize}, _data{other} {
}
MemoryValue::MemoryValue(std::vector<uint8_t> &&other,
                         const std::size_t byteSize)
: _byteSize{byteSize}, _data{std::move(other)} {
}
MemoryValue::MemoryValue(size_t byteAmount, std::size_t byteSize)
: _byteSize{byteSize}
, _data(std::size_t{byteAmount * SizeOfByte(byteSize)}, 0) {
}


MemoryValue::MemoryValue(const MemoryValue &other,
                         const std::size_t begin,
                         const std::size_t end,
                         const std::size_t byteSize)
: _byteSize{byteSize}
, _data(std::size_t{(end - begin + byteSize - 1) / byteSize *
                    SizeOfByte(byteSize)},
        0) {
  assert(begin <= end);
  assert(end <= other.getSize());
  std::size_t sizeOfByte{SizeOfByte(_byteSize)};
  std::size_t byteAmount{_data.size() / sizeOfByte};
  std::size_t pos{0};
  for (std::size_t i = 0; i < byteAmount; ++i) {
    for (std::size_t j = 0; j < sizeOfByte; ++j) {
      _data[i * sizeOfByte + j] = other.getByteAt(begin + pos + 8 * j);
    }
    pos += _byteSize;
  }
}

MemoryValue MemoryValue::subSet(const std::size_t begin,
                                const std::size_t end,
                                const std::size_t byteSize) const {
  return MemoryValue(*this, begin, end, byteSize);
}

namespace {
constexpr uint8_t testOr[8]{
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
};
constexpr uint8_t testAnd[8]{
    0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F,
};
}

bool MemoryValue::get(const std::size_t address) const {
  assert(address < getSize());
  return ((_data[(address / _byteSize) * SizeOfByte(_byteSize) +
                 (address % _byteSize) / 8]) &
          testOr[(address % _byteSize) % 8]) != 0;
}

void MemoryValue::put(const std::size_t address, const bool value) {
  assert(address < getSize());
  std::size_t byteAddress{(address / _byteSize) * SizeOfByte(_byteSize) +
                          (address % _byteSize) / 8};
  std::size_t offset{(address % _byteSize) % 8};
  if (value)
    _data[byteAddress] |= testOr[offset];
  else
    _data[byteAddress] &= testAnd[offset];
}

bool MemoryValue::set(const std::size_t address, const bool value) {
  assert(address < getSize());
  std::size_t byteAddress{(address / _byteSize) * SizeOfByte(_byteSize) +
                          (address % _byteSize) / 8};
  std::size_t offset{(address % _byteSize) % 8};
  return (((_data[byteAddress]) & testOr[offset]) != 0) == value
             ? value
             : (!value & (true | (_data[byteAddress] ^= testOr[offset])));
}

std::size_t MemoryValue::getByteSize() const {
  return _byteSize;
}

std::size_t MemoryValue::getByteAmount() const {
  return _data.size() / SizeOfByte(_byteSize);
}

std::size_t MemoryValue::getSize() const {
  return _byteSize * getByteAmount();
}

const std::vector<uint8_t> &MemoryValue::internal() const {
  return _data;
}

namespace {
constexpr uint8_t testEQ[8]{
    0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F,
};
}

bool MemoryValue::operator==(const MemoryValue &other) const {
  if (_byteSize != other._byteSize || _data.size() != other._data.size())
    return false;
  if (_byteSize % 8 == 0) return _data == other._data;
  const std::size_t byteByteSize{_byteSize / 8 + 1};
  const std::size_t shift{_byteSize % 8};
  for (std::size_t i = 0; i < _data.size(); ++i) {
    if (i % byteByteSize != byteByteSize - 1) {
      if (_data[i] != other._data[i]) return false;
    } else {
      if ((_data[i] & testEQ[shift]) != (other._data[i] & testEQ[shift]))
        return false;
    }
  }
  return true;
}

bool MemoryValue::operator!=(const MemoryValue &other) const {
  return !((*this) == other);
}

std::uint8_t MemoryValue::getByteAt(std::size_t address) const {
  assert(address < getSize());
  std::size_t pos{(address / _byteSize) * SizeOfByte(_byteSize) +
                  (address % _byteSize) / 8};
  std::uint8_t result{
      static_cast<std::uint8_t>(_data[pos++] >> ((address % _byteSize) % 8))};
  std::size_t filled{std::min((_byteSize - (address % _byteSize)),
                              8 - ((address % _byteSize) % 8))};
  while (filled < 8 && pos < _data.size()) {
    result &= testEQ[filled];
    result |= _data[pos++] << (filled);
    filled += _byteSize - ((address + filled) % _byteSize);
  }
  return result;
}

namespace {
constexpr char hex[16]{
    '0',
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
    'F',
};
std::ostream &tohex(std::ostream &stream, std::uint8_t v) {
  return stream << hex[(int)v / 16] << hex[(int)v % 16] << ';';
}
}

std::ostream &operator<<(std::ostream &stream, const MemoryValue &value) {
  stream << '\'';
  constexpr std::size_t intMinPrecision = 1;
  for (std::size_t i = value.getSize(); i > 0; --i) {
    if (i % value._byteSize == 0 && i != value.getSize()) stream << '|';
    if (value.get(i - intMinPrecision))
      stream << '1';
    else
      stream << '0';
  }
  stream << "' : " << value._byteSize << " -> [";
  for (std::size_t i = 0; i < value._data.size(); ++i)
    tohex(stream, value._data[i]);
  return stream << "]";
}
