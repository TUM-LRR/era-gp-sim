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

#include <bitset>
#include <chrono>
#include <memory>
#include <random>
#include <thread>
#include <vector>

// Include Google Test first
// clang-format off
#include "gtest/gtest.h"
#include "core/lockfree-queue.hpp"
#include "core/locking-queue.hpp"
// clang-format on

namespace {

constexpr std::size_t kProducerThreads = 4;
constexpr std::size_t kTestSize = 10000000;

// An object than can only be copied, but not moved
class CopyOnlyObject {
  std::size_t _value;
  
public:
  CopyOnlyObject(std::size_t v=0) : _value(v) {}
  CopyOnlyObject(const CopyOnlyObject& other) = default;
  CopyOnlyObject(CopyOnlyObject&& other) = delete;
  CopyOnlyObject& operator=(const CopyOnlyObject& other) = default;
  CopyOnlyObject& operator=(CopyOnlyObject&& other) = delete;
  
  explicit operator std::size_t() const { return _value; }
};

// An object that can only be moved, but not copied
class MoveOnlyObject {
  std::size_t _value;
  
public:
  MoveOnlyObject(std::size_t v=0) : _value(v) {}
  MoveOnlyObject(const MoveOnlyObject& other) = default;
  MoveOnlyObject(MoveOnlyObject&& other) = delete;
  MoveOnlyObject& operator=(const MoveOnlyObject& other) = default;
  MoveOnlyObject& operator=(MoveOnlyObject&& other) = delete;
  
  explicit operator std::size_t() const { return _value; }
};

// The producer functor
template <template <typename> class Queue, class Object>
class QueueProducer {
  std::shared_ptr<Queue<Object>> _queue;
  std::size_t _start;
  
public:
  QueueProducer(std::shared_ptr<Queue<Object>> queue, std::size_t start)
      : _queue(queue), _start(start) {}
  
  void operator()() {
    // Push every kProducerThread'th number to the queue, starting at _start.
    for (std::size_t index = _start; index < kTestSize; index += kProducerThreads) {
      _queue->push(Object(index));
    }
  }
};

// The consumer functor
template <template <typename> class Queue, class Object>
class QueueConsumer {
  std::shared_ptr<Queue<Object>> _queue;
  std::bitset<kTestSize> _verify;
  
public:
  QueueConsumer(std::shared_ptr<Queue<Object>> queue) : _queue(queue) {}
  
  void operator()() {
    Object object;
    // Pop all the objects from the queue
    for (std::size_t count = 0; count < kTestSize; ++count) {
      if (_queue->pop(object)) {
        std::size_t index = static_cast<std::size_t>(object);
        // Check that each value occurs only once
        ASSERT_FALSE(_verify[index]);
        _verify[index] = true;
      }
    }
    // The queue should be empty now
    ASSERT_FALSE(_queue->pop(object));
    // Check that all values occurred
    ASSERT_TRUE(_verify.all());
  }
};

// Test the given queue with the given object type
template <template <typename> class Queue, class Object>
void testQueue() {
  // Create queue
  std::shared_ptr<Queue<Object>> queue = std::make_shared<Queue<Object>>();
  
  // Create producer threads
  std::vector<std::thread> threads;
  for (std::size_t producer = 0; producer < kProducerThreads; ++producer) {
    threads.emplace_back(QueueProducer<Queue, Object>(queue, producer));
  }
  // Create consumer thread
  threads.emplace_back(QueueConsumer<Queue, Object>(queue));
  
  // Join all the threads
  for (auto& thread : threads) {
    thread.join();
  }
}

}

TEST(TestQueues, lockfreeQueue) {
  testQueue<LockfreeQueue, std::size_t>();
}
TEST(TestQueues, lockfreeQueueCopyOnly) {
  testQueue<LockfreeQueue, CopyOnlyObject>();
}
TEST(TestQueues, lockfreeQueueMoveOnly) {
  testQueue<LockfreeQueue, MoveOnlyObject>();
}

TEST(TestQueues, lockingQueue) {
  testQueue<LockingQueue, std::size_t>();
}
TEST(TestQueues, lockingQueueCopyOnly) {
  testQueue<LockingQueue, CopyOnlyObject>();
}
TEST(TestQueues, lockingQueueMoveOnly) {
  testQueue<LockingQueue, MoveOnlyObject>();
}
