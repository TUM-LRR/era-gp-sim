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
#include <future>
#include <memory>

#include "core/scheduler.hpp"

#ifndef CORE_PROXY_HPP
#define CORE_PROXY_HPP


#define POST(functionName)                                               \
  template <typename... Args>                                            \
  void functionName(Args... args) {                                      \
    auto servant = _servant.get();                                       \
    servant->push(std::move(std::bind(                                   \
        [servant](Args... params) { servant->functionName(params...); }, \
        std::move(args)...)));                                           \
  }

#define POST_FUTURE(functionName)                                   \
  template <typename... Args>                                       \
  auto functionName(Args... args) {                                 \
    auto servant = _servant.get();                                  \
    std::promise<decltype(servant->functionName(args...))> promise; \
    servant->push(std::move(std::bind(                              \
        [servant, &promise](Args... params) {                       \
          try {                                                     \
            auto result = servant->functionName(params...);         \
            promise.set_value(result);                              \
          } catch (std::exception e) {                              \
            promise.set_exception(std::make_exception_ptr(e));      \
          }                                                         \
        },                                                          \
        std::move(args)...)));                                      \
    return promise.get_future().get();                              \
  }

#define POST_CALLBACK(functionName)                                           \
  template <typename R, typename... Args>                                     \
  void functionName(std::function<void(Result<R>)> callback,                  \
                    std::weak_ptr<Scheduler> caller,                          \
                    Args... args) {                                           \
    auto servant = _servant.get();                                            \
    servant->push(std::move(std::bind(                                        \
        [servant](Args... params,                                             \
                  std::function<void(Result<R>)> callback,                    \
                  std::weak_ptr<Scheduler> caller) {                          \
          Result<R> result(std::move(servant->functionName(params...)));      \
          if (std::shared_ptr<Scheduler> callerShared = caller.lock()) {      \
            callerShared.get()->push(std::bind(callback, std::move(result))); \
          }                                                                   \
        },                                                                    \
        std::forward<Args>(args)...,                                          \
        std::move(callback),                                                  \
        std::move(caller))));                                                 \
  }

/**
 * \brief Proxy class which can be used to access a servant from multiple
 * threads
 *
 * This implementation is based on the active object pattern.
 * TODO guide how to use this
 */
template <typename Servant>
class Proxy {
 public:
  template <typename... Args>
  Proxy(std::weak_ptr<Scheduler>&& scheduler, Args&&... args) {
    std::promise<std::shared_ptr<Servant>> promise;
    std::future<std::shared_ptr<Servant>> future = promise.get_future();

    if (std::shared_ptr<Scheduler> sharedScheduler = scheduler.lock()) {
      sharedScheduler.get()->push(std::move(std::bind(
          [&promise](std::weak_ptr<Scheduler> schedulerParam, Args... params) {
            promise.set_value(std::make_shared<Servant>(
                std::move(schedulerParam), std::forward<Args>(params)...));
          },
          std::move(scheduler),
          std::forward<Args>(args)...)));
    } else {
      assert(false);
    }
    _servant = future.get();
  }

  ~Proxy() {
    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    _servant.get()->push([this, &promise]() {
      this->_servant.reset();
      promise.set_value();
    });
    future.get();
  }

  Proxy(const Proxy<Servant>& other) = default;
  Proxy(Proxy&& other)               = default;
  Proxy<Servant>& operator=(const Proxy<Servant>& other) = default;
  Proxy<Servant>& operator=(Proxy&& other) = default;

 protected:
  std::shared_ptr<Servant> _servant;
};

#endif /* CORE_PROXY_HPP */
