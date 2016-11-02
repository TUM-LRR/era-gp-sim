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
#ifndef ERAGPSIM_CORE_STANDARD_FLOAT_CONVERSIONS_HPP_
#define ERAGPSIM_CORE_STANDARD_FLOAT_CONVERSIONS_HPP_

#include <cstdint>
#include <utility>

#include "core/conversions.hpp"
#include "core/memory-value.hpp"

//#if (sizeof(float)=4)
typedef float float32_t;
//#endif
typedef double float64_t;
typedef long double float128_t;

namespace conversions {
namespace detail {
struct convert32_u;
struct convert64_u;
struct convert128_u;
}
namespace floatConversions {
MemoryValue convert(float32_t value);
MemoryValue convert(float64_t value);
MemoryValue convert(float128_t value);
float32_t convert32f(MemoryValue value);
float64_t convert64f(MemoryValue value);
float128_t convert128f(MemoryValue value);
}
}

#endif// ERAGPSIM_CORE_STANDARD_FLOAT_CONVERSIONS_HPP_
