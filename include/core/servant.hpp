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

#include <cassert>
#include <functional>
#include <memory>

#include "core/proxy.hpp"
#include "core/result.hpp"
#include "core/scheduler.hpp"

/**
 * \brief Servant Superclass, inherit from this in every Servant class
 */
class Servant : public std::enable_shared_from_this<Servant> {
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
      scheduler->push(std::move(task));
    } else {
      assert(false);
    }
  }


 protected:
  /**
   * \brief creates a safe callback functor, to be used with
   * POST_CALLBACK_UNSAFE
   * Creates a callback which is posted to the correct thread and only called,
   * if this servant is still alive.
   *
   * \param callback a std::function object which accepts a Result<T> as
   * argument, the result object can either have a value of type T or an
   * exception. Create the std::function object like this:
   * std::function<void(Result<T>)> callback = std::bind(&Foo::bar, this,
   * std::placeholders::_1);
   */
  template <typename Arg>
  std::function<void(Result<Arg>)>
  makeSafeCallback(std::function<void(Result<Arg>)>&& callback) {
    return std::bind(
        [](std::weak_ptr<Servant> weak,
           std::function<void(Result<Arg>)> cb,
           Result<Arg> result) {
          // TODO only push into correct thread if necessary
          if (auto servant = weak.lock()) {
            servant->push(std::bind(
                [](std::weak_ptr<Servant> weakServant,
                   std::function<void(Result<Arg>)> cb,
                   Result<Arg> result) {
                  if (auto servant = weakServant.lock()) {
                    cb(result);
                  }
                },
                weak,
                std::move(cb),
                std::move(result)));
          }
        },
        shared_from_this(),
        std::move(callback),
        std::placeholders::_1);
  }

  std::weak_ptr<Scheduler> _scheduler;
};
