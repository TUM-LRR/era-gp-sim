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

#include <cstdint>

template <typename T>
class integerInformation {
 public:
  // is true iff T is any signed Type (0-1<0)
  const static bool isSigned;
  // is true iff T is any unsigned Type (0-1>=0)
  const static bool isUnsigned;
  // the smallest number representable by T
  const static T min;
  // the biggest number representable by T
  const static T max;
  // if T is a integer type that is represented by bits, every bit is set to 1
  const static T negOne;
  // if T is a integer type that is represented by bits, only the uppermost bit
  // is set to 1
  const static T setUpper;
  // if T is a integer type that is represented by bits, the number of those
  const static std::size_t numberOfBits;
  // if T is a integer type that is represented by bits, the number of those
  // that dont change the sign
  const static std::size_t numberOfUnsignedBits;

 private:
  // number of maximum iterations to not endlessly loop for dynamic int types
  static constexpr std::size_t _maxIterations = 65536;
  // is true iff T is any signed Type (0-1<0)
  static bool _isSigned() {
    return static_cast<T>(T{0} - T{1}) < T{0};
  }
  // is true iff T is any unsigned Type (0-1>=0)
  static bool _isUnsigned() {
    return !isSigned;
  }
  // the smallest number representable by T
  static T _min() {
    if (isUnsigned) {
      return T{0};
    }
    T current{0};
    T previous{current};
    T operand{1};
    std::size_t i = 0;
    while (operand != T{0}) {
      current -= operand;
      if (current >= previous) {
        operand >>= T{1};
        current = previous;
      } else {
        T tmp = operand << T{1};
        if (tmp > T{0}) {
          operand = tmp;
        }
        previous = current;
      }
      if (i++ > _maxIterations) {
        // Die a horrible death and burn in hell
      }
    }
    return current;
  }
  // the biggest number representable by T
  static T _max() {
    T current{0};
    T previous{current};
    T operand{1};
    std::size_t i = 0;
    while (operand != T{0}) {
      current += operand;
      if (current <= previous) {
        operand >>= T{1};
        current = previous;
      } else {
        T tmp = operand << T{1};
        if (tmp > T{0}) {
          operand = tmp;
        }
        previous = current;
      }
      if (i++ > _maxIterations) {
        // Die a horrible death and burn in hell
      }
    }
    return current;
  }
  // if T is a integer type that is represented by bits, every bit is set to 1
  static T _negOne() {
    T current{1};
    T previous{0};
    std::size_t i = 0;
    while (previous != current) {
      previous = current;
      current <<= T{1};
      current |= T{1};
      if (i++ > _maxIterations) {
        // Die a horrible death and burn in hell
      }
    }
    return current;
  }
  // if T is a integer type that is represented by bits, only the uppermost bit
  // is set to 1
  static T _setUpper() {
    T current{1};
    T previous{0};
    std::size_t i = 0;
    while (current != T{0}) {
      previous = current;
      current <<= T{1};
      if (i++ > _maxIterations) {
        // Die a horrible death and burn in hell
      }
    }
    return previous;
  }
  // if T is a integer type that is represented by bits, the number of those
  static std::size_t _numberOfBits() {
    std::size_t count = 0;
    T current{1};
    T previous{0};
    while (previous != current) {
      previous = current;
      current <<= T{1};
      current |= T{1};
      ++count;
      if (count > _maxIterations) {
        // Die a horrible death and burn in hell
      }
    }
    return count;
  }
  // if T is a integer type that is represented by bits, the number of those
  // that dont change the sign
  static std::size_t _numberOfUnsignedBits() {
    if (isUnsigned) {
      return numberOfBits;
    }
    std::size_t count = 0;
    T current{1};
    while (current > 0) {
      current <<= T{1};
      ++count;
      if (count > _maxIterations) {
        // Die a horrible death and burn in hell
      }
    }
    return count;
  }
};

template <typename T>
const bool integerInformation<T>::isSigned = integerInformation<T>::_isSigned();
template <typename T>
const bool
    integerInformation<T>::isUnsigned = integerInformation<T>::_isUnsigned();
template <typename T>
const T integerInformation<T>::min = integerInformation<T>::_min();
template <typename T>
const T integerInformation<T>::max = integerInformation<T>::_max();
template <typename T>
const T integerInformation<T>::negOne = integerInformation<T>::_negOne();
template <typename T>
const T integerInformation<T>::setUpper = integerInformation<T>::_setUpper();
template <typename T>
const std::size_t integerInformation<T>::numberOfBits =
    integerInformation<T>::_numberOfBits();
template <typename T>
const std::size_t integerInformation<T>::numberOfUnsignedBits =
    integerInformation<T>::_numberOfUnsignedBits();
