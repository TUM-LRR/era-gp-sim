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

// macro for functions without return value
#define POST(functionName)                             \
  template <typename... Args>                          \
  void functionName(Args&&... args) {                  \
    auto servant = _servant.get();                     \
    servant->push(std::bind(                           \
        [servant](Args... params) {                    \
          servant->functionName(std::move(params)...); \
        },                                             \
        std::forward<Args>(args)...));                 \
  }

// uses a future to get the return value, but returns the value with
// future.get(), therefore blocking
#define POST_FUTURE_BLOCKING(functionName)                             \
  template <typename... Args>                                          \
  auto functionName(Args&&... args) {                                  \
    auto servant = _servant.get();                                     \
    std::promise<decltype(servant->functionName(args...))> promise;    \
    servant->push(std::bind(                                           \
        [servant, &promise](Args... params) {                          \
          try {                                                        \
            auto result = servant->functionName(std::move(params)...); \
            promise.set_value(result);                                 \
          } catch (std::exception & e) {                               \
            promise.set_exception(std::make_exception_ptr(e));         \
          }                                                            \
        },                                                             \
        std::forward<Args>(args)...));                                 \
    return promise.get_future().get();                                 \
  }

// uses a future to get the return value and returns the future
#define POST_FUTURE(functionName)                                           \
  template <typename... Args>                                               \
  auto functionName(Args&&... args) {                                       \
    auto servant = _servant.get();                                          \
    std::shared_ptr<std::promise<decltype(servant->functionName(args...))>> \
        promise = std::make_shared<                                         \
            std::promise<decltype(servant->functionName(args...))>>();      \
    auto future = promise->get_future();                                    \
    servant->push(std::bind(                                                \
        [servant](std::shared_ptr<std::promise<decltype(                    \
                      servant->functionName(args...))>> promise,            \
                  Args... params) {                                         \
          try {                                                             \
            auto result = servant->functionName(std::move(params)...);      \
            promise->set_value(result);                                     \
          } catch (std::exception & e) {                                    \
            promise->set_exception(std::make_exception_ptr(e));             \
          }                                                                 \
        },                                                                  \
        std::move(promise),                                                 \
        std::forward<Args>(args)...));                                      \
    return future;                                                          \
  }

// handles the return value of the function with a callback, the callback is
// unsafe as it is not posted to the correct thread and the caller could be dead
// when the callback is executed
#define POST_CALLBACK_UNSAFE(functionName)                                   \
  template <typename R, typename... Args>                                    \
  void functionName##Unsafe(std::function<void(Result<R>)> callback,         \
                            Args&&... args) {                                \
    auto servant = _servant.get();                                           \
    servant->push(std::bind(                                                 \
        [servant](Args... params, std::function<void(Result<R>)> callback) { \
          Result<R> result;                                                  \
          try {                                                              \
            result.setValue(servant->functionName(std::move(params)...));    \
          } catch (std::exception & e) {                                     \
            result.setException(std::make_exception_ptr(e));                 \
          }                                                                  \
          callback(result);                                                  \
        },                                                                   \
        std::forward<Args>(args)...,                                         \
        std::move(callback)));                                               \
  }

// handles the return value of the function with a callback and makes sure that
// the caller is alive and the correct thread is used
#define POST_CALLBACK_SAFE(functionName)                                      \
  template <typename R, typename... Args>                                     \
  void functionName(std::function<void(Result<R>)> callback,                  \
                    std::weak_ptr<Servant> callerServant,                     \
                    Args&&... args) {                                         \
    auto servant = _servant.get();                                            \
    servant->push(std::bind(                                                  \
        [servant](Args... params,                                             \
                  std::function<void(Result<R>)> callback,                    \
                  std::weak_ptr<Servant> callerServant) {                     \
          Result<R> result;                                                   \
          try {                                                               \
            result.setValue(servant->functionName(std::move(params)...));     \
          } catch (std::exception & e) {                                      \
            result.setException(std::make_exception_ptr(e));                  \
          }                                                                   \
          if (std::shared_ptr<Servant> callerShared = callerServant.lock()) { \
            callerShared->push(std::bind(                                     \
                [](std::weak_ptr<Servant> weakServant,                        \
                   std::function<void(Result<R>)> callback,                   \
                   Result<R> result) {                                        \
                  if (auto servant = weakServant.lock()) {                    \
                    callback(result);                                         \
                  }                                                           \
                },                                                            \
                callerServant,                                                \
                std::move(callback),                                          \
                std::move(result)));                                          \
          }                                                                   \
        },                                                                    \
        std::forward<Args>(args)...,                                          \
        std::move(callback),                                                  \
        std::move(callerServant)));                                           \
  }

/**
 * \brief Proxy class which can be used to access a servant. Create a proxy
 * class for every servant class and inherit from this, the servant is then
 * created by the proxy
 *
 * This implementation is based on the active object pattern.
 * TODO guide how to use this
 */
template <typename Servant>
class Proxy {
 public:
  /**
   * Creates a Proxy and a shared_ptr on a servant. The servant is created in
   * its scheduler thread, to provide thread safety
   *
   */
  template <typename... Args>
  Proxy(std::weak_ptr<Scheduler>&& scheduler, Args&&... args) {
    std::promise<std::shared_ptr<Servant>> promise;
    std::future<std::shared_ptr<Servant>> future = promise.get_future();

    if (std::shared_ptr<Scheduler> sharedScheduler = scheduler.lock()) {
      sharedScheduler->push(std::bind(
          [&promise](std::weak_ptr<Scheduler> schedulerParam, Args... params) {
            promise.set_value(std::make_shared<Servant>(
                std::move(schedulerParam), std::forward<Args>(params)...));
          },
          std::move(scheduler),
          std::forward<Args>(args)...));
    } else {
      assert(false);
    }
    _servant = future.get();
  }

  /**
   * Destroys the proxy object and posts the reset of the shared_ptr to its
   * servant into the queue to provide safety for any calls on the servant which
   * are still in the scheduler queue
   *
   */
  ~Proxy() {
    // this if clause was needed to prevent a segfault after posting a function
    // object containing a proxy (for another servant) into the queue, it seems
    // like the destructor is called too often in that situation or on invalid
    // objects?
    if (_servant) {
      std::promise<void> promise;
      std::future<void> future = promise.get_future();
      _servant->push([this, &promise]() {
        this->_servant.reset();
        promise.set_value();
      });
      future.get();
    }
  }

  Proxy(const Proxy& other) = default;
  Proxy(Proxy&& other)      = default;
  Proxy& operator=(const Proxy& other) = default;
  Proxy& operator=(Proxy&& other) = default;

 protected:
  std::shared_ptr<Servant> _servant;
};

#endif /* CORE_PROXY_HPP */
