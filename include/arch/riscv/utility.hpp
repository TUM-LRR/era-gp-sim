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

#ifndef ERAGPSIM_ARCH_RISCV_UTILITY_HPP
#define ERAGPSIM_ARCH_RISCV_UTILITY_HPP

#include <climits>
#include <limits>
#include <type_traits>

#include "arch/riscv/properties.hpp"
#include "core/conversions.hpp"
#include "core/memory-access.hpp"

namespace riscv {
/**
 * Utility function to convert a memory value for RISC-V.
 *
 * The riscv::ENDIANNESS is used.
 *
 * \tparam T The type to convert to.
 * \param memory The memory value to convert.
 *
 * \return The converted value.
 */
template <typename T>
T convert(const MemoryValue& memoryValue) {
  return conversions::convert<T>(memoryValue,
                                 riscv::BITS_PER_BYTE,
                                 riscv::ENDIANNESS,
                                 riscv::SIGNED_REPRESENTATION);
}

/**
 * Utility function to convert a value into a memory value for RISC-V.
 *
 * The riscv::ENDIANNESS is used.
 *
 * \tparam T The type to convert to.
 * \param value The value to convert.
 *
 * \return The converted memory value.
 */
template <typename T>
std::enable_if_t<std::is_integral<T>::value, MemoryValue>
convert(const T& value) {
  static const auto digits = sizeof(T) * CHAR_BIT;
  return conversions::convert(value,
                              digits,
                              riscv::BITS_PER_BYTE,
                              riscv::ENDIANNESS,
                              riscv::SIGNED_REPRESENTATION);
}

template <typename T>
std::enable_if_t<std::is_integral<T>::value, MemoryValue>
convert(const T& value, std::size_t size) {
    return conversions::convert(value, size, riscv::BITS_PER_BYTE, riscv::ENDIANNESS, riscv::SIGNED_REPRESENTATION);
}

/**
 * Utility function to retrieve a register value for RISC-V.
 *
 * \tparam T The type to convert to.
 * \param memoryAccess The memory access object.
 * \param name The name of the register to access.
 *
 * \return The value for the child.
 */
template <typename T>
T loadRegister(MemoryAccess& memoryAccess, const std::string& registerName) {
  auto memory = memoryAccess.getRegisterValue(registerName).get();
  return riscv::convert<T>(memory);
}

/**
 * Utility function to store a register value for RISC-V.
 *
 * \tparam T The type to convert to.
 * \param memoryAccess The memory access object.
 * \param name The name of the register to access.
 * \param value The value to convert and store.
 */
template <typename T>
void storeRegister(MemoryAccess& memoryAccess,
                   const std::string& registerName,
                   const T& value) {
  auto memory = convert(value);
  memoryAccess.putRegisterValue(registerName, memory);
}

template <typename UnsignedWord>
std::enable_if_t<std::is_unsigned<UnsignedWord>::value, bool>
isAddressValid(MemoryAccess& memoryAccess, UnsignedWord absoluteAdress) {
    UnsignedWord lowerBound = 0;//TODO first adress of assembled code
    UnsignedWord upperBound = memoryAccess.getMemorySize().get();//TODO last adress of assembled code
    return absoluteAdress >= lowerBound && absoluteAdress < upperBound;
}
}

#endif /* ERAGPSIM_ARCH_RISCV_UTILITY_HPP */
