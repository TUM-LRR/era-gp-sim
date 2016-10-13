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
constexpr char hex[16]{'0',
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

std::ostream &tohex(std::ostream &stream, std::uint8_t v) {
  return stream << hex[(int)v / 16] << hex[(int)v % 16] << ';';
}

std::size_t byteAddressOfBit(std::size_t address) {
  return address / 8;
}

std::size_t offset(std::size_t address) {
  return address % 8;
}
}


MemoryValue::MemoryValue() : MemoryValue(8) {
}

MemoryValue::MemoryValue(const std::vector<uint8_t> &other,
                         const std::size_t size)
: _size{size}, _data{other} {
  assert::that(true);
  assert::that(size > 0);
  assert::that(_data.size() == (size + 7) / 8);
}

MemoryValue::MemoryValue(std::vector<uint8_t> &&other, const std::size_t size)
: _size{size}, _data{std::move(other)} {
  assert::that(size > 0);
  assert::that(_data.size() == (size + 7) / 8);
}

MemoryValue::MemoryValue(std::size_t size)
: _size{size}, _data(std::size_t{(size + 7) / 8}, 0) {
  assert::that(size > 0);
}

MemoryValue::MemoryValue(const MemoryValue &other,
                         const std::size_t begin,
                         const std::size_t end)
: _size{end - begin}, _data(std::size_t{(end - begin + 7) / 8}, 0) {
  assert::that(begin < end);
  assert::that(end <= other.getSize());
  for (std::size_t i = 0; i < _data.size(); ++i) {
    _data[i] = other.getByteAt(begin + i * 8);
  }
}

MemoryValue
MemoryValue::subSet(const std::size_t begin, const std::size_t end) const {
  return MemoryValue(*this, begin, end);
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
  assert::that(address < getSize());
  assert::that(address >= 0);
  return (_data[byteAddressOfBit(address)] & testOr[offset(address)]) != 0;
}

void MemoryValue::put(const std::size_t address, const bool value) {
  assert::that(address < getSize());
  assert::that(address >= 0);
  // Setting or clearing the value bitwise.
  if (value) {
    _data[byteAddressOfBit(address)] |= testOr[offset(address)];
  } else {
    _data[byteAddressOfBit(address)] &= testAnd[offset(address)];
  }
}

bool MemoryValue::set(const std::size_t address, const bool value) {
  assert::that(address < getSize());
  assert::that(address >= 0);
  if (((_data[byteAddressOfBit(address)] & testOr[offset(address)]) != 0) ==
      value) {
    return value;
  } else {
    return flip(address);
  }
}

bool MemoryValue::flip(const std::size_t address) {
  assert::that(address < getSize());
  assert::that(address >= 0);
  _data[byteAddressOfBit(address)] ^= testOr[offset(address)];
  return !get(address);
}

std::size_t MemoryValue::getSize() const {
  return _size;
}

const std::vector<uint8_t> &MemoryValue::internal() const {
  return _data;
}

namespace {
constexpr uint8_t testEQ[8]{
    0xFF, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F,
};
}

bool MemoryValue::operator==(const MemoryValue &other) const {
  // If the sizes differ the MemoryValues are cannot be equal
  if (_size != other._size) {
    return false;
  }

  for (std::size_t i = 0; i < _data.size() - 1; ++i) {
    if (_data[i] != other._data[i]) return false;
  }
  return (_data[_data.size() - 1] & testEQ[offset(_size)]) ==
         (other._data[_data.size() - 1] & testEQ[offset(_size)]);
}

bool MemoryValue::operator!=(const MemoryValue &other) const {
  // Using the == operator.
  return !((*this) == other);
}

namespace {
constexpr uint8_t write0[8]{
    0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F,
};
constexpr uint8_t write1[8]{
    0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00,
};
}


void MemoryValue::write(const MemoryValue &other, std::size_t begin) {
  std::size_t end{begin + other._size};
  assert::that(begin < end);// _size > 0
  assert::that(end <= _size);
  // wipe clean
  if (begin / 8 == (end - 1) / 8) {// begin & end are same byte
    _data[begin / 8] &= write0[begin % 8] | write1[(end - 1) % 8];
  } else {
    // clear first byte
    _data[begin / 8] &= write0[begin % 8];
    // clear last byte
    _data[(end - 1) / 8] &= write1[(end - 1) % 8];
    // clear in between
    for (std::size_t i = (begin / 8) + 1; i < (end - 1) / 8; ++i) {
      _data[i] = 0x00;
    }
  }
  // fill first byte
  _data[begin / 8] |= other.getByteAt(0) << (begin % 8);
  std::size_t filled{8 - (begin % 8)};
  while (filled < other._size) {
    _data[(begin + filled) / 8] |= other.getByteAt(filled);
    filled += 8;
  }
}

std::uint8_t MemoryValue::getByteAt(std::size_t address) const {
  assert::that(address < getSize());
  assert::that(address >= 0);

  // first half byte
  std::uint8_t result{static_cast<std::uint8_t>(
      _data[byteAddressOfBit(address)] >> offset(address))};
  // if first byte was not last byte, second half byte
  if (byteAddressOfBit(address) + 1 < _data.size()) {
    result |= static_cast<std::uint8_t>(_data[byteAddressOfBit(address) + 1]
                                        << (8 - offset(address)));
  }
  // cut if out of size
  if (getSize() - address < 8) {
    result &= testEQ[getSize() - address];
  }
  return result;
}

std::ostream &operator<<(std::ostream &stream, const MemoryValue &value) {
  stream << '\'';
  constexpr std::size_t intMinPrecision = 1;

  // Printing in binary format.
  for (std::size_t i = value.getSize(); i > 0; --i) {
    // if (i % value._byteSize == 0 && i != value.getSize()) stream << '|';
    if (value.get(i - intMinPrecision))
      stream << '1';
    else
      stream << '0';
  }

  stream << "' : " << value._size << " -> [";

  // Printing as hex.
  for (std::size_t i = 0; i < value._data.size(); ++i)
    tohex(stream, value._data[i]);
  return stream << "]";
}
