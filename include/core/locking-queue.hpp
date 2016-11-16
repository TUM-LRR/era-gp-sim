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

#ifndef ERAGPSIM_CORE_LOCKING_QUEUE_HPP
#define ERAGPSIM_CORE_LOCKING_QUEUE_HPP

#include <deque>
#include <mutex>

/**
 * \brief A locking multi-producer queue.
 * \tparam T The type of values stored within the queue.
 */
template <class T>
class LockingQueue {
 public:
 
  LockingQueue() = default;
  ~LockingQueue() = default;
  
  LockingQueue(const LockingQueue<T>& other) = delete;
  LockingQueue(LockingQueue&& other) = default;
  LockingQueue<T>& operator=(const LockingQueue<T>& other) = delete;
  LockingQueue<T>& operator=(LockingQueue&& other) = default;
  
  /**
   * \brief Add a new value into the queue.
   * \pre \c T must be constructible from \c U&& via \c std::forward<U>.
   * \param value The value to add into the queue.
   */
  template <typename U>
  void push(U&& value) {
    std::lock_guard<std::mutex> lock(_mutex);
    using Target = typename std::conditional<std::is_move_assignable<U>::value,
                                             U&&, const U&>::type;
    _queue.emplace_back(static_cast<Target>(value));
  }
  
  /**
   * \brief Try to pop a value from the queue.
   * \post If the queue contained an element, \c value now points to it. The
   *       element is removed from the queue.
   * \returns \c true if and only if an element was successfully removed from
   *          the queue.
   */
  bool pop(T& value) {
    std::lock_guard<std::mutex> lock(_mutex);
    // If the queue is empty, return false
    if (_queue.size() == 0) return false;
    // Otherwise, cast the value to the proper target type (T&& or const T&)
    using Target = typename std::conditional<std::is_move_assignable<T>::value,
                                             T&&, const T&>::type;
    value = static_cast<Target>(_queue.front());
    // Remove the value from the queue and confirm success
    _queue.pop_front();
    return true;
  }
  
 private:
  /// \brief The internal queue used by this queue.
  std::deque<T> _queue;
  
  /// \brief The mutex protecting the internal queue
  std::mutex _mutex;
};

#endif // ERAGPSIM_CORE_LOCKED_QUEUE_HPP
