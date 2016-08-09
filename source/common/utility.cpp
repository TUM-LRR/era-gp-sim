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

namespace Utility {

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

  assert(static_cast<bool>(file));

  std::copy(std::istreambuf_iterator<char>{file},
            std::istreambuf_iterator<char>{},
            std::back_inserter(input));

  return input;
}
}
