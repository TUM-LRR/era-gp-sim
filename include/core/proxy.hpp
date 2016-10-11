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
#include "common/tuple.hpp"

#ifndef ERAGPSIM_CORE_PROXY_HPP
#define ERAGPSIM_CORE_PROXY_HPP

/** \file proxy.hpp */

/**
 * \def POST(functionName)
 * This macro can be used to make a function without return value of a servant
 * safely accessible through a proxy.
 * Example: You have a servant class, with a method void foo() which does some
 * kind of work:
 *
 * \code
 * class ExampleServant: public Servant {
 * public:
 *
 *     ExampleServant(Proxy::WeakScheduler&& scheduler) :
 * Servant(std::move(scheduler)) {}
 *
 *     void foo() {
 *         //do work
 *     }
 * }
 * \endcode
 *
 * Now you create a proxy class for the servant, which handles all calls to the
 * servant (the constructor and destructor is handled, too!)
 *
 * \code
 * class ExampleProxy : public Proxy<ExampleServant> {
 * public:
 *
 *     ExampleProxy(Proxy::WeakScheduler&& scheduler) :
 * Proxy(std::move(scheduler)) {}
 *
 *     POST(foo)
 * }
 * \endcode
 *
 * To use the servant, you have to create a scheduler and a proxy class,
 * providing the scheduler to the proxy. The creation of the servant is done by
 * the proxy.
 * Now you can call foo() through the proxy and it is invoked safely:
 *
 * \code
 * std::shared_ptr<Scheduler> scheduler = std::make_shared<Scheduler>();
 * ExampleProxy proxy(std::move(scheduler));
 *
 * proxy.foo();
 *
 * \endcode
 *
 */
#define POST(functionName)                                                    \
  template <typename... Args>                                                 \
  void functionName(Args&&... args) {                                         \
    /* wrap the servant function in a lambda */                               \
    auto servant        = _servant.get();                                     \
    auto functionLambda = [servant](auto&&... args) {                         \
      servant->functionName(std::forward<decltype(args)>(args)...);           \
    };                                                                        \
    /* capture the functionLambda and the arguments as tuple, apply the tupel \
     * to the functionLambda*/                                                \
    auto task = [                                                             \
      functionLambda = std::move(functionLambda),                             \
      tuple          = std::make_tuple(std::forward<Args>(args)...)           \
    ]() mutable {                                                             \
      TupleApply::apply(std::move(functionLambda), std::move(tuple));  \
    };                                                                        \
    servant->push(std::move(task));                                           \
  }


/**
 * \def POST_FUTURE_BLOCKING(functionName)
 * This macro can be used to make a function with a return value of a servant
 * safely accessible through a proxy.
 * Example: You have a servant class, with a method int bar() which does some
 * kind of work and returns a result.
 *
 * \code
 * class ExampleServant: public Servant {
 * public:
 *
 *     ExampleServant(Proxy::WeakScheduler&& scheduler) :
 * Servant(std::move(scheduler)) {}
 *
 *     int bar() {
 *         //do work
 *         return x;
 *     }
 * }
 * \endcode
 *
 * Now you create a proxy class for the servant, which handles all calls to the
 * servant (the constructor and destructor is handled, too!)
 *
 * \code
 * class ExampleProxy : public Proxy<ExampleServant> {
 * public:
 *
 *     ExampleProxy(Proxy::WeakScheduler&& scheduler) :
 * Proxy(std::move(scheduler)) {}
 *
 *     POST_FUTURE_BLOCKING(bar)
 * }
 * \endcode
 *
 * To use the servant, you have to create a scheduler and a proxy class,
 * providing the scheduler to the proxy. The creation of the servant is done by
 * the proxy.
 * Now you can call bar() through the proxy and it is invoked safely:
 *
 * \code
 * std::shared_ptr<Scheduler> scheduler = std::make_shared<Scheduler>();
 * ExampleProxy proxy(std::move(scheduler));
 *
 * int y = proxy.bar();
 *
 * \endcode
 *
 * This macro is called BLOCKING because the invocation of bar() blocks the
 * current thread through future.get() until the result was calculated in the
 * servant thread.
 *
 * Of course, you can also pass arguments to bar. This is true for all of these
 * macros.
 *
 */
