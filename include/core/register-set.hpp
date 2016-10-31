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

#include "core/memory-value.hpp"
#include "core/register-id.hpp"
#include "third-party/json/json.hpp"


class RegisterSet {
 public:
  using Json = nlohmann::json;
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
   * \brief returns the size of a Register in bit
   * \param name String uniquely representing the Register
   * \returns Size of the Register in bit
   */
  std::size_t getSize(const std::string &name) const;

  /**
   * \brief Creates a Register with the name name and size size
   * \param name String uniquely representing the to be created Register
   * \param size Size of the Register in bit
   * \param constant Makes it so this Register cannot be changed anymore
   */
  void createRegister(const std::string &name,
                      std::size_t size,
                      bool constant = false);
  /**
   * \brief Creates a Register with the name name and stores a copy of value
   * \param name String uniquely representing the to be created Register
   * \param value Initial value of the register
   * \param constant Makes it so this Register cannot be changed anymore
   */
  void createRegister(const std::string &name,
                      const MemoryValue &value,
                      bool constant = false);
  /**
   * \brief Creates a Register with the name nameList[0] and stores a copy of
   *        value, creates Alias to this register for the remaining names
   * \param nameList Vector of Strings uniquely representing the to be created
   *        Register
   * \param value Initial value of the register
   * \param constant Makes it so this Register cannot be changed anymore
   * \param silent option to make all aliases created by this be silent
   */
  void createRegister(const std::vector<std::string> &nameList,
                      const MemoryValue &value,
                      bool constant = false,
                      bool silent = false);
  /**
   * \brief Creates a Register with the name name and size size, creates Alias
   *        to this register for the remaining names
   * \param nameList Vector of Strings uniquely representing the to be created
   *        Register
   * \param size Size of the Register in bit
   * \param constant Makes it so this Register cannot be changed anymore
   * \param silent option to make all aliases created by this be silent
   */
  void createRegister(const std::vector<std::string> &nameList,
                      std::size_t size,
                      bool constant = false,
                      bool silent = false);
  /**
   * \brief Creates an Alias with the name name for the substring [begin; end[
   *        of the Register with the name parent
   * \param name String uniquely representing the alias Register
   * \param parent String uniquely representing the parent Register
   * \param begin First index within the parent Register of the alias register
   * \param end First index within the parent Register no longer within the
   *        alias register
   * \param silent option to make the alias created by this be silent
   */
  void aliasRegister(const std::string &name,
                     const std::string &parent,
                     const std::size_t begin,
                     const std::size_t end,
                     const bool silent = false);
  /**
   * \brief Creates an Alias with the names in nameList for the substring
   *        [begin; end[ of the Register with the name parent
   * \param nameList Vector of Strings uniquely representing the alias Register
   * \param parent String uniquely representing the parent Register
   * \param begin First index within the parent Register of the alias register
   * \param end First index within the parent Register no longer within the
   *        alias register
   * \param silent option to make the alias created by this be silent
   */
  void aliasRegister(const std::vector<std::string> &nameList,
                     const std::string &parent,
                     const std::size_t begin,
                     const std::size_t end,
                     const bool silent = false);
  /**
   * \brief Creates an Alias with the name name for the substring
   *        [begin; parent.getSize()[ of the Register with the name parent
   * \param name String uniquely representing the alias Register
   * \param parent String uniquely representing the parent Register
   * \param begin First index within the parent Register of the alias register
   * \param silent option to make the alias created by this be silent
   */
  void aliasRegister(const std::string &name,
                     const std::string &parent,
                     const std::size_t begin = 0,
                     const bool silent = false);
  /**
   * \brief Creates an Alias with the names in nameList for the substring
   *        [begin; parent.getSize()[ of the Register with the name parent
   * \param nameList Vector of Strings uniquely representing the alias Register
   * \param parent String uniquely representing the parent Register
   * \param begin First index within the parent Register of the alias register
   * \param silent option to make the alias created by this be silent
   */
  void aliasRegister(const std::vector<std::string> &nameList,
                     const std::string &parent,
                     const std::size_t begin = 0,
                     const bool silent = false);

  /**
   * \brief converts the memory into serializeable strings
   * \param json the json object to hold the data
   * \param separator character used to separate the cells of each line
   * \param lineLength the length of a line in byte
   * \returns json
   */
  Json &serializeJSON(Json &json) const;
  /**
   * \brief converts the memory into serializeable strings
   * \param json the json object to hold the data
   * \param separator character used to separate the cells of each line
   * \param lineLength the length of a line in byte
   * \returns json
   */
  Json serializeJSON(Json &&json = Json()) const;

  /**
   * \brief returns true iff the number of parent registers is equal all parent
   *        registers of this exist in other and have the same length and data
   * \param other RegisterSet to compare this with
   * \returns equality of this and other
   */
  bool operator==(const RegisterSet &other) const;

  /**
   * \brief returns true iff there exists a Register with name name
   */
  bool existsRegister(const std::string &name) const;

  /**
   * \brief sets the memory to the data stored in json
   * \param json the json object to holding the data
   */
  void deserializeJSON(const Json &json);

  /**
   * \brief prints a representation of this into the stream
   * \returns the stream
   */
  friend std::ostream &
  operator<<(std::ostream &stream, const RegisterSet &value);

 private:
  /**
   * \brief constant identifiers within a serialized RegisterSet
   */
  static const std::string _registerStringIdentifier;
  static const std::string _registerNameListStringIdentifier;
  static const std::string _registerDataMapStringIdentifier;
  /**
   * \brief Map mapping name -> RegisterID
   */
  std::unordered_map<std::string, RegisterID> _dict;
  /**
   * \brief Vector holding all the parent Registers/Registers with no parent
   */
  std::vector<MemoryValue> _register;
  /**
   * \brief Is true if this ancestor Register is constant
   */
  std::vector<bool> _constant;
  /**
   * \brief Vector mapping RegisterID.address -> all childrens name of this
   *        Register
   * \note I'm using set because that makes implementing the option to delete
   *       registers way easier, Vector would've been enough at this moment
   */
  std::vector<std::set<std::string>> _updateSet;
  /**
   * \brief Vector mapping RegisterID.address -> parent name of this Register
   */
  std::vector<std::string> _parentVector;
  /**
   * \brief This function gets called for every changed Register
   */
  std::function<void(const std::string &)> _callback = [](const std::string &) {
  };

  /**
   * \brief returns a raw serialized version of this
   */
  std::map<std::string, MemoryValue> _serializeRaw() const;

  /**
   * \brief This Method is called whenever something in the Memory changes and
   *        notifies the Gui ofthe change
   */
  void _wasUpdated(const std::size_t address);
};

#endif// ERAGPSIM_CORE_REGISTERSET_HPP
