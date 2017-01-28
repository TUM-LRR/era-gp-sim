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

#include <algorithm>
#include <set>
#include <sstream>
#include <unordered_map>

#include "common/assert.hpp"
#include "core/deserialization-error.hpp"
#include "core/memory.hpp"

using Json = nlohmann::json;

const std::string Memory::_byteCountStringIdentifier = "memory_byteCount";
const std::string Memory::_byteSizeStringIdentifier = "memory_byteSize";
const std::string Memory::_lineLengthStringIdentifier = "memory_lineLength";
const std::string Memory::_separatorStringIdentifier = "memory_separator";
const std::string Memory::_lineStringIdentifier = "memory_line_";
const std::string Memory::_dataMapStringIdentifier = "memory_map";

Memory::Memory() : Memory(64, 8) {
}

Memory::Memory(size_t byteCount, size_t byteSize)
: _byteCount{byteCount}
, _byteSize{byteSize}
, _data{byteCount * byteSize}
, _callback{[](const size_t, const size_t) {}} {
  assert::that(byteCount > 0);
  assert::that(byteSize > 0);
}

Memory::Memory(const Json& json)
: Memory(*json.find(_byteCountStringIdentifier),
         *json.find(_byteSizeStringIdentifier)) {
  deserializeJSON(json);
}

Memory::size_t Memory::getByteSize() const {
  return _byteSize;
}

Memory::size_t Memory::getByteCount() const {
  return _byteCount;
}

void Memory::setCallback(const UpdateCallback& callback) {
  _callback = callback;
}

void Memory::setSizeCallback(const SizeCallback& callback) {
  _sizeCallback = callback;
}

MemoryValue Memory::_get(size_t address, size_t amount) const {
  return _data.subSet(address * _byteSize, (address + amount) * _byteSize);
}

MemoryValue Memory::get(size_t address, size_t amount) const {
  assert::that(address >= 0);
  assert::that(amount >= 0);
  assert::that(address + amount <= _byteCount);
  return _get(address, amount);
}

MemoryValue Memory::tryGet(size_t address, size_t amount) const {
  MemoryValue result{amount * _byteSize};
  amount = std::min(_byteCount - address, amount);
  if (amount > 0) {
    MemoryValue data = _get(address, amount);
    result.write(data, 0);
  }
  return result;
}

void Memory::_put(size_t address,
                  const MemoryValue& value,
                  size_t amount,
                  bool ignoreProtection) {
  if (!ignoreProtection && isProtected(address, amount)) {
    // do not write anything
    return;
  }
  _data.write(value, address * _byteSize);
  _wasUpdated(address, amount);
}

void Memory::put(size_t address,
                 const MemoryValue& value,
                 bool ignoreProtection) {
  assert::that(address >= 0);
  assert::that(value.getSize() % _byteSize == 0);
  const size_t amount{value.getSize() / _byteSize};
  assert::that(amount >= 0);
  assert::that(address + amount <= _byteCount);
  _put(address, value, amount, ignoreProtection);
}

void Memory::tryPut(size_t address,
                    const MemoryValue& value,
                    bool ignoreProtection) {
  size_t amount{value.getSize() / _byteSize};
  if (!ignoreProtection && isProtected(address, amount)) {
    // do not write anything
    return;
  }
  amount = std::min(amount, _byteCount - address);
  MemoryValue valueInsert = value.subSet(0, amount * _byteSize);
  _put(address, valueInsert, amount, ignoreProtection);
  _wasUpdated(address, amount);
}

MemoryValue
Memory::_set(size_t address, const MemoryValue& value, bool ignoreProtection) {
  size_t amount{value.getSize() / _byteSize};
  MemoryValue prev{_get(address, amount)};
  _put(address, value, amount, ignoreProtection);
  return prev;
}

MemoryValue
Memory::set(size_t address, const MemoryValue& value, bool ignoreProtection) {
  size_t amount{value.getSize() / _byteSize};
  MemoryValue prev{get(address, amount)};
  put(address, value, ignoreProtection);
  return prev;
}

MemoryValue Memory::trySet(size_t address,
                           const MemoryValue& value,
                           bool ignoreProtection) {
  size_t amount{value.getSize() / _byteSize};
  MemoryValue prev{tryGet(address, amount)};
  tryPut(address, value, ignoreProtection);
  return prev;
}

void Memory::_appendMemoryValue(std::stringstream& strm,
                                const MemoryValue& value) {
  std::string s = value.toHexString(false, true);
  strm << s;
}

