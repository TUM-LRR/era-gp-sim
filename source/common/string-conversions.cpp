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

#include "common/string-conversions.hpp"

#include <cstdint>
#include <iomanip>
#include <sstream>

#include "core/conversions.hpp"

namespace StringConversions {

/**
 * \brief _toDecString Converts a given integral typed value to string.
 * \tparam T Integral type of the given value.
 * \param intValue Integral value to convert.
 * \return String corresponding to the given integral value.
 */
template <typename T>
std::string _toDecString(T intValue) {
  std::stringstream stream;
  stream << std::dec << intValue;
  return stream.str();
}


std::string toBinString(const MemoryValue& memoryValue) {
  std::string result;
  result.reserve(memoryValue.getSize());
  // Iterate over each bit of the memory value and insert the corresponding
  // binary digit.
  for (size_t currentBitAddress = 0; currentBitAddress < memoryValue.getSize();
       ++currentBitAddress) {
    bool currentBitValue =
        memoryValue.get(memoryValue.getSize() - 1 - currentBitAddress);
    result.push_back(currentBitValue ? '1' : '0');
  }
  // Add the binary-indicator-prefix and return.
  return result.insert(0, "0b");
}


std::string toHexString(const MemoryValue& memoryValue) {
  const std::vector<uint8_t>& internal = memoryValue.internal();
  std::stringstream stream;
  // Iterate over the 8bit-values of the internal representation to convert each
  // into hex-values.
  // In order to be able to conveniently push the hex-digits into the stream,
  // the internal representation is iterated in reverse, starting with the most
  // significant bit.
  for (auto it = internal.rbegin(); it != internal.rend(); ++it) {
    // The current 8bit-value.
    uint8_t currentValue = *it;
    // Convert the current 8bit-value into hex (= two digits) and push it into
    // the stream, inserting leading zeros where required.
    stream << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex
           << static_cast<int>(currentValue);
  }
  std::string result = stream.str();
  // Remove leading digits that exceed the memory value's specified bit-size.
  result = result.substr((result.length() * 4 - memoryValue.getSize()) / 4);
  // Add the hex-indicator-prefix and return.
  return result.insert(0, "0x");
}


std::string toUnsignedDecString(const MemoryValue& memoryValue) {
  std::uintmax_t intermediateValue =
      conversions::convert<std::uintmax_t>(memoryValue);
  // Convert integer to dec string.
  return _toDecString(intermediateValue);
}


std::string toSignedDecString(const MemoryValue& memoryValue) {
  std::intmax_t intermediateValue =
      conversions::convert<std::intmax_t>(memoryValue);
  // Convert integer to dec string.
  return _toDecString(intermediateValue);
}


Optional<MemoryValue>
binStringToMemoryValue(const std::string& stringValue, size_t memoryValueSize) {
  std::string stringValueNoPrefix = stringValue;
  // Remove the hex-indicator-prefix if necessary.
  if (stringValue.find("0b") == 0) {
    stringValueNoPrefix = stringValueNoPrefix.substr(2);
  }
  std::vector<uint8_t> resultingInternal;
  // Read the string starting with the least significant bit until no more
  // complete byte is available.
  for (size_t index = stringValueNoPrefix.length();; index -= 8) {
    size_t startPos = (index >= 8) ? index - 8 : 0;
    size_t length   = (index >= 8) ? 8 : index;
    // Try to parse the string-byte to an 8bit-integer value.
    uint8_t currentValue;
    try {
      currentValue =
          std::stoi(stringValueNoPrefix.substr(startPos, length), nullptr, 2);
    } catch (const std::invalid_argument& e) {
      return Optional<MemoryValue>();
    }
    resultingInternal.push_back(currentValue);
    // Stop iterating when the value that was just added was the last
    // value of the string
    // OR when pushing the next internal value would result in the
    // internal being larger than the specified expected memoryValueSize.
    // (i.e. unnecessary leading bits are cut off)
    if (index <= 8 || resultingInternal.size() * 8 >= memoryValueSize) {
      break;
    }
  }
  // Insert implicit-zero leading zeros.
  while (resultingInternal.size() * 8 < memoryValueSize) {
    resultingInternal.push_back(0);
  }
  return MemoryValue{resultingInternal, memoryValueSize};
}


Optional<MemoryValue>
hexStringToMemoryValue(const std::string& stringValue, size_t memoryValueSize) {
  std::string stringValueNoPrefix = stringValue;
  // Remove the hex-indicator-prefix if necessary.
  if (stringValue.find("0x") == 0) {
    stringValueNoPrefix = stringValueNoPrefix.substr(2);
  }
  std::vector<uint8_t> resultingInternal;
  // Iterate over the input string in 8bit-steps (= 2 hex digits), starting with
  // the least significant bit.
  for (auto it = stringValueNoPrefix.rbegin();
       it < stringValueNoPrefix.rend();
       it += 2) {
    // Initializes the string representing the current 8bit with the first 4bit
    // directly behind the iterator.
    std::string currentValueString(1, *it);
    // If another 4 bit following the first 4 bit are available, add them as
    // well.
    if ((it + 1) != stringValueNoPrefix.rend()) {
      currentValueString = *(it + 1) + currentValueString;
    }
    // currentValueString has a maximum of two hex digits which results in a
    // 8bit-integer.
    uint8_t currentValue;
    try {
      currentValue = std::stoi(currentValueString, nullptr, 16);
    } catch (const std::invalid_argument& e) {
      return Optional<MemoryValue>();
    }

    resultingInternal.push_back(currentValue);
    // Stop iterating when pushing the next internal value would result in the
    // internal being larger than the specified expected memoryValueSize.
    // (i.e. unnecessary leading digits are cut off)
    if ((resultingInternal.size() * 2 + 1) * 4 > memoryValueSize) {
      break;
    }
  }
  // Insert implicit-zero leading zeros.
  while (resultingInternal.size() * 8 < memoryValueSize) {
    resultingInternal.push_back(0);
  }
  return MemoryValue{resultingInternal, memoryValueSize};
}


Optional<MemoryValue>
unsignedDecStringToMemoryValue(const std::string& stringValue,
                               size_t memoryValueSize) {
  // Try to parse string value to int.
  std::uintmax_t intermediateValue;
  try {
    intermediateValue = std::stoll(stringValue, nullptr, 10);
  } catch (const std::invalid_argument& e) {
    return Optional<MemoryValue>();
  } catch (const std::out_of_range& e) {
      return Optional<MemoryValue>();
  }
  // Convert int to MemoryValue.
  MemoryValue result =
      conversions::convert<std::uintmax_t>(intermediateValue, memoryValueSize);
  return result;
}


Optional<MemoryValue>
signedDecStringToMemoryValue(const std::string& stringValue,
                             size_t memoryValueSize) {
  // Try to parse string value to int.
  std::intmax_t intermediateValue;
  try {
    intermediateValue = std::stoll(stringValue, nullptr, 10);
  } catch (const std::invalid_argument& e) {
    return Optional<MemoryValue>();
  } catch (const std::out_of_range& e) {
      return Optional<MemoryValue>();
  }
  // Convert int to MemoryValue.
  MemoryValue result =
      conversions::convert<std::intmax_t>(intermediateValue, memoryValueSize);
  return result;
}
}
