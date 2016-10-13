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

#ifndef ERAGPSIM_COMMON_STRING_CONVERSIONS_HPP
#define ERAGPSIM_COMMON_STRING_CONVERSIONS_HPP

#include "core/memory-value.hpp"

namespace StringConversions {

/**
 * @brief toBinString Converts a given MemoryValue to a corresponding binary string.
 * @param memoryValue The MemoryValue to convert.
 * @return A binary string corresponding to the given MemoryValue.
 */
std::string toBinString(MemoryValue memoryValue);

/**
 * @brief toHexString Converts a given MemoryValue to a corresponding hex string.
 * @param memoryValue The MemoryValue to convert.
 * @return A hex string corresponding to the given MemoryValue.
 */
std::string toHexString(MemoryValue memoryValue);

/**
 * @brief toUnsignedDecString Converts a given MemoryValue with a maximum size of 64 bits to a corresponding unsigned decimal string.
 * @param memoryValue The MemoryValue to convert.
 * @return An unsigned decimal string corresponding to the given MemoryValue.
 */
std::string toUnsignedDecString(MemoryValue memoryValue);

/**
 * @brief toSignedDecString Converts a given MemoryValue with a maximum size of 64 bits to a corresponding signed decimal string.
 * @param memoryValue The MemoryValue to convert.
 * @return A signed decimal string corresponding to the given MemoryValue.
 */
std::string toSignedDecString(MemoryValue memoryValue);

/**
 * @brief toDecimalFloatString Converts a given MemoryValue with a maximum size of 64 bits to a corresponding decimal float string.
 * @param memoryValue The MemoryValue to convert.
 * @return A decimal float string corresponding to the given MemoryValue.
 */
std::string toDecimalFloatString(MemoryValue memoryValue);


/**
 * @brief hexStringToMemoryValue Converts a given binary string to a corresponding MemoryValue.
 * @param stringValue Binary string to convert.
 * @param memoryValueSize Size of the memory value that is requested. Required in order to be able to be tolerant towards faulty input.
 * @return A MemoryValue corresponding to the given binary string.
 */
MemoryValue
binStringToMemoryValue(std::string stringValue, size_t memoryValueSize);

/**
 * @brief hexStringToMemoryValue Converts a given hex string to a corresponding MemoryValue.
 * @param stringValue Hex string to convert.
 * @param memoryValueSize Size of the memory value that is requested. Required in order to be able to unambiguously create a MemoryValue and to be able to be tolerant towards faulty input.
 * @return A MemoryValue corresponding to the given hex string.
 */
MemoryValue
hexStringToMemoryValue(std::string stringValue, size_t memoryValueSize);

/**
 * @brief unsignedDecStringToMemoryValue Converts a given unsigned decimal string with a maximum size of 64 bits to a corresponding MemoryValue.
 * @param stringValue Unsigned decimal string to convert.
 * @param memoryValueSize Size of the memory value that is requested. Required in order to be able to unambigiuously create a MemoryValue.
 * @return A MemoryValue corresponding to the given unsigned decimal string.
 */
MemoryValue unsignedDecStringToMemoryValue(std::string stringValue, size_t memoryValueSize);

/**
 * @brief signedDecStringToMemoryValue Converts a given signed decimal string with a maximum size of 64 bits to a corresponding MemoryValue.
 * @param stringValue Signed decimal string to convert.
 * @param memoryValueSize Size of the memory value that is requested. Required in order to be able to unambigiuously create a MemoryValue.
 * @return A MemoryValue corresponding to the given signed decimal string.
 */
MemoryValue signedDecStringToMemoryValue(std::string stringValue, size_t memoryValueSize);

/**
 * @brief decimalFloatStringToMemoryValue Converts a given decimal float string with a maximum size of 64 bits to a corresponding MemoryValue.
 * @param stringValue Decimal float string to convert.
 * @param memoryValueSize Size of the memory value that is requested. Required in order to be able to unambigiuously create a MemoryValue.
 * @return A MemoryValue corresponding to the given decimal float string.
 */
MemoryValue decimalFloatStringToMemoryValue(std::string stringValue, size_t memoryValueSize);

/**
 * @brief toDecString Converts a given integral typed value to string.
 * @tparam T Integral type of the given value.
 * @param intValue Integral value to convert.
 * @return String corresponding to the given integral value.
 */
template <typename T>
std::string toDecString(T intValue);
}

#endif /* ERAGPSIM_COMMON_STRING_CONVERSIONS_HPP */
