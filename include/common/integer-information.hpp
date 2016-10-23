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
  const static bool _isSigned;
  const static bool _isUnsigned;
  const static T _min;
  const static T _max;
  const static T _negOne;
  const static std::size_t _numberOfBits;
  const static std::size_t _numberOfUnsignedBits;

 private:
  static constexpr std::size_t maxIterations = 65536;
  static bool isSigned() {
    return static_cast<T>(T{0} - T{1}) < T{0};
  }
  static bool isUnsigned() {
    return !_isSigned;
  }
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
      if (i++ > maxIterations) {
        // Die a horrible death and burn in hell
      }
    }
    return current;
  }
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
      if (i++ > maxIterations) {
        // Die a horrible death and burn in hell
      }
    }
    return current;
  }
  static T negOne() {
    T current{1};
    T previous{0};
    std::size_t i = 0;
    while (previous != current) {
      previous = current;
      current <<= T{1};
      current |= T{1};
      if (i++ > maxIterations) {
        // Die a horrible death and burn in hell
      }
    }
    return current;
  }
  static std::size_t numberOfBits() {
    std::size_t count = 0;
    T current{1};
    T previous{0};
    while (previous != current) {
      previous = current;
      current <<= T{1};
      current |= T{1};
      ++count;
      if (count > maxIterations) {
        // Die a horrible death and burn in hell
      }
    }
    return count;
  }
  static std::size_t numberOfUnsignedBits() {
    if (_isUnsigned) {
      return _numberOfBits;
    }
    std::size_t count = 0;
    T current{1};
    while (current > 0) {
      current <<= T{1};
      ++count;
      if (count > maxIterations) {
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
