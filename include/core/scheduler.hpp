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
  : interrupt_(false)
  , sleepMilliseconds_(sleepMillis)
  , schedulerThread_(&Scheduler::run_, this) {
  }

  ~Scheduler() {
    this->shutdown_();
    schedulerThread_.join();
  }


  /**
   * \brief pushes a function-object into the taskQueue
   * \param task function-object to be pushed
   */
  void push(std::function<void()>&& task) {
    taskQueue_.push(std::move(task));
  }

 private:
  /**
   * \brief scheduler loop
   */
  void run_() {
    interrupt_ = false;
    while (!interrupt_) {
      std::function<void()> task;
      if (taskQueue_.pop(task)) {
        // there was something in the queue, ececute the task
        task();

      } else {
        // the queue is empty, sleep for a bit
        std::this_thread::sleep_for(sleepMilliseconds_);
      }
    }
  }

  /**
   * \brief stops the scheduler loop after every task that is currently in the
   * queue was finished
   */
  void shutdown_() {
    this->push([this]() { interrupt_ = true; });
  }

  Queue taskQueue_;
  bool interrupt_;
  std::chrono::milliseconds sleepMilliseconds_;
  std::thread schedulerThread_;
};

#endif// CORE_SCHEDULER_H_
