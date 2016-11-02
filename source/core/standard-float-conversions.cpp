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

#include "core/standard-float-conversions.hpp"

namespace conversions {
namespace detail {
struct convert32_u {
  union {
    std::uint32_t intRepresentation;
    float32_t floatRepresentation;
  } data{};
  convert32_u(std::uint32_t intRepresentation) {
    data.intRepresentation = intRepresentation;
  }
  convert32_u(float32_t floatRepresentation) {
    data.floatRepresentation = floatRepresentation;
  }
};

struct convert64_u {
  union {
    std::uint64_t intRepresentation;
    float64_t floatRepresentation;
  } data{};
  convert64_u(std::uint64_t intRepresentation) {
    data.intRepresentation = intRepresentation;
  }
  convert64_u(float64_t floatRepresentation) {
    data.floatRepresentation = floatRepresentation;
  }
};

// We dont have a 128 bit int, so this is a bit more complicated
struct convert128_u {
  union {
    std::uint64_t intRepresentation[2];
    float128_t floatRepresentation;
  } data{};
  convert128_u(
      const std::pair<std::uint64_t, std::uint64_t> &intRepresentation) {
    data.intRepresentation[0] = intRepresentation.first;
    data.intRepresentation[1] = intRepresentation.second;
  }
  convert128_u(std::uint64_t first, std::uint64_t second) {
    data.intRepresentation[0] = first;
    data.intRepresentation[1] = second;
  }
  convert128_u(float128_t floatRepresentation) {
    data.floatRepresentation = floatRepresentation;
  }
  std::pair<std::uint64_t, std::uint64_t> getInt() {
    return std::make_pair(data.intRepresentation[0], data.intRepresentation[1]);
  }
};
}
namespace floatConversions {
MemoryValue convert(float32_t value) {
  detail::convert32_u conv{value};
  return conversions::convert<std::uint32_t>(
      conv.data.intRepresentation,
      conversions::standardConversions::nonsigned,
      32);
}

MemoryValue convert(float64_t value) {
  detail::convert64_u conv{value};
  return conversions::convert<std::uint64_t>(
      conv.data.intRepresentation,
      conversions::standardConversions::nonsigned,
      64);
}

MemoryValue convert(float128_t value) {
  detail::convert128_u conv{value};
  auto pair = conv.getInt();
  auto lower = conversions::convert<std::uint64_t>(
      pair.first, conversions::standardConversions::nonsigned, 64);
  auto upper = conversions::convert<std::uint64_t>(
      pair.second, conversions::standardConversions::nonsigned, 64);
  MemoryValue ret{128};
  ret.write(lower, 0);
  ret.write(upper, 64);
  return ret;
}

float32_t convert32f(MemoryValue value) {
  std::uint32_t intconv =
      conversions::detail::convertUnsigned<std::uint32_t>(value);
  detail::convert32_u conv{intconv};
  return conv.data.floatRepresentation;
}

float64_t convert64f(MemoryValue value) {
  std::uint64_t intconv =
      conversions::detail::convertUnsigned<std::uint64_t>(value);
  detail::convert64_u conv{intconv};
  return conv.data.floatRepresentation;
}

float128_t convert128f(MemoryValue value) {
  auto lower = value.subSet(0, 64);
  auto upper = value.subSet(64, 128);
  std::uint64_t lowInt =
      conversions::detail::convertUnsigned<std::uint64_t>(lower);
  std::uint64_t uppInt =
      conversions::detail::convertUnsigned<std::uint64_t>(upper);
  detail::convert128_u conv{lowInt, uppInt};
  return conv.data.floatRepresentation;
}
}
}