#define POST_FUTURE_BLOCKING(functionName)                                 \
  template <typename... Args>                                              \
  auto functionName(Args&&... args) {                                      \
    /* wrap the servant function in a lambda, return result */             \
    auto servant        = _servant.get();                                  \
    auto functionLambda = [servant](auto&&... args) {                      \
      return servant->functionName(std::forward<decltype(args)>(args)...); \
    };                                                                     \
    /* create a task lambda and capture the functionLambda, a tuple of the \
     * arguments and a promise. In the lambda, the tuple is applied to the \
     * function and the promise is set with the result of the function */  \
    std::promise<decltype(servant->functionName(args...))> promise;        \
    auto task = [                                                          \
      functionLambda = std::move(functionLambda),                          \
      tuple          = std::make_tuple(std::forward<Args>(args)...),       \
      &promise                                                             \
    ]() mutable {                                                          \
      try {                                                                \
        auto result = TupleApply::apply(std::move(functionLambda),  \
                                               std::move(tuple));          \
        promise.set_value(result);                                         \
      } catch (std::exception & e) {                                       \
        promise.set_exception(std::make_exception_ptr(e));                 \
      }                                                                    \
    };                                                                     \
    servant->push(std::move(task));                                        \
    return promise.get_future().get();                                     \
  }

/**
 * \def POST_FUTURE(functionName)
 * Almost the same usage of POST_FUTURE_BLOCKING, but a invocation of a method
 * defined by POST_FUTURE will return a future with the result, not the result
 * directly.
 *
 * Because of this, it does not directly block the calling thread, only if
 * future.get() is called.
 *
 * \code
 * //in the proxy, same servant as in POST_FUTURE_BLOCKING
 * POST_FUTURE(bar)
 *
 * //somewhere else
 *
 * //this doesn't block anymore
 * std::future<int> future = proxy.bar();
 *
 * //this could block now
 * int y = future.get();
 * \endcode
 * \see POST_FUTURE_BLOCKING(functionName)
 */
#define POST_FUTURE(functionName)                                              \
  template <typename... Args>                                                  \
  auto functionName(Args&&... args) {                                          \
    /* wrap the servant function in a lambda, return result */                 \
    auto servant        = _servant.get();                                      \
    auto functionLambda = [servant](auto&&... args) {                          \
      return servant->functionName(std::forward<decltype(args)>(args)...);     \
    };                                                                         \
    /* unique_ptr can't be used, because the task (a std::function object) has \
     * to be copyable*/                                                        \
    std::shared_ptr<std::promise<decltype(servant->functionName(args...))>>    \
        promiseShared = std::make_shared<                                      \
            std::promise<decltype(servant->functionName(args...))>>();         \
    auto future = promiseShared->get_future();                                 \
    /* create a task lambda and capture the functionLambda, a tuple of the     \
     * arguments and the shared_ptr to the promise. In the lambda, the tuple   \
     * is applied to the function and the promise is set with the result of    \
     * the function  */                                                        \
    auto task = [                                                              \
      function = std::move(functionLambda),                                    \
      tuple    = std::make_tuple(std::forward<Args>(args)...),                 \
      promise  = std::move(promiseShared)                                      \
    ]() mutable {                                                              \
      try {                                                                    \
        auto result =                                                          \
            TupleApply::apply(std::move(function), std::move(tuple));   \
        promise->set_value(result);                                            \
      } catch (std::exception & e) {                                           \
        promise->set_exception(std::make_exception_ptr(e));                    \
      }                                                                        \
    };                                                                         \
                                                                               \
    servant->push(std::move(task));                                            \
    return future;                                                             \
  }

