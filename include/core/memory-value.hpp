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

#ifndef ERAGPSIM_CORE_MEMORYVALUE_HPP
#define ERAGPSIM_CORE_MEMORYVALUE_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>

#include "common/assert.hpp"
#include "common/utility.hpp"

class MemoryValue {
 public:
  using Underlying = std::vector<std::uint8_t>;
  using address_t = std::size_t;
  using size_t = std::size_t;

  /**
   * A proxy for a reference to a single bit.
   */
  class Reference {
   public:
    /**
     * Sets the bit proxied by the reference.
     *
     * \param value The boolean value to assign.
     * \return The reference itself.
     */
    Reference &operator=(bool value);

    /**
     * @return The boolean proxied by the reference object.
     */
    operator bool() const noexcept;

   private:
    friend class MemoryValue;

    /**
     * Constructs the reference.
     *
     * \param memory The memory value this instance references.
     * \parm address The index of the bit this instance references.
     */
    Reference(MemoryValue &memory, address_t address);

    /** The memory value referenced by the instance. */
    MemoryValue &_memory;

    /** The index of the bit referenced by the instance. */
    address_t _address;
  };

  using ConstReference = bool;

 private:
  /**
   * An iterator over a memory value.
   *
   * Additional relational operators are taken from the
   * `CompletelyOrdered` * * utility base class.
   */
  // clang-format off
  template <typename ReferenceType, typename = std::enable_if_t<
              std::is_same<ReferenceType, Reference>::value ||
              std::is_same<ReferenceType, ConstReference>::value>
  >
  // clang-format on
  class BaseIterator
      : public Utility::CompletelyOrdered<BaseIterator<ReferenceType, void>>,
        public std::iterator<std::random_access_iterator_tag, ReferenceType> {
   public:
    using super = std::iterator<std::random_access_iterator_tag, ReferenceType>;
    using typename super::difference_type;

    /**
     * Tests if the iterator is equal to another iterator.
     *
     * \other The other iterator to compare to.
     * \return True if the iterators point to the same address in the same
     * memory value.
     */
    bool operator==(const BaseIterator &other) const noexcept override {
      if (!pointsToSameMemoryAs(other)) return false;
      if (this->_address != other._address) return false;
      return true;
    }

    /**
     * Tests the iterator is less than another iterator.
     *
     * \other The other iterator to compare to.
     * \return True if the iterator points to the same memory value and has an
     * address less than that of the other iterator.
     * \throws AssertionError if the other iterator points to another memory
     * value.
     */
    bool operator<(const BaseIterator &other) const override {
      assert::that(pointsToSameMemoryAs(other));
      return this->_address < other._address;
    }

    /**
     * \return A reference to the memory value bit at the current address.
     */
    ReferenceType operator*() {
      return _memory[_address];
    }

    /**
     * Allows use of the arrow operator (->).
     * \return A pointer to a reference to the memory value bit at the current
     * address.
     */
    ReferenceType &operator->() {
      return &_memory[_address];
    }

    /**
     * Increments the iterator to the next address.
     * \return A reference to the memory value bit at the resulting location.
     */
    BaseIterator &operator++() noexcept {
      ++_address;
      return *this;
    }

    /**
     * Increments the iterator to the next address.
     * \return A reference to the memory value bit at the old location.
     */
    BaseIterator operator++(int)noexcept {
      auto copy = *this;
      ++this;
      return copy;
    }

    /**
     * Decrements the iterator to the previous address.
     * \return A reference to the memory value bit at the resulting location.
     */
    BaseIterator &operator--() noexcept {
      --_address;
      return *this;
    }

    /**
     * Decrements the iterator to the previous address.
     * \return A reference to the memory value bit at the old location.
     */
    BaseIterator operator--(int)noexcept {
      auto copy = *this;
      --this;
      return copy;
    }

    /**
     * Moves the iterator forwards by the specified distance.
     * \return A refererence to the resulting iterator.
     */
    BaseIterator &operator+=(difference_type distance) noexcept {
      _address += distance;
      return *this;
    }

    /**
     * \return The iterator resulting from moving the iterator forwards by the
     * specified distance.
     */
    BaseIterator operator+(difference_type distance) const noexcept {
      auto copy = *this;
      copy += distance;
      return copy;
    }

    /**
     * Moves the iterator backwards by the specified distance.
     * \return A refererence to the resulting iterator.
     */
    BaseIterator &operator-=(difference_type distance) noexcept {
      _address -= distance;
      return *this;
    }

    /**
     * \return The iterator resulting from moving the iterato backwards by the
     * specified distance.
     */
    BaseIterator operator-(difference_type distance) const noexcept {
      auto copy = *this;
      copy -= distance;
      return copy;
    }

    /**
     * \return The distance between this iterator and the other iterator.
     */
    difference_type operator-(const BaseIterator &other) const {
      assert::that(pointsToSameMemoryAs(other));
      return this->_address - other._address;
    }

    /**
     * \return The numeric address currently pointed at by the memory value.
     */
    address_t getAddress() const noexcept {
      return _address;
    }

    /**
     * Tests if this iterator points to the same memory
     * (value) as another iterator.
     *
     * \param  other The other iterator to test against.
     * \return       True if the other iterator points to the same memory value
     *               as this one, else false.
     */
    bool pointsToSameMemoryAs(const BaseIterator &other) const noexcept {
      return &(this->_memory) == &(other._memory);
    }

   private:
    friend class MemoryValue;

    // clang-format off
    using TargetType = typename std::conditional<
        std::is_same<ReferenceType, Reference>::value,
        MemoryValue,
        const MemoryValue
    >::type;
    // clang-format on

    /**
     * Constructs the reference.
     *
     * \param memory The memory value this iterator references.
     * \parm address The start address of the iterator.
     */
    BaseIterator(TargetType &memory, address_t address)
    : _memory(memory), _address(address) {
    }

    /** The memory value the iterator is referencing. */
    TargetType &_memory;

    /** The current address the iterator is referencing. */
    address_t _address;
  };