Memory::RawMapPair
Memory::_serializeRaw(char separator, size_t lineLength) const {
  if (lineLength > _byteCount) {
    lineLength = _byteCount;
  }
  std::map<std::string, std::string> data{};
  // fill map with info about the memory
  std::map<std::string, size_t> meta{{_byteCountStringIdentifier, _byteCount},
                                     {_byteSizeStringIdentifier, _byteSize},
                                     {_lineLengthStringIdentifier, lineLength}};
  const size_t lineCount = (_byteCount + lineLength - 1) / lineLength;
  // empty MemoryValue to compare others to
  const MemoryValue empty{_byteSize * lineLength};
  // iterate over all lines in memory
  for (size_t i = 0; i < lineCount; ++i) {
    // if this line == 0x00 do not do anything at all
    if (tryGet(i * lineLength, lineLength) != empty) {
      // the key is _lineStringIdentifier and the cell address
      std::stringstream name{};
      name << _lineStringIdentifier;
      name << (i * lineLength);
      // iterate over all the cells in a line to get the value
      std::stringstream value{};
      for (size_t j = 0; j < lineLength; ++j) {
        MemoryValue v{tryGet(i * lineLength + j)};
        _appendMemoryValue(value, v);
        value.put(separator);
      }
      data[name.str()] = value.str();
    }
  }
  return make_pair(std::move(meta), std::move(data));
}

Json& Memory::serializeJSON(Json& json,
                            char separator,
                            size_t lineLength) const {
  auto data = _serializeRaw(separator, lineLength);
  json[_dataMapStringIdentifier] = data.second;
  for (auto i : data.first) {
    json[i.first] = i.second;
  }
  json[_separatorStringIdentifier] = separator;
  return json;
}

Json Memory::serializeJSON(Json&& json,
                           char separator,
                           size_t lineLength) const {
  return serializeJSON(json, separator, lineLength);
}

MemoryValue Memory::_deserializeLine(const std::string& line,
                                     size_t byteSize,
                                     size_t lineLength,
                                     char separator) {
  static const std::unordered_map<char, std::uint8_t> reverseHexMap{
      {'0', 0},  {'1', 1},  {'2', 2},  {'3', 3},  {'4', 4},  {'5', 5},
      {'6', 6},  {'7', 7},  {'8', 8},  {'9', 9},  {'a', 10}, {'b', 11},
      {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}, {'A', 10}, {'B', 11},
      {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15},
  };
  MemoryValue ret{byteSize * lineLength};
  size_t byte = lineLength;
  size_t bit = 0;
  // iterate reversely over line
  for (auto i = line.crbegin(); i < line.crend(); ++i) {
    if (*i == separator) {
      if (byte == 0) {
        throw DeserializationError(
            "Could not deserialize Memory: There is a line longer than line "
            "length: \"" +
            line + "\"");
      }
      // if separator -> inc to next cell
      --byte;
      bit = 0;
    } else {
      // if hex hex into number
      auto h = reverseHexMap.find(*i);
      if (h == reverseHexMap.end()) {
        std::string badCharacter{*i};
        throw DeserializationError(
            "Could not deserialize Memory: Unexpected Character '" +
            badCharacter + "' in \"" + line + "\"");
      }
      std::uint8_t hex = h->second;
      // write all 4 bit of number into memoryvalue
      for (size_t j = 0; j < 4; ++j) {
        if (hex % 2 == 1) {
          if (bit >= byteSize) {
            throw DeserializationError(
                "Could not deserialize Memory: There is a byte > max(byte)");
          }
          ret.put(byte * byteSize + bit);
        }
        ++bit;      // increment bit counter
        hex >>= 1;  // could make this faster using long switch case
      }
    }
    // TODO::make actual useful assert
    assert::that(byte >= 0);
  }
  return ret;
}

