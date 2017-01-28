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

#ifndef ERAGPSIM_CORE_MEMORY_HPP
#define ERAGPSIM_CORE_MEMORY_HPP

#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <utility>

#include "core/memory-value.hpp"
#include "third-party/json/json.hpp"

class Memory {
 public:
  using Json = nlohmann::json;
  using CallbackFunction =
      std::function<void(const std::size_t, const std::size_t)>;
  using RawMapPair = std::pair<std::map<std::string, std::size_t>,
                               std::map<std::string, std::string>>;
  using ProtectionMap = std::map<std::size_t, std::size_t>;
  /**
   * \brief Default constructor. Constructs an empty Memory with default size
   *        (64 Bytes � 8 Bit)
   */
  Memory();

  /**
   * \brief Constructs an empty Memory with cellCount Bytes � byteSize Bit
   * \param byteCount Number of Bytes
   * \param byteSize Size of a Byte in Bit
   */
  Memory(std::size_t byteCount, std::size_t byteSize = 8);

  /**
   * \brief Constructs an Memory with the data stored in json
   * \param json a json ontaining a representaion of a memory
   */
  Memory(const Json &json);

  /**
   * \brief Copy constructor. Constructs the Memory with the copy of the
   *        contents of other.
   * \param other another Memory to be used as source to initialize the elements
   *        of the Memory with
   */
  Memory(const Memory &other) = default;
  /**
   * \brief Copy constructor. Constructs the Memory with the copy of the
   *        contents of other.
   * \param other another Memory to be used as source to initialize the elements
   *        of the Memory with
   */
  Memory &operator=(const Memory &other) = default;
  /**
   * \brief Move constructor. Constructs the Memory with the contents of other
   *        using move semantics.
   * \param other another Memory to be used as source to initialize the elements
   *        of the Memory with
   */
  Memory(Memory &&other) = default;
  /**
   * \brief Move constructor. Constructs the Memory with the contents of other
   *        using move semantics.
   * \param other another Memory to be used as source to initialize the elements
   *        of the Memory with
   */
  Memory &operator=(Memory &&other) = default;
  /**
   * \brief Destructs the Memory.
   */
  ~Memory() = default;

  /**
   * \brief returns _byteSize
   * \returns returns _byteSize
   */
  std::size_t getByteSize() const;

  /**
  * \brief returns _byteCount
  * \returns returns _byteCount
  */
  std::size_t getByteCount() const;

  /**
   * \brief Sets the callback to notify the gui about changes in the data
   * \param callback the callback to be set as _callback
   */
  void setCallback(const CallbackFunction &callback);

  /**
   * \brief Returns a MemoryValue holding the data stored in the Memory at
   *        [address; address+amount[
   * \param address Starting location of the Value in the Memory
   * \param amount Number of Bytes comprising the value
   * \returns MemoryValue holding the data stored in the Memory at
   *          [address;address+amount[
   * \throws AssertionError iff [address; address+amount[ is not subset of
   *         [0; _byteCount[
   */
  MemoryValue get(std::size_t address, std::size_t amount = 1) const;
  /**
   * \brief Returns a MemoryValue holding the data stored in the Memory at
   *        [address; address+amount[
   * \param address Starting location of the Value in the Memory
   * \param amount Number of Bytes comprising the value
   * \returns MemoryValue holding the data stored in the Memory at
   *          [address;address+amount[ with 0 for all bytes not in
   *          [0; _byteCount[
   */
  MemoryValue tryGet(std::size_t address, std::size_t amount = 1) const;
  /**
   * \brief Writes value into the Memory at address
   * \param address Starting address of the to be overwritten value
   * \param value Value to write
   * \param ignoreProtection if this is true do ignore all protection
   * \throws AssertionError iff [address; address+(value.size()/_byteSize)[ is
   *         not subset of [0; _byteCount[ or value.size()%_byteSize != 0
   */
  void put(std::size_t address,
           const MemoryValue &value,
           bool ignoreProtection = false);
  /**
   * \brief Writes value into the Memory at address
   * \param address Starting address of the to be overwritten value
   * \param value Value to write
   * \param ignoreProtection if this is true do ignore all protection
   */
  void tryPut(std::size_t address,
              const MemoryValue &value,
              bool ignoreProtection = false);
  /**
   * \brief Writes value into the Memory at address and returns the previous
   *        value
   * \param address Starting address of the to be overwritten value
   * \param value Value to write
   * \param ignoreProtection if this is true do ignore all protection
   * \returns Value that was overwritten
   * \throws AssertionError iff [address; address+(value.size()/_byteSize)[ is
   *         not subset of [0; _byteCount[ or value.size()%_byteSize != 0
   */
  MemoryValue set(std::size_t address,
                  const MemoryValue &value,
                  bool ignoreProtection = false);
  /**
   * \brief Writes value into the Memory at address and returns the previous
   *        value
   * \param address Starting address of the to be overwritten value
   * \param value Value to write
   * \param ignoreProtection if this is true do ignore all protection
   * \returns Value that was overwritten with 0 for all bytes not in
   *          [0; _byteCount[
   */
  MemoryValue trySet(std::size_t address,
                     const MemoryValue &value,
                     bool ignoreProtection = false);

  /**
   * \brief converts the memory into serializeable strings
   * \param json the json object to hold the data
   * \param separator character used to separate the cells of each line
   * \param lineLength the length of a line in byte
   * \returns json
   */
  Json &serializeJSON(Json &json,
                      char separator = _standardSeparator,
                      std::size_t lineLength = 1) const;

