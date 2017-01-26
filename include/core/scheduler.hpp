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

#ifndef ERASIM_CORE_SCHEDULER_HPP
#define ERASIM_CORE_SCHEDULER_HPP

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

/**
 * This class is a task-scheduler, to be used by a servant
 *
 * The scheduler owns a thread, which is running in a loop and executes tasks in
 * a queue one at a time.
 * If all calls to the servant are pushed into the queue, which can be easily
 * done by using a proxy, this ensures thread safety for the servant, as only
 * one task can be executed at any given moment.
 */
class Scheduler {
 public:
  using Task = std::function<void()>;
  using Queue = std::queue<Task>;

  /**
   * \brief creates new Scheduler
   */
  Scheduler();

  /**
   * Destroys the scheduler safely by posting an interrupting tasks into its own
   * queue
   *
   */
  ~Scheduler();

  // The scheduler can not be moved or copied, as it has a own thread and its
  // queue can not be copied or moved as well
  Scheduler(const Scheduler& other) = delete;
  Scheduler(const Scheduler&& other) = delete;
  Scheduler& operator=(const Scheduler& other) = delete;
  Scheduler& operator=(const Scheduler&& other) = delete;


  /**
   * \brief pushes a function-object into the taskQueue
   * \param task function-object to be pushed
   */
  void push(Task&& task);

  /**
   * returns the id of the scheduler thread
   *
   */
  std::thread::id getThreadId();

 private:
  /**
   * \brief scheduler loop, executes every task in the queue
   */
  void _run();

  /**
   * \brief stops the scheduler loop after every task that is currently in the
   * queue was finished
   */
  void _shutdown();

  /** A queue to store Task objects. */
  Queue _taskQueue;

  /** A mutex to control access to the queue. */
  std::mutex _mutex;

  /** A condition variable to notify the scheduler of new tasks. */
  std::condition_variable _conditionVariable;

  /** A boolean flag indicating whether the scheduler should interrupt its loop.
   */
  bool _interrupt;

  /** The thread handle of the scheduler execution thread, this thread is
   * running the scheduler loop. */
  std::thread _schedulerThread;
};

#endif /* ERASIM_CORE_SCHEDULER_HPP */
