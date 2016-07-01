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

#ifndef ERAGPSIM_ARCH_COMMON_INFORMATION_HPP
#define ERAGPSIM_ARCH_COMMON_INFORMATION_HPP

#include "third-party/json/json.hpp"

/**
 * The base class for all information classes.
 *
 * Defines information classes' interface w.r.t to deserialization from JSON.
 */
class Information {
 public:
  using Format = nlohmann::json;

  /**
   * Destructor.
   */
  virtual ~Information();

  /**
   * Deserializes an `Information` object from the given data.
   *
   * @param data The data to deserialize from.
   *
   * @return The current `Information` object.
   */
  virtual Information& operator<<(const Format& data);

  /**
   * Deserializes an `Information` object from the given data.
   *
   * @param data The data to deserialize from.
   *
   * @return The data after deserialization.
   */
  friend Format& operator>>(Format& data, Information& information);

 protected:
  virtual void _deserialize(const Format& data) = 0;
};

#endif /* ERAGPSIM_ARCH_COMMON_INFORMATION_HPP */
