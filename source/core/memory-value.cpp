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

#include <iostream>
#include <sstream>
#include <vector>

#include "common/assert.hpp"
#include "core/memory-value.hpp"

namespace {

std::ostream &tohex(std::ostream &stream, std::uint8_t value) {
  static const char hex[] = "0123456789ABCDEF";
  return stream << hex[value / 16] << hex[value % 16] << ';';
}

auto byteAddressOfBit(MemoryValue::address_t address) {
  return address / 8;
}

auto offset(MemoryValue::address_t address) {
  return address % 8;
}
}


MemoryValue::Reference::Reference(MemoryValue &memory, address_t address)
: _memory(memory), _address(address) {
}

MemoryValue::Reference &MemoryValue::Reference::operator=(bool value) {
  _memory.set(_address, value);
  return *this;
}

MemoryValue::Reference::operator bool() const noexcept {
  return _memory.get(_address);
}

MemoryValue::MemoryValue() : MemoryValue(8) {
}

MemoryValue::MemoryValue(const Underlying &other, address_t size)
: _size{size}, _data{other} {
  assert::that(size > 0);
  assert::that(_data.size() == (size + 7) / 8);
}

MemoryValue::MemoryValue(Underlying &&other, address_t size)
: _size{size}, _data{std::move(other)} {
  assert::that(size > 0);
  assert::that(_data.size() == (size + 7) / 8);
}

MemoryValue::MemoryValue(size_t size)
: _size{size}, _data(address_t{(size + 7) / 8}, 0) {
  assert::that(size > 0);
}

MemoryValue::MemoryValue(const MemoryValue &other,
                         address_t begin,
                         address_t end)
: _size{end - begin}, _data(address_t{(end - begin + 7) / 8}, 0) {
  assert::that(begin < end);
  assert::that(end <= other.getSize());
  for (address_t i = 0; i < _data.size(); ++i) {
    _data[i] = other.getByteAt(begin + i * 8);
  }
}

MemoryValue MemoryValue::subSet(address_t begin, address_t end) const {
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

MemoryValue::Iterator MemoryValue::begin() noexcept {
  return {*this, 0};
}

MemoryValue::ConstIterator MemoryValue::cbegin() const noexcept {
  return {*this, 0};
}

MemoryValue::ConstIterator MemoryValue::begin() const noexcept {
  return {*this, 0};
}

MemoryValue::Iterator MemoryValue::end() noexcept {
  return {*this, _size};
}

MemoryValue::ConstIterator MemoryValue::cend() const noexcept {
  return {*this, _size};
}

MemoryValue::ConstIterator MemoryValue::end() const noexcept {
  return {*this, _size};
}

MemoryValue::Iterator MemoryValue::getIterator(address_t address) {
  assert::that(address <= _size);
  return {*this, address};
}

MemoryValue::ConstIterator MemoryValue::getIterator(address_t address) const {
  assert::that(address <= _size);
  return {*this, address};
}

bool MemoryValue::get(address_t address) const {
  assert::that(address < getSize());
  assert::that(address >= 0);
  return (_data[byteAddressOfBit(address)] & testOr[offset(address)]) != 0;
}

void MemoryValue::put(address_t address, bool value) {
  assert::that(address < getSize());
  assert::that(address >= 0);
  // Setting or clearing the value bitwise.
  if (value) {
    _data[byteAddressOfBit(address)] |= testOr[offset(address)];
  } else {
    _data[byteAddressOfBit(address)] &= testAnd[offset(address)];
  }
}

bool MemoryValue::set(address_t address, bool value) {
  assert::that(address < getSize());
  assert::that(address >= 0);
  if (((_data[byteAddressOfBit(address)] & testOr[offset(address)]) != 0) ==
      value) {
    return value;
  } else {
    return flip(address);
  }
}

bool MemoryValue::flip(address_t address) {
  assert::that(address < getSize());
  assert::that(address >= 0);
  _data[byteAddressOfBit(address)] ^= testOr[offset(address)];
  return !get(address);
}

MemoryValue::Reference MemoryValue::operator[](address_t address) {
  return {*this, address};
}

MemoryValue::ConstReference MemoryValue::operator[](address_t address) const {
  return get(address);
}

MemoryValue::address_t MemoryValue::getSize() const {
  return _size;
}

bool MemoryValue::isZero() {
  for (address_t i = 0; i < getSize(); ++i) {
    if (get(i)) return false;
  }
  return true;
}

bool MemoryValue::front() const {
  return get(0);
}

bool MemoryValue::back() const {
  return get(_size - 1);
}

const MemoryValue::Underlying &MemoryValue::internal() const {
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

  for (address_t i = 0; i < _data.size() - 1; ++i) {
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


void MemoryValue::write(const MemoryValue &other, address_t begin) {
  address_t end{begin + other._size};
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
    for (address_t i = (begin / 8) + 1; i < (end - 1) / 8; ++i) {
      _data[i] = 0x00;
    }
  }
  // fill first byte
  _data[begin / 8] |= other.getByteAt(0) << (begin % 8);
  address_t filled{8 - (begin % 8)};
  while (filled < other._size) {
    _data[(begin + filled) / 8] |= other.getByteAt(filled);
    filled += 8;
  }
}

void MemoryValue::clear() {
  for (auto &i : _data) {
    i = 0x00;
  }
}

std::uint8_t MemoryValue::getByteAt(address_t address) const {
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
  for (size_t i = value.getSize(); i > 0; --i) {
    // if (i % value._byteSize == 0 && i != value.getSize()) stream << '|';
    if (value.get(i - intMinPrecision))
      stream << '1';
    else
      stream << '0';
  }

  stream << "' : " << value._size << " -> [";

  // Printing as hex.
  for (size_t i = 0; i < value._data.size(); ++i) {
    tohex(stream, value._data[i]);
  }

  return stream << "]";
}

std::string MemoryValue::toHexString(bool Ox, bool leadingZeros) const {
  static const char hex[] = "0123456789ABCDEF";
  std::stringstream stream{};
  if (Ox) {
    stream << "0x";
  }
  bool zero = true;
  // reversely iterate over all the bytes in value (high->low)
  for (std::size_t l = (getSize() + 7) / 8; l-- > 0;) {
    std::uint8_t byte = getByteAt(l * 8);
    std::uint8_t upperChar = byte / 16;
    std::uint8_t lowerChar = byte % 16;
    // do not print if this is the first character && it's 0
    if (leadingZeros || !zero || upperChar != 0) {
      stream.put(hex[upperChar]);
      zero = false;
    }
    // do not print if this is the first character && it's 0
    if (leadingZeros || !zero || lowerChar != 0) {
      stream.put(hex[lowerChar]);
      zero = false;
    }
  }
  // if no characters have been printed print 0
  if (zero) {
    stream.put('0');
  }
  return stream.str();
}
