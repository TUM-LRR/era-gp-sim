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
const std::string Memory::_lineStringIdentifier = "memory_line";
const std::string Memory::_dataMapStringIdentifier = "memory_map";

Memory::Memory() : Memory(64, 8) {
}

Memory::Memory(std::size_t byteCount, std::size_t byteSize)
: _byteCount{byteCount}, _byteSize{byteSize}, _data{byteCount * byteSize} {
  assert::that(byteCount > 0);
  assert::that(byteSize > 0);
}

Memory::Memory(const Json& json)
: Memory(*json.find(_byteCountStringIdentifier),
         *json.find(_byteSizeStringIdentifier)) {
  deserializeJSON(json);
}

std::size_t Memory::getByteSize() const {
  return _byteSize;
}

std::size_t Memory::getByteCount() const {
  return _byteCount;
}

void Memory::setCallback(
    const std::function<void(const std::size_t, const std::size_t)>& callback) {
  _callback = callback;
}

MemoryValue
Memory::get(const std::size_t address, const std::size_t amount) const {
  assert::that(address >= 0);
  assert::that(amount >= 0);
  if (address + amount <= _byteCount) {
    return _data.subSet(address * _byteSize, (address + amount) * _byteSize);
  } else {
    MemoryValue value =
        _data.subSet(address * _byteSize, (address + amount) * _byteSize);
    MemoryValue ret = MemoryValue(amount * _byteSize);
    ret.write(value, 0);
    return ret;
  }
}

void Memory::put(const std::size_t address, const MemoryValue& value) {
  assert::that(address >= 0);
  assert::that(value.getSize() % _byteSize == 0);
  const std::size_t amount{value.getSize() / _byteSize};
  assert::that(amount >= 0);
  if (address + amount <= _byteCount) {
    _data.write(value, address * _byteSize);
  } else {
    _data.write(value.subSet(0, _byteSize * (_byteCount - address)),
                address * _byteSize);
  }
}

MemoryValue Memory::set(const std::size_t address, const MemoryValue& value) {
  const std::size_t amount{value.getSize() / _byteSize};
  MemoryValue prev{get(address, amount)};
  put(address, value);
  return prev;
}

void Memory::_appendMemoryValue(std::stringstream& strm,
                                const MemoryValue& value) {
  static const char hex[] = "0123456789ABCDEF";
  bool zero = true;
  // reversely iterate over all the bytes in value (high->low)
  for (std::size_t l = (value.getSize() + 7) / 8; l-- > 0;) {
    std::uint8_t byte = value.getByteAt(l * 8);
    std::uint8_t upperChar = byte / 16;
    std::uint8_t lowerChar = byte % 16;
    // do not print if this is the first character && it's 0
    if (!zero || upperChar != 0) {
      strm.put(hex[upperChar]);
      zero = false;
    }
    // do not print if this is the first character && it's 0
    if (!zero || lowerChar != 0) {
      strm.put(hex[lowerChar]);
      zero = false;
    }
  }
  // if no characters have been printed print 0
  if (zero) {
    strm.put('0');
  }
}

std::pair<std::map<std::string, std::size_t>,
          std::map<std::string, std::string>>
Memory::_serializeRaw(char separator, std::size_t lineLength) const {
  if (lineLength > _byteCount) {
    lineLength = _byteCount;
  }
  std::map<std::string, std::string> data{};
  // fill map with info about the memory
  std::map<std::string, std::size_t> meta{
      {_byteCountStringIdentifier, _byteCount},
      {_byteSizeStringIdentifier, _byteSize},
      {_lineLengthStringIdentifier, lineLength}};
  const std::size_t lineCount = (_byteCount + lineLength - 1) / lineLength;
  // empty MemoryValue to compare others to
  const MemoryValue empty{_byteSize * lineLength};
  // iterate over all lines in memory
  for (std::size_t i = 0; i < lineCount; ++i) {
    // if this line == 0x00 do not do anything at all
    if (get(i * lineLength, lineLength) != empty) {
      // the key is _lineStringIdentifier and the cell address
      std::stringstream name{};
      name << _lineStringIdentifier;
      name << (i * lineLength);
      // iterate over all the cells in a line to get the value
      std::stringstream value{};
      for (std::size_t j = 0; j < lineLength; ++j) {
        MemoryValue v{get(i * lineLength + j)};
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
                            std::size_t lineLength) const {
  auto data = _serializeRaw(separator, lineLength);
  // json["meta"]=data.first;
  json[_dataMapStringIdentifier] = data.second;
  for (auto i : data.first) {
    json[i.first] = i.second;
  }
  // for (auto i : data.second) {
  //   json[i.first] = i.second;
  // }
  json[_separatorStringIdentifier] = separator;
  return json;
}

Json Memory::serializeJSON(Json&& json,
                           char separator,
                           std::size_t lineLength) const {
  return serializeJSON(json, separator, lineLength);
}

MemoryValue Memory::_deserializeLine(const std::string& line,
                                     std::size_t byteSize,
                                     std::size_t lineLength,
                                     char separator) {
  static const std::unordered_map<char, std::uint8_t> reverseHexMap{
      {'0', 0},  {'1', 1},  {'2', 2},  {'3', 3},  {'4', 4},  {'5', 5},
      {'6', 6},  {'7', 7},  {'8', 8},  {'9', 9},  {'a', 10}, {'b', 11},
      {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}, {'A', 10}, {'B', 11},
      {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15},
  };
  MemoryValue ret{byteSize * lineLength};
  std::size_t byte = lineLength;
  std::size_t bit = 0;
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
      for (std::size_t j = 0; j < 4; ++j) {
        if (hex % 2 == 1) {
          if (bit >= byteSize) {
            throw DeserializationError(
                "Could not deserialize Memory: There is a byte > max(byte)");
          }
          ret.put(byte * byteSize + bit);
        }
        ++bit;    // increment bit counter
        hex >>= 1;// could make this faster using long switch case
      }
    }
    // TODO::make actual useful assert
    assert::that(byte >= 0);
  }
  return ret;
}

void Memory::deserializeJSON(const Json& json) {
  // TODO::look into making this const to not unnecessarily copy stuff
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
  std::size_t byteCount = *byteCountIt;
  if (_byteCount != byteCount) {
    std::string expected = std::to_string(_byteCount);
    std::size_t byteCount = *byteCountIt;
    std::string recieved = std::to_string(byteCount);
    throw DeserializationError(
        "Could not deserialize Memory: ByteCount did not match: expected \"" +
        expected + "\" to be equal to \"" + recieved + "\"");
  }
  std::size_t byteSize = *byteSizeIt;
  if (_byteSize != byteSize) {
    std::string expected = std::to_string(_byteSize);
    std::string recieved = std::to_string(byteSize);
    throw DeserializationError(
        "Could not deserialize Memory: ByteSize did not match: expected \"" +
        expected + "\" to be equal to \"" + recieved + "\"");
  }
  std::size_t lineLength = json[_lineLengthStringIdentifier].get<std::size_t>();
  if (lineLength <= 0) {
    throw DeserializationError("Could not deserialize Memory: lineLength == 0");
  }
  const std::size_t lineCount = (_byteCount + lineLength - 1) / lineLength;
  // TODO::find separator
  const char separator = json[_separatorStringIdentifier].get<char>();
  auto data = *dataMapIt;
  // iterate over all lines
  for (std::size_t i = 0; i < lineCount; ++i) {
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
      put(i * lineLength, value);
    }
  }
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
  _wasUpdated(0, _byteCount);
}

void Memory::_wasUpdated(const std::size_t address, const std::size_t amount) {
  _callback(address, amount);
}
