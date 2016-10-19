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

#include "sstream"

#include "common/assert.hpp"
#include "core/memory.hpp"

Memory::Memory() : Memory(64, 8) {
}

Memory::Memory(std::size_t byteCount, std::size_t byteSize)
: _byteCount{byteCount}, _byteSize{byteSize}, _data{byteCount * byteSize} {
  assert::that(byteCount > 0);
  assert::that(byteSize > 0);
}

// evtl use STOI
Memory::Memory(const nlohmann::json& jsonObj, char separator)
: Memory(jsonObj["byteCount"], jsonObj["byteSize"]) {
  deserializeJSON(jsonObj, separator);
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

MemoryValue Memory::get(const std::size_t address, const std::size_t amount) {
  assert::that(address >= 0);
  assert::that(amount >= 0);
  assert::that(address + amount <= _byteCount);
  return _data.subSet(address * _byteSize, (address + amount) * _byteSize);
}

void Memory::put(const std::size_t address, const MemoryValue& value) {
  assert::that(address >= 0);
  assert::that(value.getSize() % _byteSize == 0);
  const std::size_t amount{value.getSize() / _byteSize};
  assert::that(amount >= 0);
  assert::that(address + amount <= _byteCount);
  _data.write(value, address * _byteSize);
}

MemoryValue Memory::set(const std::size_t address, const MemoryValue& value) {
  const std::size_t amount{value.getSize() / _byteSize};
  MemoryValue prev{get(address, amount)};
  put(address, value);
  return prev;
}

// maybe use map, map would be soooo much nicer
std::vector<std::pair<std::string, std::string>>
Memory::serializeRaw(char separator, std::size_t lineLength) {
  static const char hex[] = "0123456789ABCDEF";
  std::vector<std::pair<std::string, std::string>> ret{};
  ret.push_back(std::make_pair("byteCount", std::to_string(_byteCount)));
  ret.push_back(std::make_pair("byteSize", std::to_string(_byteSize)));
  ret.push_back(std::make_pair("lineLength", std::to_string(lineLength)));
  const std::size_t lineCount = _byteCount / lineLength;
  const MemoryValue empty{_byteSize * lineLength};
  for (std::size_t i = 0; i < lineCount; ++i) {
    if (get(i * lineLength, lineLength) != empty) {
      std::stringstream name("line");
      std::stringstream value{};
      name << (i * lineLength);
      for (std::size_t j = 0; j < lineLength; ++j) {
        MemoryValue v{get(i * lineLength + j)};
        bool zero = true;
        for (std::size_t l = (_byteSize + 7) / 8; l-- > 0;) {
          std::uint8_t byte = v.getByteAt(l * 8);
          std::uint8_t upperChar = byte / 16;
          std::uint8_t lowerChar = byte % 16;
          if (!zero || upperChar != 0) {
            value.put(hex[upperChar]);
            zero = false;
          }
          if (!zero || lowerChar != 0) {
            value.put(hex[lowerChar]);
            zero = false;
          }
        }
        value.put(separator);
      }
      ret.push_back(std::make_pair(name.str(), value.str()));
    }
  }
  // TODO::serialize last line
  return ret;
}

nlohmann::json& Memory::serializeJSON(nlohmann::json& jsonObj,
                                      char separator,
                                      std::size_t lineLength) {
  auto data = serializeRaw(separator, lineLength);
  for (auto& p : data) {
    jsonObj[p.first] = p.second;
  }
  return jsonObj;
}

nlohmann::json Memory::serializeJSON(nlohmann::json&& jsonObj,
                                     char separator,
                                     std::size_t lineLength) {
  return serializeJSON(jsonObj, separator, lineLength);
}

void Memory::wasUpdated(const std::size_t address, const std::size_t amount) {
  _callback(address, amount);
}

void Memory::deserializeJSON(const nlohmann::json& jsonObj, char separator) {
  assert::that(_byteCount == jsonObj["byteCount"]);
  assert::that(_byteSize == jsonObj["byteSize"]);
  std::size_t lineLength = jsonObj["lineLength"];
  const std::size_t lineCount = _byteCount / lineLength;
  const MemoryValue empty{_byteSize * lineLength};
  for (std::size_t i = 0; i < lineCount; ++i) {
    std::stringstream name("line");
    name << (i * lineLength);
    auto line = jsonObj.find(name.str());
    // TODO::make line to MemoryValue
  }
}
