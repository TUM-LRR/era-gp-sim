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

#ifndef ERAGPSIM_CORE_LOCKFREE_PERMANENT_QUEUE_HPP_
#define ERAGPSIM_CORE_LOCKFREE_PERMANENT_QUEUE_HPP_

#include <atomic>
#include <type_traits>

/**
 * \brief A lockfree "queue" from which elements may not be removed.
 *
 * This implementation uses atomics to avoid the use of locks, and is threadsafe
 * to access from any number of threads.
 *
 * \tparam T The type of values stored within the queue.
 */
template <class T>
class LockfreePermanentQueue {
 public:
  /// \brief Creates an empty queue.
  LockfreePermanentQueue() {
    // Create an empty sentinel node
    _head = new Node;
    _tail = &_head->next;
  }
  
  ~LockfreePermanentQueue() {
    // Iterate over the queue nodes and delete them
    while (_head != nullptr)
    {
      Node* next = _head->next;
      delete _head;
      _head = next;
    }
  }
  
  // Queues can neither be moved nor copied. It might be possible to support
  // moving permanent queues, but support for this is not currently necessary.
  LockfreePermanentQueue(const LockfreePermanentQueue<T>& other) = delete;
  LockfreePermanentQueue(LockfreePermanentQueue&& other) = delete;
  LockfreePermanentQueue<T>& operator=(const LockfreePermanentQueue<T>& other) = delete;
  LockfreePermanentQueue<T>& operator=(LockfreePermanentQueue&& other) = delete;
  
  /**
   * \brief Add a new value into the queue.
   * \pre \c T must be constructible from \c U&& via \c std::forward<U>.
   * \param value The value to add into the queue.
   */
  template <typename U>
  void push(U&& value) noexcept(std::is_nothrow_constructible<T, decltype(std::forward<U>(value))>::value) {
    // Create new node
    Node* node = new Node { std::forward<U>(value) };
    
    // Get an "iterator" pointer
    std::atomic<Node*>* target = _tail.load();
    
    // The value to be replaced (nullptr), as an lvalue for use in 
    // compare_exchange_strong
    Node* expected = nullptr;
    
    // Go through the list, and attempt to replace the first null pointer with
    // the new node
    while (!target->compare_exchange_strong(expected, node)) {
      target = &expected->next;
      expected = nullptr;
    }
    
    _tail.store(&target->load()->next);
  }
  
  /**
   * \brief Applies a functor to each element in the queue
   * \param fn The functor to apply to the elements
   * \tparam F The type of the functor
   * \return Whether the iteration was aborted by the functor
   * If the functor returns a truthy value, iteration is aborted.
   */
  template <class F>
  bool apply(F&& fn) {
    // Iterate.
    for (std::atomic<Node*>* target = &_head->next;
         target != nullptr && target->load() != nullptr;
         target = &target->load()->next) {
      // Apply the functor to the current element
      if (fn(target->load()->value)) return true;
    }
    return false;
  }
  
 private:
  /// \brief A node of the internal linked list
  struct Node {
    std::atomic<Node*> next;
    T value;
    
    Node() : next(nullptr) {}
    Node(const T& t) : next(nullptr), value(t) {}
    Node(T&& t) : next(nullptr), value(std::forward<T>(t)) {}
  };
  
  /// \brief The front end of the list (sentinel node)
  Node* _head;
  
  /**
   * \brief A "hint" for the tail end of the list, to facilitate insertion
   * This is an atomic pointer to one of the \c next pointers in a list node
   */
  std::atomic<std::atomic<Node*>*> _tail;
};

#endif // ERAGPSIM_CORE_LOCKFREE_PERMANENT_QUEUE_HPP_
