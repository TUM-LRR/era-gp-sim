/* C++ Assembler Interpreter
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

#include <atomic>
#include <exception>
#include <functional>
#include <memory>
#include <thread>

// clang-format off
#include "gtest/gtest.h"
#include "core/proxy.hpp"
#include "core/result.hpp"
#include "core/scheduler.hpp"
#include "core/servant.hpp"
// clang-format on

///////////////////////////////////////////////////////////////////////////////
// Servant class to test calls from one servant (Testservant1) to another
// servant (Testservant2)
class Testservant2 : public Servant {
 public:
  Testservant2(std::weak_ptr<Scheduler>&& scheduler)
  : Servant(std::move(scheduler)), counterServant2(0) {
  }

  void testPost(int counter,
                std::thread::id testThreadId,
                std::thread::id servant1ThreadId) {
    EXPECT_EQ(counter, counterServant2);
    EXPECT_NE(servant1ThreadId, std::this_thread::get_id());
    EXPECT_NE(testThreadId, std::this_thread::get_id());
    EXPECT_EQ(std::this_thread::get_id(), _scheduler.lock()->getThreadId());
    counterServant2++;
  }

  int testCall(int counter,
               std::thread::id testThreadId,
               std::thread::id servant1ThreadId) {
    EXPECT_EQ(counter, counterServant2);
    EXPECT_NE(servant1ThreadId, std::this_thread::get_id());
    EXPECT_NE(testThreadId, std::this_thread::get_id());
    EXPECT_EQ(std::this_thread::get_id(), _scheduler.lock()->getThreadId());
    counterServant2++;
    return counter;
  }


  int testException() {
    throw std::exception();
  }

  int testExceptionBlocking() {
    throw std::exception();
  }


  int counterServant2;
};

class Testproxy2 : public Proxy<Testservant2> {
 public:
  Testproxy2(std::weak_ptr<Scheduler>&& scheduler)
  : Proxy(std::move(scheduler)) {
  }

  POST(testPost)
  POST_CALLBACK_UNSAFE(testCall)
  POST_CALLBACK_SAFE(testCall)

  POST_FUTURE_BLOCKING(testExceptionBlocking)
  POST_FUTURE(testException)
  POST_CALLBACK_UNSAFE(testException)
  POST_CALLBACK_SAFE(testException)
};

///////////////////////////////////////////////////////////////////////////////
// Servant class to test simple posts, futures and calls to another servant
// (Testservant2)
class Testservant1 : public Servant {
 public:
  Testservant1(std::weak_ptr<Scheduler>&& scheduler)
  : Servant(std::move(scheduler))
  , counterServant1(0)
  , testCounter(-1)
  , servant1ThreadId(std::this_thread::get_id()) {
  }

  // methods
  void testPost(int counter, std::thread::id testThreadId) {
    ASSERT_EQ(counter, counterServant1);
    ASSERT_NE(testThreadId, std::this_thread::get_id());
    EXPECT_EQ(std::this_thread::get_id(), _scheduler.lock()->getThreadId());
    counterServant1++;
  }

  int testFuture(int counter, std::thread::id testThreadId) {
    EXPECT_EQ(counter, counterServant1);
    EXPECT_NE(testThreadId, std::this_thread::get_id());
    EXPECT_EQ(std::this_thread::get_id(), _scheduler.lock()->getThreadId());
    counterServant1++;
    return counter;
  }

  int testFutureNonBlocking(int counter, std::thread::id testThreadId) {
    EXPECT_EQ(counter, counterServant1);
    EXPECT_NE(testThreadId, std::this_thread::get_id());
    EXPECT_EQ(std::this_thread::get_id(), _scheduler.lock()->getThreadId());
    counterServant1++;
    return counter;
  }

  void testCallback(Result<int> result) {
    EXPECT_EQ(result.get(), testCounter + 1);
    EXPECT_EQ(servant1ThreadId, std::this_thread::get_id());
    EXPECT_EQ(std::this_thread::get_id(), _scheduler.lock()->getThreadId());
    testCounter = result.get();
  }

  void postCallbackUnsafeTest(int counter,
                              std::thread::id testThreadId,
                              std::weak_ptr<Scheduler> scheduler1,
                              Testproxy2 servant2Proxy) {
    EXPECT_EQ(counter, counterServant1);
    EXPECT_NE(testThreadId, std::this_thread::get_id());
    EXPECT_EQ(std::this_thread::get_id(), _scheduler.lock()->getThreadId());
    servant1ThreadId = std::this_thread::get_id();
    counterServant1++;
    std::function<void(Result<int>)> lambdaCallback =
        [this](Result<int> result) { testCallback(result); };
    servant2Proxy.testCallUnsafe(makeSafeCallback(std::move(lambdaCallback)),
                                 counter,
                                 testThreadId,
                                 servant1ThreadId);
  }

  void postCallbackTest(int counter,
                        std::thread::id testThreadId,
                        Testproxy2 servant2Proxy) {
    EXPECT_EQ(counter, counterServant1);
    EXPECT_NE(testThreadId, std::this_thread::get_id());
    EXPECT_EQ(std::this_thread::get_id(), _scheduler.lock()->getThreadId());
    counterServant1++;
    std::function<void(Result<int>)> callback = [this](Result<int> result) {
      testCallback(result);
    };
    std::weak_ptr<Servant> weak = shared_from_this();
    servant2Proxy.testCall(
        callback, weak, counter, testThreadId, servant1ThreadId);
  }

  void postDoubleTest(int counter,
                      std::thread::id testThreadId,
                      Testproxy2 servant2Proxy) {
    EXPECT_EQ(counter, counterServant1);
    EXPECT_NE(testThreadId, std::this_thread::get_id());
    servant1ThreadId = std::this_thread::get_id();
    counterServant1++;
    servant2Proxy.testPost(counter, testThreadId, servant1ThreadId);
  }

  void throwCallback(Result<int> result) {
    EXPECT_ANY_THROW(result.get());
  }

  void testExceptionPost(Testproxy2 servant2Proxy) {
    EXPECT_ANY_THROW(servant2Proxy.testExceptionBlocking());
    EXPECT_ANY_THROW(servant2Proxy.testException().get());
    std::function<void(Result<int>)> lambdaCallback =
        [this](Result<int> result) { throwCallback(result); };
    servant2Proxy.testException<int>(lambdaCallback, shared_from_this());
    servant2Proxy.testExceptionUnsafe(
        makeSafeCallback(std::move(lambdaCallback)));
  }


  int testCounter;
  std::thread::id servant1ThreadId;
  int counterServant1;
};


class Testproxy1 : public Proxy<Testservant1> {
 public:
  Testproxy1(std::weak_ptr<Scheduler>&& scheduler)
  : Proxy(std::move(scheduler)) {
  }

  POST(testPost)
  POST_FUTURE_BLOCKING(testFuture)
  POST_FUTURE(testFutureNonBlocking)
  POST(postCallbackUnsafeTest)
  POST(postCallbackTest)
  POST(postDoubleTest)
  POST(testExceptionPost)
};

///////////////////////////////////////////////////////////////////////////////
// Servant to test the constructor and destructor without the test fixture
class Testservant3 : public Servant {
 public:
  Testservant3(std::weak_ptr<Scheduler>&& scheduler,
               std::thread::id testThreadId)
  : Servant(std::move(scheduler)), testThreadId(testThreadId) {
    assertThreadId();
  }

  ~Testservant3() {
    assertThreadId();
  }

  // assert that the constructor and destructor run in the scheduler thread, not
  // in the test thread
  void assertThreadId() {
    EXPECT_NE(testThreadId, std::this_thread::get_id());
    EXPECT_EQ(_scheduler.lock()->getThreadId(), std::this_thread::get_id());
    EXPECT_EQ(std::this_thread::get_id(), _scheduler.lock()->getThreadId());
  }

  std::thread::id testThreadId;
};

class Testproxy3 : public Proxy<Testservant3> {
 public:
  Testproxy3(std::weak_ptr<Scheduler>&& scheduler, std::thread::id id)
  : Proxy(std::move(scheduler), id) {
  }
};


///////////////////////////////////////////////////////////////////////////////
// Testfixture
class ThreadingTestFixture : public ::testing::Test {
 public:
  ThreadingTestFixture()
  : scheduler1(std::make_shared<Scheduler>())
  , scheduler2(std::make_shared<Scheduler>())
  , proxy1(std::move(scheduler1))
  , proxy2(std::move(scheduler2)) {
  }

  std::shared_ptr<Scheduler> scheduler1;
  std::shared_ptr<Scheduler> scheduler2;
  Testproxy1 proxy1;
  Testproxy2 proxy2;
  int testRepeat = 10;
};

///////////////////////////////////////////////////////////////////////////////


// Tests if constructor and destructor are running in the scheduler thread
TEST(ThreadingTestProxy, constructorTest) {
  std::shared_ptr<Scheduler> scheduler = std::make_shared<Scheduler>();
  Testproxy3 proxy(std::move(scheduler), std::this_thread::get_id());
}

// test posting a simple task(without return value)
TEST_F(ThreadingTestFixture, postTask) {
  for (int i = 0; i < testRepeat; i++) {
    proxy1.testPost(i, std::this_thread::get_id());
  }
}

// tests posting a task and directly receiving the return value(like a normal
// function call)(implemented with a future internally)
TEST_F(ThreadingTestFixture, postFutureBlocking) {
  for (int i = 0; i < testRepeat; i++) {
    int test = proxy1.testFuture(i, std::this_thread::get_id());
    ASSERT_EQ(test, i);
  }
}

// tests posting a task and receiving the return value through a future
TEST_F(ThreadingTestFixture, postFuture) {
  for (int i = 0; i < testRepeat; i++) {
    std::future<int> test =
        proxy1.testFutureNonBlocking(i, std::this_thread::get_id());
    ASSERT_EQ(test.get(), i);
  }
}

// tests calling a simple method of servant1 (POST) and servant1 calling a
// simple method of servant2
TEST_F(ThreadingTestFixture, doublePost) {
  for (int i = 0; i < testRepeat; i++) {
    proxy1.postDoubleTest(i, std::this_thread::get_id(), proxy2);
  }
}

// tests receiving a result through a callback: test -> servant1 -> call in
// servant2 -> callback in servant1 (with the POST_CALLBACK_UNSAFE macro)
TEST_F(ThreadingTestFixture, postCallbackUnsafe) {
  for (int i = 0; i < testRepeat; i++) {
    proxy1.postCallbackUnsafeTest(
        i, std::this_thread::get_id(), scheduler1, proxy2);
  }
  // wait just a bit, so that the callbacks can be (partially) executed
  std::this_thread::sleep_for(std::chrono::milliseconds(testRepeat * 5));
}

// tests receiving a result through a callback: test -> servant1 -> call in
// servant2 -> callback in servant1 (with the POST_CALLBACK_SAFE macro)
TEST_F(ThreadingTestFixture, postCallback) {
  for (int i = 0; i < testRepeat; i++) {
    proxy1.postCallbackTest(i, std::this_thread::get_id(), proxy2);
  }
  // wait just a bit, so that the callbacks can be (partially) executed
  std::this_thread::sleep_for(std::chrono::milliseconds(testRepeat * 5));
}

// tests if exceptions are correctly thrown when receiving a result through a
// future or callback
TEST_F(ThreadingTestFixture, exceptionTest) {
  proxy1.testExceptionPost(proxy2);
  // wait just a bit, so that the callbacks can be (partially) executed
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}
