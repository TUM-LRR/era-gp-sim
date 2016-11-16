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

#ifndef ERAGPSIM_CORE_MEMORY_MANAGER_HPP
#define ERAGPSIM_CORE_MEMORY_MANAGER_HPP

#include "core/project.hpp"
#include "core/proxy.hpp"

/**
 * A proxy to manage the memory and registers of a project safely.
 *
 */
class MemoryManager : public Proxy<Project> {
 public:
  MemoryManager(const Proxy<Project>& proxy) : Proxy(proxy) {
  }

  /**
   * Reset the state of the memory
   *
   */
  POST(resetMemory)

  /**
   * Reset the state of the registers
   *
   */
  POST(resetRegisters)

  /**
   * Set the callback which is used to signal the gui that a register was
   * updated
   *
   * \param callback std::function<void(std::string)>
   *
   */
  POST(setUpdateRegisterCallback)

  /**
   * Set the callback which is used to signal the gui that the memory was
   * updated
   *
   * \param callback std::function<void(int)>
   *
   */
  POST(setUpdateMemoryCallback)
};

#endif /* ERAGPSIM_CORE_MEMORY_MANAGER_HPP */
