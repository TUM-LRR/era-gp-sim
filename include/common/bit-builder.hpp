/*
* C++ Assembler Interpreter
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

#ifndef ERAGPSIM_COMMON_BIT_BUILDER_HPP
#define ERAGPSIM_COMMON_BIT_BUILDER_HPP

#include <cstddef>
#include <limits>

#include "common/assert.hpp"

/**
 * A class to construct bitfields piecewise.
 *
 * With *bitfield*, any type that can be used to store bits is meant.
 * Most often, this will be an (unsigned) integral type, liked `std::uint32_t`.
 *
 * Example usage of the BitBuilder could look like so:
 *
 * \code{.cpp}
 * auto bits = BitBuilder<std::uint8_t>()
 *                .addBit(x, 1)      // add bit 1 of x
 *                .addRange(y, 3, 5) // add bits 3..5 of y
 *                .addUpTo(z, 4)     // add bits 0...4 of z
 *                .build();
 * \endcode
 *
 */
template <typename T>
class BitBuilder {
 public:
  using index_t = std::size_t;
  using size_t = std::size_t;

  /**
   * Constructs a new, empty builder/
   *
   * \param initial The initial value to set the builder to.
   */
  explicit BitBuilder(const T& initial = T()) : _carrier(initial), _size(0) {
  }

  /**
   * Adds the bit at the given index to the builder.
   *
   * \param value The value whose bit to add.
   * \param index The index of the bit of the value to add.
   * \return The `BitBuilder`.
   */
  template <typename U>
  BitBuilder& addBit(const U& value, index_t index) {
    assert::that(index < std::numeric_limits<U>::digits);
    assert::that(_size < _capacity);

    const bool bit = value & (U(1) << index);
    if (bit) {
      _carrier |= T(1) << _size;
    }

    ++_size;

    return *this;
  }

  /**
   * Adds the range of bits in the given bounds to the builder.
   *
   * \param value The value whose bits to add.
   * \param firstBit The least-significant bit of the value to add.
   * \param lastBit The most-significant bit of the value to add (inclusive!).
   * \return The `BitBuilder`.
   */
  template <typename U>
  BitBuilder& addRange(const U& value, index_t firstBit, index_t lastBit) {
    const auto width = lastBit - firstBit + 1;

    assert::that(firstBit < std::numeric_limits<U>::digits);
    assert::that(lastBit < std::numeric_limits<U>::digits);
    assert::that(_size + width <= _capacity);

    // Create a mask of `width` many bits
    const U mask = (U(1) << width) - 1;

    // Grab the relevant portion (range) of the target value
    const auto inputRange = (value & (mask << firstBit)) >> firstBit;

    _carrier |= inputRange << _size;

    _size += width;

    return *this;
  }

  /**
   * Adds the the range of bits of the given value up to the given index.
   *
   * \param value The value whose bits to add.
   * \param lastBit The (inclusive!) boundary of the range of bits to add.
   * \return The `BitBuilder`.
   */
  template <typename U>
  BitBuilder& addUpTo(const U& value, index_t lastBit) {
    return addRange(value, 0, lastBit);
  }

  /**
   * Adds the the range of bits of the given value starting at the given index.
   *
   * \param value The value whose bits to add.
   * \param firstBit The least-significant bit of the range of bits to add.
   * \return The `BitBuilder`.
   */
  template <typename U>
  BitBuilder& addStartingAt(const U& value, index_t firstBit) {
    return addRange(value, firstBit, std::numeric_limits<U>::digits - 1);
  }

  /**
   * Adds all the bits of the given value to the builder.
   *
   * \param value The value whose bits to add.
   * \return The `BitBuilder`.
   */
  template <typename U>
  BitBuilder& add(const U& value) {
    return addStartingAt(value, 0);
  }

  /**
   * \return The current value of the builder.
   */
  const T& build() const noexcept {
    return _carrier;
  }

  /**
   * Resets the builder's internal state.
   */
  void clear() noexcept {
    _carrier = 0;
    _size = 0;
  }

  /**
   * \return The number of bits added to the builder so far.
   */
  size_t size() const noexcept {
    return _size;
  }

  /**
   * \return The maximum  number of bits that can be added to the builder.
   */
  size_t capacity() const noexcept {
    return _capacity;
  }

  /**
   * \return True if no bits have been added to the builder so far, else false.
   */
  bool isEmpty() const noexcept {
    return _size == 0;
  }

  /**
   * \return True if the builder's size equals its capacity, else false.
   */
  bool isFull() const noexcept {
    return _size == _capacity;
  }

 private:
  static constexpr size_t _capacity = std::numeric_limits<T>::digits;

  /** The underlying value used to store bits. */
  T _carrier;

  /** The current number of bits added to the builder. */
  size_t _size;
};

#endif /* ERAGPSIM_COMMON_BIT_BUILDER_HPP */
