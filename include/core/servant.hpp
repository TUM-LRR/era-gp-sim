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

#ifndef ERAGPSIM_CORE_SERVANT_HPP
#define ERAGPSIM_CORE_SERVANT_HPP

#include <cassert>
#include <functional>
#include <memory>

#include "common/tuple.hpp"
#include "core/proxy.hpp"
#include "core/result.hpp"
#include "core/scheduler.hpp"

/**
 * \brief Servant Superclass, inherit from this in every Servant class
 *
 * A servant class is an active object, as it has its own scheduler and
 * therefore its own thread.
 *
 * It is a separate class from the scheduler to allow for more flexibility, as
 * this can allow multiple servants to run on the same scheduler.
 * However, this could be dangerous with the current implementation.
 *
 *
 */
class Servant : public std::enable_shared_from_this<Servant> {
 public:
  Servant(std::weak_ptr<Scheduler>&& scheduler)
  : _scheduler(std::move(scheduler)) {
  }

  /**
   * \brief if the scheduler is still alive, push a task into its task-queue.
   * An assertion fails otherwise.
   * \param task The task which is pushed into the task-queue.
   */
  void push(Scheduler::Task&& task) {
    if (auto scheduler = _scheduler.lock()) {
      scheduler->push(std::move(task));
    } else {
      assert(false);
    }
  }


 protected:
  /**
   * \brief creates a safe callback functor, to be used with
   * POST_CALLBACK_UNSAFE(functionName)
   * Creates a callback which is posted to the correct thread and only called,
   * if this servant is still alive.
   *
   * \param callback a std::function object which accepts a something as
   * argument, for use with POST_CALLBACK_UNSAFE it has to be a Result<T>
   * object. Create the std::function object like this:
   *
   * \code
   * //use the a real type instead of T
   * std::function<void(Result<T>)> callback = [this](Result<T> result) {
   * //call a member method wich accepts a Result<T> object as paramter
   * callback(result);};
   * \endcode
   */
  template <typename... Args>
  std::function<void(Args...)>
  makeSafeCallback(std::function<void(Args...)>&& callback) {
    std::weak_ptr<Servant> weak = shared_from_this();
    /* create a task which in turn creates a safe callback task when called and
     * puts it into the scheduler queue of this servant. */
    auto task = [ weak, callback = std::move(callback) ](Args && ... args) {
      if (auto servant = weak.lock()) {
        /* This callback task captures the arguments, which were given to the
         * "outer task" as paramters, and applies them to the callback
         * function.*/
        auto callbackTask = [
          weak,
          callback = std::move(callback),
          tuple    = std::make_tuple(std::forward<Args>(args)...)
        ]() mutable {
          if (auto servant = weak.lock()) {
            TupleApply::apply(std::move(callback), std::move(tuple));
          }
        };
        servant->push(callbackTask);
      }
    };
    return task;
  }

  /** A weak_ptr to the scheduler of this servant. */
  std::weak_ptr<Scheduler> _scheduler;
};

#endif /* ERAGPSIM_CORE_SERVANT_HPP */