  /**
   * \brief converts the memory into serializeable strings
   * \param json the json object to hold the data
   * \param separator character used to separate the cells of each line
   * \param lineLength the length of a line in byte
   * \returns json
   */
  Json serializeJSON(Json &&json = Json(),
                     char separator = _standardSeparator,
                     std::size_t lineLength = 1) const;

  /**
   * \brief sets the memory to the data stored in json
   * \param json the json object to holding the data
   */
  void deserializeJSON(const Json &json);

  /**
   * \brief returns true iff this == other
   * \returns the equality of this and other
   */
  bool operator==(const Memory &other) const;

  /**
   * \brief prints a representation of this into the stream
   * \returns the stream
   */
  friend std::ostream &operator<<(std::ostream &stream, const Memory &value);

  /**
   * \brief sets everything to 0x00
   */
  void clear();

  /**
   * \brief returns true iff any cells within the area is protected
   * \param address first address of the to check area
   * \param amount of cells to ckeck beginning with address
   * \returns true iff any cells within the area is protected
   */
  bool isProtected(std::size_t address, std::size_t amount = 1) const;

  /**
   * \brief makes the area protected
   * \param address first address of the to protect area
   * \param amount of cells to protect beginning with address
   */
  void makeProtected(std::size_t address, std::size_t amount = 1);

  /**
   * \brief makes the area not protected
   * \param address first address of the to unprotect area
   * \param amount of cells to unprotect beginning with address
   */
  void removeProtection(std::size_t address, std::size_t amount = 1);

  /**
   * \brief makes nothing protected
   */
  void removeAllProtection();

 private:
  /**
   * \brief character defaulty separating serialized cells
   */
  static constexpr char _standardSeparator = ',';
  /**
   * \brief constant identifiers within a serialized memory
   */
  static const std::string _byteCountStringIdentifier;
  static const std::string _byteSizeStringIdentifier;
  static const std::string _lineLengthStringIdentifier;
  static const std::string _separatorStringIdentifier;
  static const std::string _lineStringIdentifier;
  static const std::string _dataMapStringIdentifier;

  /**
   * \brief appends a string represenation of value to stream
   * \param strm stringstream so append value to
   * \param value value to append to strm
   */
  static void
  _appendMemoryValue(std::stringstream &strm, const MemoryValue &value);

  /**
   * \brief converts a line of hex values to a MemoryValue
   * \param line line containing all the data
   * \param byteSize Size of a memory cell in bit
   * \param lineLength maximum number of cells separated by separator
   * \param separator char separating different cells
   * \returns a long MemoryValue represenation of line
   */
  static MemoryValue _deserializeLine(const std::string &line,
                                      std::size_t byteSize,
                                      std::size_t lineLength,
                                      char separator);

  /**
   * \brief Returns a MemoryValue holding the data stored in the Memory at
   *        [address; address+amount[, undefined behaviour if
   *        [address; address+amount[ is not subset of [0;_byteCount[
   * \param address Starting location of the Value in the Memory
   * \param amount Number of Bytes comprising the value
   * \returns MemoryValue holding the data stored in the Memory at
   *          [address;address+amount[
   */
  MemoryValue _get(std::size_t address, std::size_t amount = 1) const;
  /**
   * \brief Writes value into the Memory at address, undefined behaviour if
   *        [address; address + value.size() / _byteSize[ is not subset of
   *        [0;_byteCount[
   * \param address Starting address of the to be overwritten value
   * \param value Value to write
   * \param amount precalculated amount of bytes stored in value
   * \param ignoreProtection if this is true do ignore all protection
   */
  void _put(std::size_t address,
            const MemoryValue &value,
            std::size_t amount,
            bool ignoreProtection = false);
  /**
   * \brief Writes value into the Memory at address and returns the previous
   *        value, undefined behaviour if [address; address + value.size() /
   *        _byteSize[ is not subset of [0;_byteCount[
   * \param address Starting address of the to be overwritten value
   * \param value Value to write
   * \param ignoreProtection if this is true do ignore all protection
   * \returns Value that was overwritten
   */
  MemoryValue _set(std::size_t address,
                   const MemoryValue &value,
                   bool ignoreProtection = false);
  /**
   * \brief This Method is called when the whole Memory changes and
   *        notifies the Gui of the change
   */
  void _wasUpdated() const;
  /**
   * \brief This Method is called whenever something in the Memory changes and
   *        notifies the Gui of the change
   * \param address address of the updated value
   * \param amount length of the updated value
   */
  void _wasUpdated(std::size_t address, std::size_t amount = 1) const;

  /**
   * \brief converts the memory into serializeable strings
   * \param separator character used to separate the cells of each line
   * \param lineLength the length of a line in byte
   * \returns a serialize representation of the Memory
   */
  RawMapPair _serializeRaw(char separator = _standardSeparator,
                           std::size_t lineLength = 64) const;

  /**
   * \brief returns true if the protection area [protectionBegin, protectionEnd]
   *        overlaps with the area [address, address + amount]
   */
  bool _overlaps(std::size_t protectionBegin,
                 std::size_t protectionEnd,
                 std::size_t address,
                 std::size_t amount,
                 bool equal) const;
  /**
   * \brief Size of a Byte in Bit
   */
  std::size_t _byteSize;
  /**
   * \brief Number of Bytes
   */
  std::size_t _byteCount;
  /**
   * \brief MemoryValue holding all the data
   */
  MemoryValue _data;
  /**
   * \brief This function gets called for every changed area in Memory
   */
  CallbackFunction _callback;

  /**
   * \brief vector storing data about protected memory areas
   * \note this takes linear time, one could improve this by implementing some
   *       binary search, or using some indexed algorithm
   */
  ProtectionMap _protection{};
};

#endif// ERAGPSIM_CORE_MEMORY_HPP
