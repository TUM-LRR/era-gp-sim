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

#ifndef CORE_SCHEDULER_H_
#define CORE_SCHEDULER_H_

#include <atomic>
#include <chrono>
#include <functional>

#include "core/lockfree-queue.hpp"


class Scheduler {
 public:
  /**
 * @brief creates new Scheduler
 * @param sleepMillis optional parameter, how long the scheduler tries to sleep
 * after polling the queue
 */
  Scheduler(int sleepMillis = 50);
  ~Scheduler();


  /**
   * @brief pushes a function-object into the taskQueue
   * @param task function-object to be pushed
   */
  void push(std::function<void()> task);

 private:
  /**
   * @brief scheduler loop
   */
  void run_();

  LockfreeQueue<function<void()>> taskQueue_;
  std::atomic<bool> stopRequested_;
  std::chrono::milliseconds sleepMilliseconds_;
  std::atomic<bool> stopped_;
};

#endif// CORE_SCHEDULER_H_
