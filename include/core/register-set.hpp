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

#ifndef ERAGPSIM_CORE_REGISTERSET_HPP
#define ERAGPSIM_CORE_REGISTERSET_HPP

#include <cstddef>
#include <functional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/assert.hpp"
#include "core/memory-value.hpp"
#include "core/register-id.hpp"

class RegisterSet {
 public:
  /**
   * \brief Default constructor. Constructs an empty RegisterSet
   */
  RegisterSet();
  /**
   * \brief Constructs an empty RegisterSet with an unordered-map with a
   *        bucketCount of bucketCount
   * \param bucketCount bucketCount of the unordered_map
   */
  RegisterSet(const std::size_t bucketCount);
  /**
   * \brief Move constructor. Constructs the RegisterSet with the contents of
   *        other using move semantics.
   * \param other another RegisterSet to be used as source to initialize the
   *        elements of the RegisterSet with
   */
  RegisterSet(RegisterSet &&other) = default;
  /**
  * \brief Move constructor. Constructs the RegisterSet with the contents of
  *        other using move semantics.
  * \param other another RegisterSet to be used as source to initialize the
  *        elements of the RegisterSet with
   */
  RegisterSet &operator=(RegisterSet &&other) = default;
  /**
   * \brief Copy constructor. Constructs the RegisterSet with the copy of the
   *        contents of other.
   * \param other another RegisterSet to be used as source to initialize the
   *        elements of the RegisterSet with
   */
  RegisterSet(const RegisterSet &other) = default;
  /**
   * \brief Copy constructor. Constructs the RegisterSet with the copy of the
   *        contents of other.
   * \param other another RegisterSet to be used as source to initialize the
   *        elements of the RegisterSet with
   */
  RegisterSet &operator=(const RegisterSet &other) = default;
  /**
   * \brief Destructs the RegisterSet
   */
  ~RegisterSet() = default;

  /**
   * \brief Sets the callback to notify the gui about changes in the data
   * \param callback the callback to be set as _callback
   */
  void setCallback(const std::function<void(const std::string &)> &);

  /**
   * \brief Returns a MemoryValue holding the data stored in the Register with
   *        the name name
   * \param name String uniquely representing the Register
   * \returns MemoryValue holding the data stored in the Register with the name
   *          name
   */
  MemoryValue get(const std::string &name) const;
  /**
   * \brief Writes value into the Register with the name name
   * \param name String uniquely representing the Register
   * \param value Value to write
   */
  void put(const std::string &name, const MemoryValue &value);
  /**
   * \brief Writes value into the Register with the name name and returns its
   *        previous value
   * \param name String uniquely representing the Register
   * \param value Value to write
   * \returns previous value of the Register with the name name
   */
  MemoryValue set(const std::string &name, const MemoryValue &value);

  /**
   * \brief Creates a Register with the name name and size size
   * \param name String uniquely representing the to be created Register
   * \param size Size of the Register in bit
   */
  void createRegister(const std::string &name, const std::size_t size);
  /**
  * \brief Creates a Register with the name name and stores a copy of value
  * \param name String uniquely representing the to be created Register
  * \param value Initial value of the register
   */
  void createRegister(const std::string &name, const MemoryValue &value);
  /**
   * \brief Creates an Alias with the name name for the substring [begin; end[
   *        of the Register with the name parent
   * \param name String uniquely representing the alias Register
   * \param parent String uniquely representing the parent Register
   * \param begin First index within the parent Register of the alias register
   * \param end First index within the parent Register no longer within the
   *        alias register
   */
  void aliasRegister(const std::string &name,
                     const std::string &parent,
                     const std::size_t begin,
                     const std::size_t end);
  /**
  * \brief Creates an Alias with the name name for the substring
  *        [begin; parent.getSize()[ of the Register with the name parent
  * \param name String uniquely representing the alias Register
  * \param parent String uniquely representing the parent Register
  * \param begin First index within the parent Register of the alias register
   */
  void aliasRegister(const std::string &name,
                     const std::string &parent,
                     const std::size_t begin = 0);

 private:
  std::unordered_map<std::string, RegisterID>
      _dict; /**< Brief Map mapping name -> RegisterID*/
  std::vector<MemoryValue>
      _register; /**< Brief Vector holding all the Registers with no parent*/
  // I'm using set because that makes implementing the option to delete
  // registers way easier, vector would've been enough at this moment
  std::vector<std::set<std::string>> _updateSet; /**< Brief Vector mapping
                                                      RegisterID.address -> all
                                                      childrens name of this
                                                      Register*/
  static const std::
      function<void(const std::string &)>
          emptyCallback; /**< Brief An empty function that does absolutely
                            nothing, used as default for _callback*/
  std::function<void(const std::string &)> _callback =
      emptyCallback; /**< Brief This function gets called for every changed
                        Register*/
  /**
   * \brief This Method is called whenever something in the Memory changes and
   *        notifies the Gui ofthe change
   */
  void wasUpdated(const std::size_t address);
};

#endif// ERAGPSIM_CORE_REGISTERSET_HPP
