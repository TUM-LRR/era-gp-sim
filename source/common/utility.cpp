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

#include <algorithm>
#include <string>

#include "common/utility.hpp"
#include "core/memory-value.hpp"

namespace Utility {

namespace Detail {
bool performSignedBitWidthCheck(const MemoryValue& memory,
                                std::size_t numberOfBits) {
  // Get the sign bit
  bool sign = memory.get(memory.getSize() - 1);

  // When the value is signed, all bits after the sign bit should be
  // equal to the sign bit. That is, for unsigned values all bits after
  // the sign bit should be zero and for signed values all should be one.
  // As such, we will start checking the bits at the sign bit of the immediate
  // (i.e. where we want the sign bit to be) and move up to one bit before the
  // sign bit of the memory value.
  auto range = Utility::range(numberOfBits - 1, memory.getSize() - 1);

  // clang-format off
  return Utility::anyOf(range, [&memory, sign] (auto index) {
    return memory[index] != sign;
  });
  // clang-format on
}

bool performUnsignedBitWidthCheck(const MemoryValue& memory,
                                  std::size_t numberOfBits) {
  // clang-format off
  auto range = Utility::range(numberOfBits, memory.getSize());
  return Utility::anyOf(range, [&memory](auto index) {
    return memory[index] == 1;
  });
  // clang-format on
}
}

bool occupiesMoreBitsThan(const MemoryValue& memory,
                          size_t numberOfBits,
                          bool isSigned) {
  if (numberOfBits == 0) return true;
  if (memory.getSize() <= numberOfBits) return false;

  if (isSigned) {
    return Detail::performSignedBitWidthCheck(memory, numberOfBits);
  } else {
    return Detail::performUnsignedBitWidthCheck(memory, numberOfBits);
  }
}

std::string toLower(const std::string& string) {
  return transform(string, [](auto& c) { return std::tolower(c); });
}
std::string toUpper(const std::string& string) {
  return transform(string, [](auto& c) { return std::toupper(c); });
}

std::string rootPath() {
  static const std::string query("era-gp-sim");
  static std::string root;

  if (root.empty()) {
    // Hope it exists
    root = __FILE__;

    auto index = root.rfind(query) + query.length();
    root.erase(index);
  }

  return root;
}

std::string joinPaths(const std::string& single) {
  return single;
}

std::string loadFromFile(const std::string& filePath) {
  std::string input;
  std::ifstream file(filePath);

  assert::that(static_cast<bool>(file));

  std::copy(std::istreambuf_iterator<char>{file},
            std::istreambuf_iterator<char>{},
            std::back_inserter(input));

  return input;
}
}
