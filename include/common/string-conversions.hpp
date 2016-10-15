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

#include "common/optional.hpp"
#include "core/memory-value.hpp"

namespace StringConversions {

/**
 * \brief toBinString Converts a given MemoryValue to a corresponding binary
 * string.
 * \param memoryValue The MemoryValue to convert.
 * \return A binary string corresponding to the given MemoryValue.
 */
std::string toBinString(const MemoryValue& memoryValue);

/**
 * \brief toHexString Converts a given MemoryValue to a corresponding hex
 * string.
 * \param memoryValue The MemoryValue to convert.
 * \return A hex string corresponding to the given MemoryValue including the
 * "0x" hex indicator.
 */
std::string toHexString(const MemoryValue& memoryValue);

/**
 * \brief toUnsignedDecString Converts a given MemoryValue with a maximum size
 * of 64 bits to a corresponding unsigned decimal string.
 * \param memoryValue The MemoryValue to convert.
 * \return An unsigned decimal string corresponding to the given MemoryValue.
 */
std::string toUnsignedDecString(const MemoryValue& memoryValue);

/**
 * \brief toSignedDecString Converts a given MemoryValue with a maximum size of
 * 64 bits to a corresponding signed decimal string.
 * \param memoryValue The MemoryValue to convert.
 * \return A signed decimal string corresponding to the given MemoryValue.
 */
std::string toSignedDecString(const MemoryValue& memoryValue);


/**
 * \brief binStringToMemoryValue Converts a given binary string to a
 * corresponding MemoryValue.
 * \param stringValue Binary string to convert.
 * \param memoryValueSize Size of the memory value that is requested. Required
 * in order to be able to be tolerant towards incomplete input.
 * \return An optional MemoryValue corresponding to the given binary string or
 * empty optional if no conversion was possible.
 */
Optional<MemoryValue>
binStringToMemoryValue(const std::string& stringValue, size_t memoryValueSize);

/**
 * \brief hexStringToMemoryValue Converts a given hex string to a corresponding
 * MemoryValue.
 * \param stringValue Hex string to convert.
 * \param memoryValueSize Size of the memory value that is requested. Required
 * in order to be able to unambiguously create a MemoryValue and to be able to
 * be tolerant towards incomplete input.
 * \return An optional MemoryValue corresponding to the given hex string or
 * empty optional if no conversion was possible.
 */
Optional<MemoryValue>
hexStringToMemoryValue(const std::string& stringValue, size_t memoryValueSize);

/**
 * \brief unsignedDecStringToMemoryValue Converts a given unsigned decimal
 * string with a maximum size of 64 bits to a corresponding MemoryValue.
 * \param stringValue Unsigned decimal string to convert.
 * \param memoryValueSize Size of the memory value that is requested. Required
 * in order to be able to unambigiuously create a MemoryValue.
 * \return An optional MemoryValue corresponding to the given unsigned decimal
 * string or empty optional if no conversion was possible.
 */
Optional<MemoryValue>
unsignedDecStringToMemoryValue(const std::string& stringValue,
                               size_t memoryValueSize);

/**
 * \brief signedDecStringToMemoryValue Converts a given signed decimal string
 * with a maximum size of 64 bits to a corresponding MemoryValue.
 * \param stringValue Signed decimal string to convert.
 * \param memoryValueSize Size of the memory value that is requested. Required
 * in order to be able to unambigiuously create a MemoryValue.
 * \return An optional MemoryValue corresponding to the given signed decimal
 * string or empty optional if no conversion was possible.
 */
Optional<MemoryValue>
signedDecStringToMemoryValue(const std::string& stringValue,
                             size_t memoryValueSize);
}

#endif /* ERAGPSIM_COMMON_STRING_CONVERSIONS_HPP */