/**
 * \def POST_CALLBACK_UNSAFE(functionName)
 * handles the return value of the function with a callback, the callback is
 * unsafe as it is not posted to the correct thread and the caller could be dead
 * when the callback is executed.
 *
 * If you use the same example as for see POST_CALLBACK_SAFE(functionName), you
 * have to use makeSafeCallback with the invocation of doWork (in the call()
 * method):
 *
 * \code
 *  class CallerServant : public Servant {
 *  public:
 *      //constructor [...]
 *
 *      void callback(Result<int> result) {
 *          //this could rethrow an exception
 *          int i = result.get();
 *          //do something with the result
 *      }
 *
 *      void call() {
 *          std::function<void(Result<int>)> callback = [this](Result<int>
 * result) {callback(result);};
 *          proxyCallbackServant.doWorkUnsafe(makeSafeCallback(
 *                                                        std::move(callback));
 *      }
 * }
 * \endcode
 * Note that you have to call doWorkUnsafe, as the Unsafe macro adds Unsafe to
 * the name of the function.
 *
 * \see POST_CALLBACK_SAFE(functionName)
 */
#define POST_CALLBACK_UNSAFE(functionName)                                  \
  template <typename R, typename... Args>                                   \
  void functionName##Unsafe(std::function<void(Result<R>)> callback,        \
                            Args&&... args) {                               \
    /* wrap the servant function in a lambda, return result */              \
    auto servant        = _servant.get();                                   \
    auto functionLambda = [servant](auto&&... args) {                       \
      return servant->functionName(std::forward<decltype(args)>(args)...);  \
    };                                                                      \
    auto task = [                                                           \
      functionLambda = std::move(functionLambda),                           \
      tuple          = std::make_tuple(std::forward<Args>(args)...),        \
      callback       = std::move(callback)                                  \
    ]() mutable {                                                           \
      /* A result object is created and the value/exception is set */       \
      Result<R> result;                                                     \
      try {                                                                 \
        result.setValue(TupleApply::apply(std::move(functionLambda), \
                                                 std::move(tuple)));        \
      } catch (std::exception & e) {                                        \
        result.setException(std::make_exception_ptr(e));                    \
      }                                                                     \
      /* the callback is called, the callback has to handle all checks for  \
       *dead                                                                \
       *objects and has to push the callback task to the right thread */    \
      callback(result);                                                     \
    };                                                                      \
    servant->push(std::move(task));                                         \
  }

/**
 * \def POST_CALLBACK_SAFE(functionName)
 * handles the return value of the function with a callback and makes sure that
 * the caller is alive and the correct thread is used. Caller and Callee both
 * have to be servants (active objects)
 *
 * Example:
 *
 * There are two servants, defined as the following:
 *
 * \code
 *  class CallbackServant : public Servant {
 *  public:
 *      //constructor [...]
 *
 *      int doWork() {
 *          //do something and return a int
 *          return x;
 *      }
 *      // [...]
 *  }
 * \endcode
 *
 * \code
 *  class CallerServant : public Servant {
 *  public:
 *      //constructor [...]
 *
 *      void callback(Result<int> result) {
 *          //this could rethrow an exception
 *          int i = result.get();
 *          //do something with the result
 *      }
 *
 *      void call() {
 *          std::function<void(Result<int>)> callback = [this](Result<int>
 * result) {callback(result);};
 *          proxyCallbackServant.doWork(callback, shared_from_this());
 *      }
 *  }
 * \endcode
 *
 * The proxy for the CallbackServant is defined as follows:
 *
 * \code
 *  class CallbackServantProxy : public Proxy<CallbackServant> {
 *      //constructor [...]
 *
 *      POST_CALLBACK_SAFE(doWork)
 *  }
 * \endcode
 *
 * The definition of the proxy for the CallerServant is not relevant for this
 * example.
 *
 * If the call() method of the CallerServant is executed, it calls the doWork()
 * method in the CallbackServant, which in turn calls the CallerServant back
 * with the provided method.
 */
