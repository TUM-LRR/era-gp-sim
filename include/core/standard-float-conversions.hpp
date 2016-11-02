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

#include <cstdint>

#include "core/conversions.hpp"
#include "core/memory-value.hpp"

//#if (sizeof(float)=4)
typedef float float32_t;
//#endif
typedef double float64_t;
typedef long double float128_t;

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

MemoryValue convert(float32_t value) {
  convert32_u conv{value};
  return conversions::convert<std::uint32_t>(
      conv.data.intRepresentation,
      conversions::standardConversions::nonsigned,
      32);
}

MemoryValue convert(float64_t value) {
  convert64_u conv{value};
  return conversions::convert<std::uint64_t>(
      conv.data.intRepresentation,
      conversions::standardConversions::nonsigned,
      64);
}

float64_t convert64f(MemoryValue value) {
  std::uint64_t intconv =
      conversions::detail::convertUnsigned<std::uint64_t>(value);
  convert64_u conv{intconv};
  return conv.data.floatRepresentation;
}

float32_t convert32f(MemoryValue value) {
  std::uint32_t intconv =
      conversions::detail::convertUnsigned<std::uint32_t>(value);
  convert32_u conv{intconv};
  return conv.data.floatRepresentation;
}
