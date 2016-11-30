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

#include <chrono>
#include <memory>

#include "core/condition-timer.hpp"
#include "core/project.hpp"
#include "core/proxy.hpp"

/**
 * A Proxy to access the memory and register components of a project safely.
 *
 */
class MemoryAccess : public Proxy<Project> {
 public:
  using SharedCondition = std::shared_ptr<ConditionTimer>;

  /**
   * Constructs a new MemoryAccess.
   *
   */
  MemoryAccess(const Proxy<Project>& proxy, SharedCondition conditionTimer)
  : Proxy(proxy), _conditionTimer(conditionTimer) {
  }

  /**
   * Lets the thread that calls this sleep for a specified amount of time.
   * The sleep is interruptible through a stop flag.
   *
   * \param sleepDuration (minimum)duration of the sleep.
   */
  template <typename Rep, typename Period>
  void sleep(const std::chrono::duration<Rep, Period> sleepDuration) {
    _conditionTimer->waitFor(sleepDuration);
  }

  /**
   * Returns a number of memory cells at a given address as a MemoryValue.
   *
   * \param address address of the memory cells.
   * \param length number of memory cells to return, default is 1.
   */
  POST_FUTURE_CONST(getMemoryValueAt)

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
  POST_CALLBACK_SAFE(getMemoryValueAt)

  /**
   * Puts a value into a memory cell.
   *
   * \param address The value is written to this address.
   * \param value The MemoryValue to write.
   */
  POST(putMemoryValueAt)

  /**
   * Sets a memory cell to a value and returns the old value.
   *
   * \param address The value is written to this address.
   * \param value The MemoryValue to write.
   */
  POST_FUTURE(setMemoryValueAt)

  /**
   * Set a memory cell to a value and return the old value through a callback.
   *
   * \param callback std::function<void(Result<R>)> callback to call with the
   * value(s).
   * \param callerServant std::weak_ptr<Servant> servant which called this.
   * \param address The value is written to this address.
   * \param value The MemoryValue to write.
   */
  POST_CALLBACK_SAFE(setMemoryValueAt)

  /**
   * Checks if any memory cells in the given area are protected.
   *
   * \param address The first address of the area to check
   * \param amount The amount of cells to check
   */
  POST_FUTURE_CONST(isMemoryProtectedAt)

  /**
   * Creates a memory protection.
   *
   * \param address The first address of the area to lock
   * \param amount The amount of cells to protect
   */
  POST(makeMemoryProtected)

  /**
   * Removes a memory protection.
   *
   * \param address The first address of the locked area
   * \param value The amount of cells to unlock
   */
  POST(removeMemoryProtection)

  /**
   * Checks if any memory cells in the given area are protected.
   *
   * \param callback std::function<void(Result<R>)> callback to call with the
   * value(s).
   * \param callerServant std::weak_ptr<Servant> servant which called this.
   * \param address The first address of the area to check
   * \param amount The amount of cells to check
   */
  POST_CALLBACK_SAFE(isMemoryProtectedAt)

  /**
   * Returns the content of a register as MemoryValue.
   *
   * \param name The name of the register as std::string.
   *
   */
  POST_FUTURE_CONST(getRegisterValue)

  /**
   * Returns the content of a register as a MemoryValue through a callback.
   *
   * \param callback std::function<void(Result<R>)> callback to call with the
   * value(s).
   * \param callerServant std::weak_ptr<Servant> servant which called this.
   * \param name The name of the register as std::string.
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
   * \param value MemoryValue object which is
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
   * \param MemoryValue object which is
   * written to the register.
   */
  POST_CALLBACK_SAFE(setRegisterValue)

  /**
   * Returns the number of memory cells(number of bytes)
   *
   */
  POST_FUTURE_CONST(getMemorySize)

 private:
  /** Condition timer which can block this thread for a specified time, but
   * wakes up on notification. */
  SharedCondition _conditionTimer;
};

#endif /* ERAGPSIM_CORE_MEMORY_ACCESS_HPP */
