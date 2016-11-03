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

#ifndef ERAGPSIM_CORE_ARCHITECTURE_ACCESS_HPP
#define ERAGPSIM_CORE_ARCHITECTURE_ACCESS_HPP

#include "core/project.hpp"
#include "core/proxy.hpp"

/**
 * A Proxy to access the architecture of a project safely.
 *
 */
class ArchitectureAccess : public Proxy<Project> {
 public:
  ArchitectureAccess(const Proxy<Project>& proxy) : Proxy(proxy) {
  }

  /**
   * Returns a container of all registers
   *
   */
  POST_FUTURE_CONST(getRegisterUnits)

  /**
   * Returns the number of bits in a byte
   *
   */
  POST_FUTURE_CONST(getByteSize)

  /**
   * Returns a set of all instructions of the architecture
   *
   */
  POST_FUTURE_CONST(getInstructionSet)

  /**
   * Returns the callback used for conversion from a MemoryValue to a signed
   * decimal integer as a std::string
   *
   */
  POST_FUTURE_CONST(getSignedDecimalConversion)

  /**
   * Returns the callback used for conversion from a MemoryValue to a unsigned
   * decimal integer as a std::string
   *
   */
  POST_FUTURE_CONST(getUnsignedDecimalConversion)

  /**
   * Returns the callback used for conversion from a MemoryValue to a signed
   * decimal float as a std::string
   *
   */
  POST_FUTURE_CONST(getFloatConversion)

  /**
   * Returns the callback used for conversion from a signed decimal integer as a
   * std::string to a MemoryValue
   *
   */
  POST_FUTURE_CONST(getSignedToMemoryValue)

  /**
   * Returns the callback used for conversion from a unsigned decimal integer as
   * a std::string to a MemoryValue
   *
   */
  POST_FUTURE_CONST(getUnsignedToMemoryValue)

  /**
   * Returns the callback used for conversion from a signed decimal float as a
   * std::string to a MemoryValue
   *
   */
  POST_FUTURE_CONST(getFloatToMemoryValue)

  /**
   * \returns Returns the Architecture object of the project.
   *
   */
  POST_FUTURE_CONST(getArchitecture)
};

#endif /* ERAGPSIM_CORE_ARCHITECTURE_ACCESS_HPP */
