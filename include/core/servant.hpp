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

#include <cassert>
#include <functional>
#include <memory>

/**
 * \brief Servant Superclass
 */
class Servant {
 public:
  Servant(std::weak_ptr<Scheduler>&& scheduler)
  : _scheduler(std::move(scheduler)) {
  }

  /**
   * \brief if the scheduler is still alive, push a task in its task-queue
   * \param functor to be pushed into the task-queue
   */
  void push(std::function<void()>&& task) {
    if (auto scheduler = _scheduler.lock()) {
      scheduler.get()->push(std::move(task));
    } else {
      assert(false);
    }
  }

 private:
  std::weak_ptr<Scheduler> _scheduler;
};
