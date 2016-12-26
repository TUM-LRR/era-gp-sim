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

#include "core/condition-timer.hpp"

ConditionTimer::ConditionTimer()
: _mutex(), _flag(false), _conditionVariable() {
}

void ConditionTimer::notifyAll() {
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _flag = true;
  }
  _conditionVariable.notify_all();
}

void ConditionTimer::notifyOne() {
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _flag = true;
  }
  _conditionVariable.notify_one();
}

void ConditionTimer::reset() {
  std::lock_guard<std::mutex> lock(_mutex);
  _flag = false;
}

bool ConditionTimer::getFlag() {
  std::lock_guard<std::mutex> lock(_mutex);
  return _flag;
}

void ConditionTimer::wait() {
  std::unique_lock<std::mutex> lock(_mutex);
  while (!_flag) {
    _conditionVariable.wait(lock);
  }
}

void ConditionTimer::waitAndReset() {
  std::unique_lock<std::mutex> lock(_mutex);
  while (!_flag) {
    _conditionVariable.wait(lock);
  }
  if (!lock) lock.lock();
  _flag = false;
}
