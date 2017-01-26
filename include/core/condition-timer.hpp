/*
* ERASIM Assembler Interpreter
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

#ifndef ERASIM_CORE_CONDITION_TIMER_HPP
#define ERASIM_CORE_CONDITION_TIMER_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>

class ConditionTimer {
 public:
  ConditionTimer();

  /**
   * Wakes up all waiting threads.
   */
  void notifyAll();

  /**
   * Wakes up a waiting thread.
   */
  void notifyOne();

  /**
   * Reset the flag to false.
   */
  void reset();

  /**
   * Return the value of the flag.
   */
  bool getFlag();

  /**
   * Blocks current thread until notify is called by any thread, does not reset
   * the internal flag (reset has to be called before wait will have any
   * effect again).
   */
  void wait();

  /**
   * Blocks the current thread until notify is called by any thread, resets the
   * internal flag.
   */
  void waitAndReset();

  /**
   * Blocks current thread until notify is called or the wait timed out.
   * Does not reset the internal flag (reset has to be called before wait will
   * have any effect again).
   *
   * \param duration Default timeout in case notify is not called.
   */
  template <typename Rep, typename Period>
  void waitFor(std::chrono::duration<Rep, Period> duration) {
    std::unique_lock<std::mutex> lock(_mutex);
    while (!_flag) {
      auto start = std::chrono::steady_clock::now();
      auto didTimeOut = _conditionVariable.wait_for(lock, duration);
      // check if the sleep was interupted by the notify method or timed out.
      if (_flag) break;
      if (didTimeOut == std::cv_status::timeout) break;
      // this appears to be a spurious wakeup, sleep again.
      auto end = std::chrono::steady_clock::now();
      auto elapsed =
          std::chrono::duration_cast<decltype(duration)>(end - start);
      if (elapsed >= duration) break;
      duration -= elapsed;
    }
  }

 private:
  /** Mutex for the condition variable. */
  std::mutex _mutex;

  /** Flag to avoid race conditions with the condition variable. */
  bool _flag;

  /** A condition variable for sleeping until a condition is met. */
  std::condition_variable _conditionVariable;
};

#endif /* ERASIM_CORE_CONDITION_TIMER_HPP */
