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
#include <thread>

#include "lockfree-queue.hpp"


class Scheduler {
 public:
  using Queue = LockfreeQueue<std::function<void()>>;

  /**
 * \brief creates new Scheduler
 * \param sleepMillis optional parameter, how long the scheduler tries to sleep
 * after polling the queue
 */
  Scheduler(int sleepMillis = 50)
  : _interrupt(false)
  , _sleepMilliseconds(sleepMillis)
  , _schedulerThread(&Scheduler::_run, this) {
  }

  ~Scheduler() {
    this->_shutdown();
    _schedulerThread.join();
  }

  // The scheduler can not be moved or copied, as it has a own thread and its
  // queue can not be copied or moved as well
  Scheduler(const Scheduler& other)  = delete;
  Scheduler(const Scheduler&& other) = delete;
  Scheduler& operator=(const Scheduler& other) = delete;
  Scheduler& operator=(const Scheduler&& other) = delete;


  /**
   * \brief pushes a function-object into the taskQueue
   * \param task function-object to be pushed
   */
  void push(std::function<void()>&& task) {
    _taskQueue.push(std::move(task));
  }

 private:
  /**
   * \brief scheduler loop
   */
  void _run() {
    _interrupt = false;
    while (!_interrupt) {
      std::function<void()> task;
      if (_taskQueue.pop(task)) {
        // there was something in the queue, ececute the task
        task();

      } else {
        // the queue is empty, sleep for a bit
        std::this_thread::sleep_for(_sleepMilliseconds);
      }
    }
  }

  /**
   * \brief stops the scheduler loop after every task that is currently in the
   * queue was finished
   */
  void _shutdown() {
    this->push([this]() { _interrupt = true; });
  }

  Queue _taskQueue;
  bool _interrupt;
  std::chrono::milliseconds _sleepMilliseconds;
  std::thread _schedulerThread;
};

#endif// CORE_SCHEDULER_H_
