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
#include <iomanip>
#include <sstream>

namespace StringConversions {

std::string toBinString(MemoryValue memoryValue) {
  std::string result = "";
  // Iterate over each bit of the memory value and insert the corresponding
  // binary digit.
  for (size_t currentBitAddress = 0; currentBitAddress < memoryValue.getSize();
       ++currentBitAddress) {
    bool currentBitValue         = memoryValue.get(currentBitAddress);
    std::string currentBitString = (currentBitValue) ? "1" : "0";
    result.insert(0, currentBitString);
  }
  // Remove leading digits that exceed the memory value's specified bit-size.
  result = result.substr(result.length() - memoryValue.getSize());
  return result;
}


std::string toHexString(MemoryValue memoryValue) {
  std::vector<uint8_t> internal = memoryValue.internal();
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
    // the stream.
    stream << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex
           << (int)currentValue;
  }
  std::string result = stream.str();
  // Remove leading digits that exceed the memory value's specified bit-size.
  result = result.substr((result.length() * 4 - memoryValue.getSize())/4);
  // Add the hex-indicator-prefix and return.
  return result.insert(0, "0x");
}


std::string toUnsignedDecString(MemoryValue memoryValue) {
  uint64_t intermediateValue =
      0;// TODO: Use _unsigned_ conversion from memoryValue to int64_t
  // Convert integer to dec string.
  return toDecString(intermediateValue);
}


std::string toSignedDecString(MemoryValue memoryValue) {
  int64_t intermediateValue =
      0;// TODO: Use _signed_ conversion from memoryValue to int64_t
  // Convert integer to dec string.
  return toDecString(intermediateValue);
}


std::string toDecimalFloatString(MemoryValue memoryValue) {
  double intermediateValue =
      0.0;// TODO: Use conversion from memoryValue to double
  std::ostringstream stream;
  stream.precision(std::numeric_limits<double>::digits10);
  stream << std::fixed << intermediateValue;
  // TODO: Remove padding??
  return stream.str();
}


MemoryValue
binStringToMemoryValue(std::string stringValue, size_t memoryValueSize) {
  std::vector<uint8_t> resultingInternal;
  // Read the string starting with the least significant bit until no more
  // complete byte is available.
  for (size_t index = stringValue.length(); ; index -= 8) {
    size_t startPos = (index >= 8) ? index-8 : 0;
    size_t length = (index >= 8) ? 8 : index;
    // Parse the string-byte to an 8bit-integer value.
    uint8_t currentValue =
        std::stoi(stringValue.substr(startPos, length), nullptr, 2);
    resultingInternal.push_back(currentValue);
    // Stop iterating when the value that was just added was the last
    // value of the string
    // OR when pushing the next internal value would result in the
    // internal being too big for the specified expected memoryValueSize.
    // (i.e. unnecessary leading bits are cut off)
    if (index <= 8 || resultingInternal.size()*8 >= memoryValueSize) {
        break;
    }
  }
  // Insert implicit-zero leading zeros.
  while (resultingInternal.size()*8 < memoryValueSize) {
      resultingInternal.push_back(0);
  }
  return MemoryValue(resultingInternal, memoryValueSize);
}


MemoryValue
hexStringToMemoryValue(std::string stringValue, size_t memoryValueSize) {
  // Remove the hex-indicator-prefix if necessary.
  if (stringValue.find("0x") == 0) {
    stringValue = stringValue.substr(2);
  }
  std::vector<uint8_t> resultingInternal;
  // Iterate over the input string in 8bit-steps (= 2 hex digits), starting with
  // the least significant bit.
  for (auto it = stringValue.rbegin(); it < stringValue.rend(); it += 2) {
    // Initializes the string representing the current 8bit with the first 4bit
    // directly behind the iterator.
    std::string currentValueString(1, *it);
    // If another 4 bit following the first 4 bit are available, add them as
    // well.
    if ((it + 1) != stringValue.rend()) {
      currentValueString = *(it + 1) + currentValueString;
    }
    // currentValueString has a maximum of two hex digits which results in a
    // 8bit-integer.
    uint8_t currentValue = std::stoi(currentValueString, nullptr, 16);
    resultingInternal.push_back(currentValue);
    // Stop iterating when pushing the next internal value would result in the
    // internal being too big for the specified expected memoryValueSize.
    // (i.e. unnecessary leading digits are cut off)
    if ((resultingInternal.size() * 2 + 1) * 4 > memoryValueSize) {
      break;
    }
  }
  // Insert implicit-zero leading zeros.
  while (resultingInternal.size()*8 < memoryValueSize) {
      resultingInternal.push_back(0);
  }
  return MemoryValue(resultingInternal, memoryValueSize);
}


MemoryValue unsignedDecStringToMemoryValue(std::string stringValue,
                                           size_t memoryValueSize) {
  long long intermediateValue = std::stoll(stringValue, nullptr, 10);
  MemoryValue
      result;// TODO: Use _unsigned_ conversion from int64_t to memoryValue
  return result;
}


MemoryValue
signedDecStringToMemoryValue(std::string stringValue, size_t memoryValueSize) {
  long long intermediateValue = std::stoll(stringValue, nullptr, 10);
  MemoryValue
      result;// TODO: Use _signed_ conversion from int64_t to memoryValue
  return result;
}


MemoryValue decimalFloatStringToMemoryValue(std::string stringValue) {
  double floatResult = std::stod(stringValue);
  MemoryValue result;// TODO: Use conversion from double to memoryValue
  return result;
}


template <typename T>
std::string toDecString(T intValue) {
  std::stringstream stream;
  stream << std::dec << intValue;
  std::string result(stream.str());
  return result;
}
}
