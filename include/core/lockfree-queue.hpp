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

#ifndef ERAGPSIM_CORE_LOCKFREE_QUEUE_HPP
#define ERAGPSIM_CORE_LOCKFREE_QUEUE_HPP

#include <atomic>
#include <type_traits>

/**
 * \brief A lockfree multi-producer, single-consumer queue.
 *
 * This implementation uses atomics to avoid the use of locks, but is only
 * threadsafe if there is at most one producer and consumer. Data is stored
 * within a simply linked list.
 *
 * For more information on SPSC queues, on which this implementation is based,
 * see for example
 *   "Single-Producer/Single-Consumer Queue", Daniel Vyukov (https://software.intel.com/en-us/articles/single-producer-single-consumer-queue)
 *   "Writing Lock-Free Code: A Corrected Queue", Herb Sutter (http://www.drdobbs.com/parallel/writing-lock-free-code-a-corrected-queue/210604448)
 * 
 * \tparam T The type of values stored within the queue.
 */
template <class T>
class LockfreeQueue {
 public:
  /// \brief Creates an empty single-producer, single-consumer queue.
  LockfreeQueue() {
    _head = _tail = new Node;
  }
  
  ~LockfreeQueue() {
    // Iterate over the queue nodes and delete them
    while (_head != nullptr)
    {
      Node* next = _head->next;
      delete _head;
      _head = next;
    }
  }
  
  // Queues can neither be moved nor copied, because there is no way to
  // guarantee a lock-free copy/move of multiple atomics without introducing
  // locking behavior.
  LockfreeQueue(const LockfreeQueue<T>& other) = delete;
  LockfreeQueue(LockfreeQueue&& other) = delete;
  LockfreeQueue<T>& operator=(const LockfreeQueue<T>& other) = delete;
  LockfreeQueue<T>& operator=(LockfreeQueue&& other) = delete;
  
  /**
   * \brief Add a new value into the queue.
   * \pre \c T must be constructible from \c U&& via \c std::forward<U>.
   * \param value The value to add into the queue.
   */
  template <typename U>
  void push(U&& value) {
    // Create new node
    using Target = typename std::conditional<std::is_move_assignable<U>::value, 
                                             U&&, const U&>::type;
    Node* node = new Node { nullptr, static_cast<Target>(value) };
    
    // Add the new node
    while (true) {
      Node* previous = _tail.load(std::memory_order_acquire);
      if (_tail.compare_exchange_strong(previous, node)) {
        previous->next = node;
        break;
      }
    }
  }
  
  /**
   * \brief Try to pop a value from the queue.
   * \post If the queue contained an element, \c value now points to it. The
   *       element is removed from the queue.
   * \returns \c true if and only if an element was successfully removed from
   *          the queue.
   * 
   * \c pop is not thread-safe with regards to other calls to \c pop - only one
   * consumer may use the queue concurrently. No such limitations exist on the 
   * use of \c push, and even multiple invocations of \c push may be concurrent
   * to one invocation of \c pop.
   */
  bool pop(T& value) {
    if (_head != _tail.load(std::memory_order_acquire)) {
      // The first unconsumed node in the queue
      Node* node = _head->next;
      
      if (node == nullptr) return false;
      
      // Selects to cast the value to an rvalue reference (like std::move)
      // if and only if the value type can be move-assigned (via
      // std::is_move_assignable), otherwise use the normal copy constructor.
      using Target = typename std::conditional<std::is_move_assignable<T>::value, 
                                               T&&, const T&>::type;
      value = static_cast<Target>(node->value);
      
      // Consume this node
      Node* previous = _head;
      _head = node;
      delete previous;
      
      // Confirm success
      return true;
    } else {
      // The queue is empty, no nodes are available.
      return false;
    }
  }
  
 private:
  /// \brief A node of the internal linked list
  struct Node {
    Node* next;
    T value;
  };
  
  /// \brief The front end of the list, where the consumer will remove nodes
  Node* _head;
  
  /// \brief The tail end of the list, where the producer will add nodes
  std::atomic<Node*> _tail;
};

#endif // ERAGPSIM_CORE_LOCKFREE_QUEUE_HPP
