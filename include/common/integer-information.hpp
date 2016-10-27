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
  const static bool _isSigned;
  // is true iff T is any unsigned Type (0-1>=0)
  const static bool _isUnsigned;
  // the smallest number representable by T
  const static T _min;
  // the biggest number representable by T
  const static T _max;
  // if T is a integer type that is represented by bits, every bit is set to 1
  const static T _negOne;
  // if T is a integer type that is represented by bits, the number of those
  const static std::size_t _numberOfBits;
  // if T is a integer type that is represented by bits, the number of those
  // that dont change the sign
  const static std::size_t _numberOfUnsignedBits;

 private:
  // number of maximum iterations to not endlessly loop for dynamic int types
  static constexpr std::size_t _maxIterations = 65536;
  // is true iff T is any signed Type (0-1<0)
  static bool isSigned() {
    return static_cast<T>(T{0} - T{1}) < T{0};
  }
  // is true iff T is any unsigned Type (0-1>=0)
  static bool isUnsigned() {
    return !_isSigned;
  }
  // the smallest number representable by T
  static T min() {
    if (_isUnsigned) {
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
  static T max() {
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
  static T negOne() {
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
  // if T is a integer type that is represented by bits, the number of those
  static std::size_t numberOfBits() {
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
  static std::size_t numberOfUnsignedBits() {
    if (_isUnsigned) {
      return _numberOfBits;
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
const bool integerInformation<T>::_isSigned = integerInformation<T>::isSigned();
template <typename T>
const bool
    integerInformation<T>::_isUnsigned = integerInformation<T>::isUnsigned();
template <typename T>
const T integerInformation<T>::_min = integerInformation<T>::min();
template <typename T>
const T integerInformation<T>::_max = integerInformation<T>::max();
template <typename T>
const T integerInformation<T>::_negOne = integerInformation<T>::negOne();
template <typename T>
const std::size_t integerInformation<T>::_numberOfBits =
    integerInformation<T>::numberOfBits();
template <typename T>
const std::size_t integerInformation<T>::_numberOfUnsignedBits =
    integerInformation<T>::numberOfUnsignedBits();
