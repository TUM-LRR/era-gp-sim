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

Scheduler::Scheduler(int sleepMillis)
: _interrupt(false)
, _sleepMilliseconds(sleepMillis)
, _schedulerThread(&Scheduler::_run, this) {
}

Scheduler::~Scheduler() {
  this->_shutdown();
  _schedulerThread.join();
}

void Scheduler::push(Scheduler::Task&& task) {
  _taskQueue.push(std::move(task));
}

std::thread::id Scheduler::getThreadId() {
  return _schedulerThread.get_id();
}

void Scheduler::_run() {
  _interrupt = false;

  while (!_interrupt) {
    Task task;
    if (_taskQueue.pop(task)) {
      // there was something in the queue, ececute the task
      task();

    } else {
      // the queue is empty, sleep for a bit
      std::this_thread::sleep_for(_sleepMilliseconds);
    }
  }
}

void Scheduler::_shutdown() {
  this->push([this]() { _interrupt = true; });
}