 public:
  using Iterator = BaseIterator<Reference>;
  using ConstIterator = BaseIterator<ConstReference>;

  /**
   * \brief Constructs an empty MemoryValue of default length, 1 byte á 8
   * bit
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
   *        size of size
   * \param other The vector to be copied and held by this
   * \param size The size of the MemoryValue in bit
   */
  MemoryValue(const Underlying &other, address_t size);
  /**
   * \brief Constructs an MemoryValue with other and a size of size
   * \param other The vector to acquire the datas of
   * \param size The size of the MemoryValue in bit
   */
  MemoryValue(Underlying &&other, address_t size);
  /**
   * \brief Constructs a empty MemoryValue with a size of size
   * \param size The size of the MemoryValue in bit
   */
  explicit MemoryValue(size_t size);

  /**
   * \brief Constructs a MemoryValue with the data of other between begin and
   *        end
   * \param other the MemoryValue the data to be copied of
   * \param begin the begin index to be copied
   * \param end the first index no longer to be copied
   */
  MemoryValue(const MemoryValue &other, address_t begin, address_t end);

  /**
  * \brief returns a MemoryValue with the data of this between begin and
  *        end
  * \param begin the begin index to be copied
  * \param end the first index no longer to be copied
  * \return a MemoryValue with the data of this between begin and end
  *         with a _byteSize of byteSize
  */
  MemoryValue subSet(address_t begin, address_t end) const;

  /**
   * \return An iterator pointing to the start of the memory value.
   */
  Iterator begin() noexcept;

  /**
   * \return A const iterator pointing to the start of the memory value.
   */
  ConstIterator cbegin() const noexcept;

  /**
   * \return A const iterator pointing to the start of the memory value.
   */
  ConstIterator begin() const noexcept;

  /**
   * \return An iterator pointing to the end of the memory value.
   */
  Iterator end() noexcept;

  /**
   * \return A const iterator pointing to the end of the memory value.
   */
  ConstIterator cend() const noexcept;

  /**
   * \return A const iterator pointing to the end of the memory value.
   */
  ConstIterator end() const noexcept;

