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

#ifndef ERAGPSIM_CORE_LOCKFREE_QUEUE_HPP_
#define ERAGPSIM_CORE_LOCKFREE_QUEUE_HPP_

#include <atomic>
#include <thread>
#include <type_traits>

#include "core/lockfree-permanent-queue.hpp"
#include "core/lockfree-spsc-queue.hpp"
#include "core/tls.hpp"

/**
 * \brief A lockfree multi-producer, single-consumer queue.
 * 
 * Because \c thread_local is only allowed on variables with \c static storage
 * duration, this impementation maintains an internal (thread-safe) hash table
 * to map thread IDs to subqueues.
 * 
 * \tparam T The type of values stored within the queue.
 */
template <class T>
class LockfreeQueue {
 public:
 
  LockfreeQueue() {
    _tls_spsc_queue = tls::allocate<LockfreeSPSCQueue<T>>();
    _tls_was_registered = tls::allocate<bool>();
  }
  ~LockfreeQueue() = default;
  
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
  void push(U&& value) noexcept(noexcept(LockfreeSPSCQueue<T>::template push<U>)) {
    // If this producer is not registered yet, register it
    if (!*_tls_was_registered) {
      registerProducer();
      *_tls_was_registered = true;
    }
    // Fetch the proper sub-queue and push the value into that queue
    _tls_spsc_queue->push(std::forward<U>(value));
  }
  
  /**
   * \brief Try to pop a value from the queue.
   * 
   * \post If the queue contained an element, \c value now points to it. The
   *       element is removed from the queue.
   * 
   * \returns \c true if and only if an element was successfully removed from
   *          the queue.
   */
  bool pop(T& value) {
    // Iterate over all valid queues and attempt to pop from them
    auto visitor = [&value](tls::Pointer<LockfreeSPSCQueue<T>>& pointer) mutable {
      if (pointer.valid())
        if (pointer->pop(value))
          return true;
      return false;
    };
    return _registered_queues.apply(visitor);
  }
  
 private:
  /**
   * \brief Registers a new producer thread.
   * This adds the producer thread's local queue to the consumer's queue list
   */
  void registerProducer() {
    _registered_queues.push(_tls_spsc_queue.get());
  }
  
  /**
   * \brief A thread-local SPSC queue wrapper. This wrapper will be used to
   *        create the queues when each producer is registered
   */
  tls::ThreadLocal<LockfreeSPSCQueue<T>> _tls_spsc_queue;
  
  /**
   * \brief A boolean value stored in TLS to automatically register queues on
   *        first use from a specific thread
   */
  tls::ThreadLocal<bool> _tls_was_registered;
  
  /**
   * \brief A collection of TLS pointers to the SPSC queue(s).
   * When registering producers, their queues' \c tls::Pointer wrappers will be
   * added here. Before the consumer uses the queues, their \c valid() state 
   * should always be checked.
   */
  LockfreePermanentQueue<tls::Pointer<LockfreeSPSCQueue<T>>> _registered_queues;
};

#endif // ERAGPSIM_CORE_LOCKFREE_QUEUE_HPP_
