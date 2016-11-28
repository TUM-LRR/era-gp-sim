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

#ifndef ERAGPSIM_CORE_DESERIALIZATION_ERROR_HPP_
#define ERAGPSIM_CORE_DESERIALIZATION_ERROR_HPP_

#include <stdexcept>
#include <string>

struct DeserializationError : public std::runtime_error {
  using super = std::runtime_error;

  /**
   * Constructs an AssertionError from a string object
   *
   * \param what A message describing the cause of the DeserializationError.
   */
  explicit DeserializationError(const std::string& what) : super(what) {
  }

  /**
   * Constructs an AssertionError from a string literal
   *
   * \param what A message describing the cause of the DeserializationError.
   */
  explicit DeserializationError(const char*& what) : super(what) {
  }
};

#endif// ERAGPSIM_CORE_DESERIALIZATION_ERROR_HPP_
