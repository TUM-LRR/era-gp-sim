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

#ifndef ERAGPSIM_CORE_MEMORY_ACCESS_HPP
#define ERAGPSIM_CORE_MEMORY_ACCESS_HPP

#include "core/project.hpp"
#include "core/proxy.hpp"

/**
 * A Proxy to access the memory component of a project safely.
 *
 */
class MemoryAccess : public Proxy<Project> {
 public:
  MemoryAccess(const Proxy<Project>& proxy) : Proxy(proxy) {
  }

  /**
   * Returns a number of memory cells at a given address as a MemoryValue.
   *
   * \param address address of the memory cells.
   * \param length number of memory cells to return, default is 1.
   */
  POST_FUTURE(getMemory)

  /**
   * Returns a number of memory cells at a given address as
   * MemoryValue through a callback.
   *
   * \param callback std::function<void(Result<R>)> callback to call with the
   * value(s).
   * \param callerServant std::weak_ptr<Servant> servant which called this.
   * \param address address of the memory cells.
   * \param length number of memory cells to return, default is 1.
   */
  POST_CALLBACK_SAFE(getMemory)

  /**
   * Puts a value into a memory cell.
   *
   * \param address The value is written to this address.
   * \param value The MemoryValue to write.
   */
  POST(putMemoryCell)

  /**
   * Sets a memory cell to a value and returns the old value.
   *
   * \param address The value is written to this address.
   * \param value The MemoryValue to write.
   */
  POST_FUTURE(setMemoryCell)

  /**
   * Set a memory cell to a value and return the old value through a callback.
   *
   * \param callback std::function<void(Result<R>)> callback to call with the
   * value(s).
   * \param callerServant std::weak_ptr<Servant> servant which called this.
   * \param address The value is written to this address.
   * \param value The MemoryValue to write.
   */
  POST_CALLBACK_SAFE(setMemoryCell)

  /**
   * Returns the number of memory cells(number of bytes)
   *
   */
  POST_FUTURE(getMemorySize)

  /**
   * Sets the number of memory cells
   * WARNING: might not be supported later
   *
   * \param size Number of memory cells
   */
  POST(setMemorySize)

  /**
   * Reset the state of the memory
   *
   */
  POST(resetMemory)
};

#endif /* ERAGPSIM_CORE_MEMORY_ACCESS_HPP */
