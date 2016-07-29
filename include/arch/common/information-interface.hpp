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

#include <string>

#include "common/builder-interface.hpp"
#include "third-party/json/json.hpp"


/**
 * The base class for all information classes.
 *
 * Defines information classes' interface w.r.t to deserialization from JSON.
 * As all information objects support the builder pattern and builder interface,
 * this class inherits from the `BuilerInterface` class.
 */
class InformationInterface : public BuilderInterface {
 public:
  using Format = nlohmann::json;

  /**
   * Loads data in the InformationInterface's Format from the given file path.
   *
   * The information interface defines the format, so it's a good idea to leave
   * this file here and not in the `Utility` namespace.
   *
   * \param filePath The file path to load data from.
   *
   * \return The contained data in the given format.
   */
  static Format load(const std::string& filePath);

  /**
   * Stores datain the InformationInterface's Format at the given file path.
   *
   * \param filePath The file path to store data at.
   * \param data The data to store.
   */
  static void store(const std::string& filePath, const Format& data);

  /**
   * Destructor.
   */
  virtual ~InformationInterface();

  /**
   * Deserializes an `InformationInterface` object from the given data.
   *
   * \param data The data to deserialize from.
   *
   * \return The current `InformationInterface` object.
   */
  virtual void operator<<(Format& data);

  /**
   * Deserializes an `InformationInterface` object from the given data.
   *
   * \param data The data to deserialize from.
   *
   * \return The data after deserialization.
   */
  friend Format& operator>>(Format& data, InformationInterface& information);

 protected:
  /**
   * This method should handle the actual deserialization workload.
   *
   * Other methods, such as the shift overloads or public deserialize() methods
   * should call this function to unify the deserialization in one point.
   *
   * \param data The data to deserialize.
   *
   */
  virtual void _deserialize(Format& data) = 0;
};

#endif /* ERAGPSIM_ARCH_COMMON_INFORMATION_HPP */
