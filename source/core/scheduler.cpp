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

#include "core/scheduler.hpp"

Scheduler::Scheduler()
: _taskQueue()
, _mutex()
, _conditionVariable()
, _interrupt(false)
, _schedulerThread(&Scheduler::_run, this) {
}

Scheduler::~Scheduler() {
  this->_shutdown();
  _schedulerThread.join();
}

void Scheduler::push(Scheduler::Task&& task) {
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _taskQueue.emplace(std::move(task));
  }
  // the lock is destroyed before this to avoid waking the scheduler up and
  // blocking again immediately
  _conditionVariable.notify_one();
}

std::thread::id Scheduler::getThreadId() {
  return _schedulerThread.get_id();
}

void Scheduler::_run() {
  _interrupt = false;

  while (!_interrupt) {
    std::unique_lock<std::mutex> lock(_mutex);
    // wait while there is nothing in the queue, guards for spurious wakeups.
    _conditionVariable.wait(lock, [this] { return !_taskQueue.empty(); });
    // The mutex is reacquired after wait() ends. Now pop an element from the
    // queue (which is guaranteed to not be empty)
    Task task = _taskQueue.front();
    _taskQueue.pop();
    lock.unlock();
    // there was something in the queue, execute the task
    task();
  }
}

void Scheduler::_shutdown() {
  this->push([this] { _interrupt = true; });
}