#define POST_CALLBACK_SAFE(functionName)                                      \
  template <typename R, typename... Args>                                     \
  void functionName(std::function<void(Result<R>)> callback,                  \
                    std::weak_ptr<Servant> callerServant,                     \
                    Args&&... args) {                                         \
    /* wrap the servant function in a lambda, return result */                \
    auto servant        = _servant.get();                                     \
    auto functionLambda = [servant](auto&&... args) {                         \
      return servant->functionName(std::forward<decltype(args)>(args)...);    \
    };                                                                        \
    auto task = [                                                             \
      functionLambda = std::move(functionLambda),                             \
      tuple          = std::make_tuple(std::forward<Args>(args)...),          \
      callerServant  = std::move(callerServant),                              \
      callback       = std::move(callback)                                    \
    ]() mutable {                                                             \
      /* A result object is created and the value/exception is set */         \
      Result<R> result;                                                       \
      try {                                                                   \
        result.setValue(TupleApply::apply(std::move(functionLambda),   \
                                                 std::move(tuple)));          \
      } catch (std::exception & e) {                                          \
        result.setException(std::make_exception_ptr(e));                      \
      }                                                                       \
      /* A callback lambda is created, which checks if the servant, where the \
       * callback is executed, is still alive and executes the callback if    \
       * this is the case */                                                  \
      auto callbackTask = [                                                   \
        weakServant = std::move(callerServant),                               \
        callback    = std::move(callback),                                    \
        result      = std::move(result)                                       \
      ] {                                                                     \
        if (auto servant = weakServant.lock()) {                              \
          callback(result);                                                   \
        }                                                                     \
      };                                                                      \
      /* if the calling servant is still alive, push the callback task into   \
       * its queue */                                                         \
      if (auto callerShared = callerServant.lock()) {                         \
        callerShared->push(callbackTask);                                     \
      }                                                                       \
    };                                                                        \
    servant->push(std::move(task));                                           \
  }

/**
 * \brief Proxy class which can be used to access a servant. Create a proxy
 * class for every servant class and inherit from this, the servant is then
 * created by the proxy
 *
 * This implementation is based on the active object pattern.
 *
 * Examples for the usage of the proxy and the threading classes in general are
 * in the documentation of the macros:
 *
 * \see POST(functionName) POST_FUTURE_BLOCKING(functionName)
 * POST_FUTURE(functionName) POST_CALLBACK_UNSAFE(functionName)
 * POST_CALLBACK_SAFE(functionName) task-scheduler-tests.cpp
 *
 * For complete, working code, you can look at the task-scheduler-tests.
 */
template <typename Servant>
class Proxy {
 public:
     using SharedServant = std::shared_ptr<Servant>;
     using ServantPromise = std::promise<SharedServant>;
     using WeakScheduler = std::weak_ptr<Scheduler>;

  /**
   * Creates a Proxy and a shared_ptr on a servant. The servant is created in
   * its scheduler thread, to provide thread safety
   *
   */
  template <typename... Args>
  Proxy(WeakScheduler&& scheduler, Args&&... args) {
    ServantPromise promise;
    auto future = promise.get_future();

    if (auto sharedScheduler = scheduler.lock()) {
      /* wrap the creation of the servant in a lambda, this has to be done to
       * allow for forwarding of the arguments by using TupleApply::apply
       * with this lambda */
      auto servantConstructionLambda = [&promise](auto&&... args) {
        promise.set_value(
            std::make_shared<Servant>(std::forward<decltype(args)>(args)...));
      };
      /* create a task which applies the weak_ptr to the scheduler and the
       * captured arguments to the servant construction lambda */
      auto task = [
        &promise,
        servantConstructionLambda = std::move(servantConstructionLambda),
        tuple =
            std::make_tuple(std::move(scheduler), std::forward<Args>(args)...)
      ]() mutable {
        TupleApply::apply(std::move(servantConstructionLambda),
                                 std::move(tuple));
      };
      sharedScheduler->push(std::move(task));
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
  /* A shared_ptr to the servant of this proxy */
  SharedServant _servant;
};

#endif /* ERAGPSIM_CORE_PROXY_HPP */
