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

#include "common/integer-information.hpp"
#include "gtest/gtest.h"

template <typename T>
void testType(bool isSigned, T min, T max, T negOne, std::size_t numberOfBits) {
  ASSERT_EQ(integerInformation<T>::_isSigned, isSigned);
  ASSERT_EQ(integerInformation<T>::_isUnsigned, !isSigned);
  ASSERT_EQ(integerInformation<T>::_min, min);
  ASSERT_EQ(integerInformation<T>::_max, max);
  ASSERT_EQ(integerInformation<T>::_negOne, negOne);
  ASSERT_EQ(integerInformation<T>::_numberOfBits, numberOfBits);
  ASSERT_EQ(integerInformation<T>::_numberOfUnsignedBits,
            numberOfBits - (isSigned ? 1 : 0));
}

TEST(integerInformation, standardTypes) {
  testType<std::uint8_t>(false, 0x00, 0xFF, 0xFF, 8);
  testType<std::uint16_t>(false, 0x00, 0xFFFF, 0xFFFF, 16);
  testType<std::uint32_t>(false, 0x00, 0xFFFFFFFF, 0xFFFFFFFF, 32);
  testType<std::uint64_t>(
      false, 0x00, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 64);
  testType<std::int8_t>(true, 0x00, 0x7F, -1, 8);
  testType<std::int16_t>(true, 0x00, 0x7FFF, -1, 16);
  testType<std::int32_t>(true, 0x00, 0x7FFFFFFF, -1, 32);
  testType<std::int64_t>(true, 0x00, 0x7FFFFFFFFFFFFFFF, -1, 64);
}