  /**
  * Gives access to the memory value through an
  * iterator from the given address onwards.
  * \param address The address at which to start the iterator.
  * \return An iterator pointing to the specified address.
  */
  Iterator getIterator(address_t address);

  /**
   * Gives access to the memory value through a
   * const iterator from the given address onwards.
   * \param address The address at which to start the iterator.
   * \return An iterator pointing to the specified address.
   */
  ConstIterator getIterator(address_t address) const;

  /**
   * \brief returns the previous value at address
   * \param address The address of the bit
   * \return the value at address
   */
  bool get(address_t address) const;
  /**
   * \brief sets the value at address to value
   * \param address The address of the bit
   * \param value The Value to be written
   */
  void put(address_t address, bool value = true);

  /**
   * \brief sets the value at address to value and returns the previous value
   * \param address The address of the bit
   * \param value The Value to be written
   * \return the previous value at address
   */
  bool set(address_t address, bool value = true);

  /**
  * \brief flips the value at address and returns the previous value
  * \param address The address of the bit
  * \return the previous value at address
  */
  bool flip(address_t address);

  /**
   * Returns a proxy reference to a single bit.
   *
   * @param  address The address at which to access the memory value.
   * @return A proxy that acts like a reference.
   */
  Reference operator[](address_t address);

  /**
   * Returns a proxy reference to a single bit.
   *
   * @param  address The address at which to access the memory value.
   * @return A proxy that acts like a reference.
   */
  ConstReference operator[](address_t address) const;

  /**
   * \brief returns the capacity of the MemoryValue in bit
   * \return the capacity of the MemoryValue in bit
   */
  address_t getSize() const;

  /**
   * Checks if all bits are set to zero.
   * \return true if all bits are set to zero.
   */
  bool isZero();


  /**
   * @return The first bit of the memory value.
   */
  bool front() const;

  /**
   * @return The first bit of the memory value.
   */
  bool back() const;


  /**
   * \brief returns a reference to the data vector. For internal purposes only.
   *        Do not use or don't complain about crashes.
   * \return a reference to the data vector
   */
  const Underlying &internal() const;

  /**
   * \brief returns true iff this and other have the same _byteSize and
   *        _data.size() and the values representing the MemoryValue are
   *        identical, else returns false
   * \param the MemoryValue to be compared with
   * \return the equality of this and MemoryValue
   */
  bool operator==(const MemoryValue &other) const;

  /**
   * \brief returns false iff this and other have the same _byteSize and
   *        _data.size() and the values representing the MemoryValue are
   *        identical, else returns true
   * \param other the MemoryValue to be compared with
   * \return the inequality of this and MemoryValue
   */
  bool operator!=(const MemoryValue &other) const;

  /**
  * \brief returns writes the data of other into this starting with the
  *        begin-th bit
  * \param other the MemoryValue to be written
  * \param begin the index of the first bit to be written to
  */
  void write(const MemoryValue &other, address_t begin = 0);

  /**
   * \brief outputs the value onto the stream
   * \param stream stream to output value to
   * \param value the value to be outputted
   * \return the stream
   */
  friend std::ostream &
  operator<<(std::ostream &stream, const MemoryValue &value);
  // friend MemoryValue conversions::permute(const MemoryValue &memoryValue,
  //  address_t byteSize,
  //  const std::vector<address_t> permutation);
  // I know this is extremely inelegant
  friend std::uint8_t getByteAt(MemoryValue memoryValue, address_t address) {
    return memoryValue.getByteAt(address);
  }


#ifdef FRIEND_TEST
  FRIEND_TEST(TestMemoryValue, charAt);
  FRIEND_TEST(TestMemoryValue, death);
  FRIEND_TEST(TestMemoryValue, charAt2);
#endif

 private:
  std::uint8_t getByteAt(address_t address) const;

  address_t _size;  /**< Brief Size of the MemoryValue in Bit*/
  Underlying _data; /**< Brief The Data stored by the MemoryValue*/
};
#endif// ERAGPSIM_CORE_MEMORYVALUE_HPP_
