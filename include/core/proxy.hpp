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

#include <functional>
#include <future>
#include <memory>

#include "core/scheduler.hpp"

#ifndef CORE_PROXY_HPP
#define CORE_PROXY_HPP

#define POST(functionName)                                               \
  template <typename... Args>                                            \
  void functionName(Args... args) {                                      \
    auto servant = servant_.get();                                       \
    servant->push(std::bind(                                             \
        [servant](Args... params) { servant->functionName(params...); }, \
        args...));                                                       \
  }

#define POST_FUTURE(functionName)                                   \
  template <typename... Args>                                       \
  auto functionName(Args... args) {                                 \
    auto servant = servant_.get();                                  \
    std::promise<decltype(servant->functionName(args...))> promise; \
    servant->push(std::bind(                                        \
        [servant, &promise](Args... params) {                       \
          try {                                                     \
            auto result = servant->functionName(params...);         \
            promise.set_value(result);                              \
          } catch (std::exception e) {                              \
            promise.set_exception(std::make_exception_ptr(e));      \
          }                                                         \
        },                                                          \
        args...));                                                  \
    return promise.get_future().get();                              \
  }

#define POST_CALLBACK(functionName)                                      \
  template <typename R, typename... Args>                                \
  void functionName(std::function<void(Result<R>)> callback,             \
                    std::weak_ptr<Scheduler> caller,                     \
                    Args... args) {                                      \
    auto servant = servant_.get();                                       \
    servant->push(std::bind(                                             \
        [servant, callback, caller](Args... params) {                    \
          Result<R> result(servant->functionName(params...));            \
          if (std::shared_ptr<Scheduler> callerShared = caller.lock()) { \
            callerShared.get()->push(std::bind(callback, result));       \
          }                                                              \
        },                                                               \
        args...));                                                       \
  }

template <typename Servant>
class Proxy {
 public:
  template <typename... Args>
  Proxy(std::shared_ptr<Scheduler> scheduler, Args&&... args) {
    std::promise<std::shared_ptr<Servant>> promise;
    std::future<std::shared_ptr<Servant>> future = promise.get_future();

    scheduler.get()->push(std::bind(
        [&promise](std::shared_ptr<Scheduler> schedulerParam,
                   Args&&... params) {
          promise.set_value(std::make_shared<Servant>(
              schedulerParam, std::forward<Args>(params)...));
        },
        scheduler,
        std::forward<Args>(args)...));
    servant_ = future.get();
  }

  ~Proxy() {
    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    servant_.get()->push([this, &promise]() {
      this->servant_.reset();
      promise.set_value();
    });
    future.get();
  }

 protected:
  std::shared_ptr<Servant> servant_;
};

#endif /* CORE_PROXY_HPP */
