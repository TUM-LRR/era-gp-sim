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

#include <cassert>
#include <cstddef>
#include <set>
#include <unordered_map>
#include <vector>

#include "core/memory-value.hpp"
#include "core/register-id.hpp"

class RegisterSet {
 public:
  /**
   * \brief
   */
  RegisterSet();
  /**
   * \brief
   * \param bucketCount
   */
  RegisterSet(const std::size_t bucketCount);
  /**
   * \brief
   * \param other
   */
  RegisterSet(RegisterSet &&other) = default;
  /**
   * \brief
   * \param other
   */
  RegisterSet &operator=(RegisterSet &&other) = default;
  /**
   * \brief
   * \param other
   */
  RegisterSet(const RegisterSet &other) = default;
  /**
   * \brief
   * \param other
   */
  RegisterSet &operator=(const RegisterSet &other) = default;
  /**
   * \brief
   */
  ~RegisterSet() = default;

  /**
   * \brief
   * \param name
   */
  MemoryValue get(const std::string &name) const;
  /**
   * \brief
   * \param name
   * \param out
   */
  void get(const std::string &name, MemoryValue &out) const;
  /**
   * \brief
   * \param name
   * \param value
   */
  void put(const std::string &name, const MemoryValue &value);
  /**
   * \brief
   * \param name
   * \param value
   */
  MemoryValue set(const std::string &name, const MemoryValue &value);
  /**
   * \brief
   * \param name
   * \param value
   */
  MemoryValue &exchange(const std::string &name, MemoryValue &value);

  /**
   * \brief
   * \param name
   * \param size
   */
  void createRegister(const std::string &name, const std::size_t size);
  /**
   * \brief
   * \param name
   * \param value
   */
  void createRegister(const std::string &name, const MemoryValue &value);
  /**
   * \brief
   * \param name
   * \param parent
   * \param begin
   * \param end
   */
  void aliasRegister(const std::string &name,
                     const std::string &parent,
                     const std::size_t begin,
                     const std::size_t end);
  /**
   * \brief
   * \param name
   * \param parent
   * \param begin
   */
  void aliasRegister(const std::string &name,
                     const std::string &parent,
                     const std::size_t begin = 0);

 private:
  std::unordered_map<std::string, RegisterID> _dict;
  std::vector<MemoryValue> _register;
  // I'm using set because that makes implementing the option to delete
  // registers way easier, vector would've been enough at this moment
  std::vector<std::set<std::string>> _updateSet;
  void wasUpdated(const std::size_t address) {
  }
};

#endif// ERAGPSIM_CORE_REGISTERSET_HPP