void Memory::deserializeJSON(const Json& json) {
  const auto byteCountIt = json.find(_byteCountStringIdentifier);
  const auto byteSizeIt = json.find(_byteSizeStringIdentifier);
  const auto lineLengthIt = json.find(_lineLengthStringIdentifier);
  const auto separatorIt = json.find(_separatorStringIdentifier);
  const auto dataMapIt = json.find(_dataMapStringIdentifier);
  // check that everything exists
  if (byteCountIt == json.end()) {
    throw DeserializationError("Could not deserialize Memory: Could not find " +
                               _byteCountStringIdentifier);
  }
  if (byteSizeIt == json.end()) {
    throw DeserializationError("Could not deserialize Memory: Could not find " +
                               _byteSizeStringIdentifier);
  }
  if (lineLengthIt == json.end()) {
    throw DeserializationError("Could not deserialize Memory: Could not find " +
                               _lineLengthStringIdentifier);
  }
  if (separatorIt == json.end()) {
    throw DeserializationError("Could not deserialize Memory: Could not find " +
                               _separatorStringIdentifier);
  }
  if (dataMapIt == json.end()) {
    throw DeserializationError("Could not deserialize Memory: Could not find " +
                               _dataMapStringIdentifier);
  }
  // TODO::check types!
  // check that sizes match
  size_t byteCount = *byteCountIt;
  if (_byteCount < byteCount) {
    _byteCount = byteCount;
    _data = MemoryValue(byteCount * _byteSize);
    _sizeCallback(_byteCount);
  }
  size_t byteSize = *byteSizeIt;
  if (_byteSize != byteSize) {
    std::string expected = std::to_string(_byteSize);
    std::string recieved = std::to_string(byteSize);
    throw DeserializationError(
        "Could not deserialize Memory: ByteSize did not match: expected \"" +
        expected + "\" to be equal to \"" + recieved + "\"");
  }
  size_t lineLength = json[_lineLengthStringIdentifier].get<size_t>();
  if (lineLength <= 0) {
    throw DeserializationError("Could not deserialize Memory: lineLength == 0");
  }

  // clear the current memory to write the zeros in the serialized data
  clear();

  const size_t lineCount = (_byteCount + lineLength - 1) / lineLength;
  const char separator = json[_separatorStringIdentifier].get<char>();
  auto data = *dataMapIt;
  // iterate over all lines
  for (size_t i = 0; i < lineCount; ++i) {
    // the key is _lineStringIdentifier and the cell address
    std::stringstream name{};
    name << _lineStringIdentifier;
    name << (i * lineLength);
    // find key
    auto lineIterator = data.find(name.str());
    // auto lineIterator = json.find(name.str());
    // if key exists
    if (lineIterator != data.end()) {
      // deserialize Line
      MemoryValue value =
          _deserializeLine(*lineIterator, _byteSize, lineLength, separator);
      // write line
      tryPut(i * lineLength, value);
    }
  }
  _wasUpdated();
}

bool Memory::operator==(const Memory& other) const {
  return _byteSize == other._byteSize && _byteCount == other._byteCount &&
         _data == other._data;
}

std::ostream& operator<<(std::ostream& stream, const Memory& value) {
  return stream << value._byteCount << " * " << value._byteSize << "; "
                << value._data;
}

void Memory::clear() {
  _data.clear();
  _wasUpdated();
}

void Memory::_wasUpdated() const {
  _wasUpdated(0, _byteCount);
}

void Memory::_wasUpdated(size_t address, size_t amount) const {
  _callback(address, amount);
}

bool Memory::isProtected(size_t address, size_t amount) const {
  // search for any pair overlapping with the area
  auto it = _protection.lower_bound(address);
  if (it != _protection.begin()) --it;
  for (; it != _protection.end() && it->first <= address + amount; it++) {
    if (_overlaps(it->first, it->second, address, amount, false)) {
      return true;
    }
  }
  return false;
}

bool Memory::_overlaps(size_t protectionBegin,
                       size_t protectionEnd,
                       size_t address,
                       size_t amount,
                       bool equal) const {
  return protectionBegin < address + amount &&
         (equal ? (protectionEnd >= address) : (protectionEnd > address));
}

void Memory::makeProtected(size_t address, size_t amount) {
  if (!isProtected(std::min(address - 1, address), amount + 1)) {
    // naively insert into map
    _protection.insert(std::make_pair(address, address + amount));
  } else {
    // find all conflicting pairs
    std::vector<std::pair<size_t, size_t>> conflictList{};
    for (const auto& pair : _protection) {
      if (_overlaps(pair.first, pair.second, address, amount, true)) {
        conflictList.push_back(pair);
      }
    }
    // delete all others and gather extremes
    size_t min = address;
    size_t max = address + amount;
    for (const auto& pair : conflictList) {
      min = (pair.first < min) ? pair.first : min;
      max = (pair.second > max) ? pair.second : max;
      _protection.erase(pair.first);
    }
    _protection.insert(std::make_pair(min, max));
  }
}

void Memory::removeProtection(size_t address, size_t amount) {
  // maybe I should use a Linked List instead
  if (!isProtected(std::min(address - 1, address), amount + 1)) {
  } else {
    // find all conflicting pairs
    std::vector<std::pair<size_t, size_t>> conflictList{};
    for (const auto& pair : _protection) {
      if (_overlaps(pair.first, pair.second, address, amount, true)) {
        conflictList.push_back(pair);
      }
    }
    // delete all others and gather extremes
    size_t min = address;
    size_t max = address + amount;
    for (const auto& pair : conflictList) {
      min = (pair.first < min) ? pair.first : min;
      max = (pair.second > max) ? pair.second : max;
      _protection.erase(pair.first);
    }
    if (min < address) {
      _protection.insert(std::make_pair(min, address));
    }
    if (max > address + amount) {
      _protection.insert(std::make_pair(address + amount, max));
    }
  }
}

void Memory::removeAllProtection() {
  _protection.clear();
}
