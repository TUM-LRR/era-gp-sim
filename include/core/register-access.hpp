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

#ifndef ERAGPSIM_CORE_REGISTER_ACCESS_HPP
#define ERAGPSIM_CORE_REGISTER_ACCESS_HPP

#include "core/project.hpp"
#include "core/proxy.hpp"

/**
 * A Proxy to access the register component of a project safely.
 *
 */
class RegisterAccess : public Proxy<Project> {
 public:
  RegisterAccess(const Proxy<Project>& proxy) : Proxy(proxy) {
  }

  /**
   * Returns the content of a register as MemoryValue.
   *
   * WARNING: the second paramter might dissappear in future versions
   *
   * Two overloads exist:
   * First version:
   * \param name The name of the register as std::string.
   * \param byteSize The number of bits in one byte for this register, defaults
   * to 8.
   *
   * Second version:
   * \param name The name of the register as std::string.
   * \param out rvalue ref of MemoryValue to write to.
   */
  POST_FUTURE(getRegisterValue)

  /**
   * Returns the content of a register as a MemoryValue through a callback.
   *
   * \param callback std::function<void(Result<R>)> callback to call with the
   * value(s).
   * \param callerServant std::weak_ptr<Servant> servant which called this.
   *
   * first version:
   * \param name The name of the register as std::string.
   * \param byteSize The number of bits in one byte for this register, defaults
   *
   * second version:
   * \param name The name of the register as std::string.
   * \param out rvalue ref of MemoryValue to write to.
   */
  POST_CALLBACK_SAFE(getRegisterValue)

  /**
   * Puts a value in a register.
   *
   * \param name The name of the register as std::string.
   * \param value The MemoryValue which is written in the register.
   *
   */
  POST(putRegisterValue)

  /**
   * Sets a register to a value and returns the old value
   *
   * \param name The name of the register as std::string.
   * \param value lvalue or rvalue reference to a MemoryValue object which is
   * written to the register.
   */
  POST_FUTURE(setRegisterValue)

  /**
   * Sets a register to a value and returns the old value through a callback.
   *
   * \param callback std::function<void(Result<R>)> callback to call with the
   * value(s).
   * \param callerServant std::weak_ptr<Servant> servant which called this.
   * \param name The name of the register as std::string.
   * \param value lvalue or rvalue reference to a MemoryValue object which is
   * written to the register.
   */
  POST_CALLBACK_SAFE(setRegisterValue)

  /**
   * Reset the state of the registers
   *
   */
  POST(resetRegisters)
};

#endif /* ERAGPSIM_CORE_REGISTER_ACCESS_HPP */
