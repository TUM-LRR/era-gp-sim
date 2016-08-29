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

#ifndef ERAGPSIM_CORE_MEMORYVALUE_HPP_
#define ERAGPSIM_CORE_MEMORYVALUE_HPP_

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

class MemoryValue {
 public:
  /**
   * \brief Constructs an empty MemoryValue of default length, 1 byte á 8 bit
   */
  MemoryValue();
  /**
   * \brief Constructs an MemoryValue that acquires the data of other
   * \param other The MemoryValue to acquire the data of
   */
  MemoryValue(MemoryValue &&other) = default;
  /**
   * \brief Constructs an MemoryValue that acquires the data of other
   * \param other The MemoryValue to acquire the data of
   */
  MemoryValue &operator=(MemoryValue &&other) = default;
  /**
   * \brief Constructs an MemoryValue with a copy of the data of other
   * \param other The MemoryValue to copy the data of
   */
  MemoryValue(const MemoryValue &other) = default;
  /**
   * \brief Constructs an MemoryValue with a copy of the data of other
   * \param other The MemoryValue to copy the data of
   */
  MemoryValue &operator=(const MemoryValue &other) = default;
  /**
   * \brief Destroys the MemoryValue and invalidates also the references used
   *        for conversion
   */
  ~MemoryValue() = default;
  /**
   * \brief Constructs a MemoryValue with a copy of other and a
   *        byteSize of byteSize
   * \param other The vector to be copied and held by this
   * \param byteSize The size of a byte in bit
   */
  MemoryValue(const std::vector<uint8_t> &other, const std::size_t byteSize);
  /**
   * \brief Constructs an MemoryValue with other and a ByteSize of byteSize
   * \param other The vector to acquire the data of
   * \param byteSize The size of a byte in bit
   */
  MemoryValue(std::vector<uint8_t> &&other, const std::size_t byteSize);
  /**
   * \brief Constructs a empty MemoryValue with byteAmount bytes of size
   *        byteSize
   * \param byteAmount Amount of Bytes
   * \param byteSize The size of a byte in bit
   */
  MemoryValue(std::size_t byteAmount, std::size_t byteSize);

  /**
   * \brief Constructs a MemoryValue with the data of other between begin and
   *        end with a _byteSize of byteSize
   * \param other the MemoryValue the data to be copied of
   * \param begin the begin index to be copied
   * \param end the first index no longer to be copied
   * \param byteSize The size of a byte in bit
   */
  MemoryValue(const MemoryValue &other,
              const std::size_t begin,
              const std::size_t end,
              const std::size_t byteSize);

  /**
  * \brief returns a MemoryValue with the data of this between begin and
  *        end with a _byteSize of byteSize
  * \param begin the begin index to be copied
  * \param end the first index no longer to be copied
  * \param byteSize The size of a byte in bit
  * \return a MemoryValue with the data of this between begin and end
  *         with a _byteSize of byteSize
  */
  MemoryValue subSet(const std::size_t begin,
                     const std::size_t end,
                     const std::size_t byteSize) const;

  /**
   * \brief returns the previous value at address
   * \param address The address of the bit
   * \return the value at address
   */
  bool get(const std::size_t address) const;
  /**
   * \brief sets the value at address to value
   * \param address The address of the bit
   * \param value The Value to be written
   */
  void put(const std::size_t address, const bool value = true);

  /**
   * \brief sets the value at address to value and returns the previous value
   * \param address The address of the bit
   * \param value The Value to be written
   * \return the previous value at address
   */
  bool set(const std::size_t address, const bool value = true);

  /**
   * \brief returns the size of a single byte in bit of the MemoryValue
   * \return the size of a single byte in the MemoryValue
   */
  std::size_t getByteSize() const;
  /**
   * \brief returns the num of bytes held by the MemoryValue
   * \return the amount of bytes held by the MemoryValue
   */
  std::size_t getByteAmount() const;
  /**
   * \brief returns the capacity of the MemoryValue in bit
   * \return the capacity of the MemoryValue in bit
   */
  std::size_t getSize() const;

  /**
   * \brief returns a reference to the data vector. For internal purposes only.
   *        Do not use or don't complain about crashes.
   * \return a reference to the data vector
   */
  const std::vector<uint8_t> &internal() const;

  /**
   * \brief returns true if this and other have the same _byteSize and
   *        _data.size() and the values representing the MemoryValue are
   *        identical, else returns false
   * \param the MemoryValue to be compared with
   * \return the equality of this and MemoryValue
   */
  bool operator==(const MemoryValue &other) const;

  /**
   * \brief returns false if this and other have the same _byteSize and
   *        _data.size() and the values representing the MemoryValue are
   *        identical, else returns true
   * \param other the MemoryValue to be compared with
   * \return the inequality of this and MemoryValue
   */
  bool operator!=(const MemoryValue &other) const;

  /**
   * \brief outputs the value onto the stream
   * \param stream stream to output value to
   * \param value the value to be outputted
   * \return the stream
   */
  friend std::ostream &
  operator<<(std::ostream &stream, const MemoryValue &value);

// This is not beautiful at all... But I think it is better than including a
// gtest header (i.e. external dependency).
#ifndef FRIEND_TEST
#define FRIEND_TEST(a, b)
#endif
  FRIEND_TEST(TestMemoryValue, charAt);
  FRIEND_TEST(TestMemoryValue, death);
#undef FRIEND_TEST

 private:
  std::size_t _byteSize;
  std::vector<std::uint8_t> _data;

  std::uint8_t getByteAt(std::size_t address) const;
};
#endif// ERAGPSIM_CORE_MEMORYVALUE_HPP_
