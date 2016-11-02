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
  /**
   * \brief struct used to convert 32bit floats to 32bit integer
   */
struct convert32_u;
/**
 * \brief struct used to convert 64bit floats to 64bit integer
 */
struct convert64_u;
/**
 * \brief struct used to convert 128bit floats to 128bit integer
 */
struct convert128_u;
}
namespace floatConversions {
/**
 * \brief converts a 32bit float to a MemoryValue
 * \param value the value to be converted
 * \return 32bit MemoryValue representing value
 */
MemoryValue convert(float32_t value);
/**
 * \brief converts a 64bit float to a MemoryValue
 * \param value the value to be converted
 * \return 64bit MemoryValue representing value
 */
MemoryValue convert(float64_t value);
/**
 * \brief converts a 128bit float to a MemoryValue
 * \param value the value to be converted
 * \return 128bit MemoryValue representing value
 */
MemoryValue convert(float128_t value);
/**
 * \brief converts a 32bit MemoryValue to a float
 * \param value the value to be converted
 * \return 32bit float representing value
 */
float32_t convert32f(MemoryValue value);
/**
 * \brief converts a 64bit MemoryValue to a float
 * \param value the value to be converted
 * \return 64bit float representing value
 */
float64_t convert64f(MemoryValue value);
/**
 * \brief converts a 128bit MemoryValue to a float
 * \param value the value to be converted
 * \return 128bit float representing value
 */
float128_t convert128f(MemoryValue value);
}
}

#endif// ERAGPSIM_CORE_STANDARD_FLOAT_CONVERSIONS_HPP_
