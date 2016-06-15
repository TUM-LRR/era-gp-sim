#ifndef LOCKFREE_SPSC_QUEUE_HPP_
#define LOCKFREE_SPSC_QUEUE_HPP_

#include <atomic>
#include <type_traits>

/**
 * \brief A lockfree single-producer, single-consumer (SPSC) queue.
 *
 * This implementation uses atomics to avoid the use of locks, but is only
 * threadsafe if there is at most one producer and consumer. Data is stored
 * within a simply linked list.
 *
 * For more information on SPSC queues, see for example
 *   "Single-Producer/Single-Consumer Queue", Daniel Vyukov (https://software.intel.com/en-us/articles/single-producer-single-consumer-queue)
 *   "Writing Lock-Free Code: A Corrected Queue", Herb Sutter (http://www.drdobbs.com/parallel/writing-lock-free-code-a-corrected-queue/210604448)
 * 
 * Internally, Nodes exist from \c _head to \c _tail, but only those from
 * \c _divider to \c _tail store values. The remainder (from \c _head to
 * \c _divider) will be cleaned up with the next push operation.
 * 
 * \tparam T The type of values stored within the queue.
 */
template <class T>
class LockfreeSPSCQueue {
 public:
  /// \brief Creates an empty single-producer, single-consumer queue.
  LockfreeSPSCQueue() {
    // Create an empty node and store it in the queue, as an "end" marker
    _head = _tail = _divider = new Node;
  }
  
  ~LockfreeSPSCQueue() {
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
  LockfreeSPSCQueue(const LockfreeSPSCQueue<T>& other) = delete;
  LockfreeSPSCQueue(LockfreeSPSCQueue&& other) = delete;
  LockfreeSPSCQueue<T>& operator=(const LockfreeSPSCQueue<T>& other) = delete;
  LockfreeSPSCQueue<T>& operator=(LockfreeSPSCQueue&& other) = delete;
  
  /**
   * \brief Copy a new value into the queue.
   * 
   * \pre \c T must be copy-constructible
   * 
   * \returns nothing.
   * \param value The value to copy into the queue
   */
  typename std::enable_if<std::is_copy_constructible<T>::value, void>::type
  push(const T& value) noexcept(std::is_nothrow_copy_constructible<T>::value) {
    Node* node = new Node { nullptr, value };
    appendNode(node);
  }
  
  /**
   * \brief Move a new value into the queue.
   * 
   * \pre \c T must be move-constructible.
   * 
   * \returns nothing.
   * \param value The value to move into the queue.
   */
  typename std::enable_if<std::is_move_constructible<T>::value, void>::type
  push(T&& value) noexcept(std::is_nothrow_move_constructible<T>::value) {
    Node* node = new Node { nullptr, std::move(value) };
    appendNode(node);
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
    if (_divider != _tail) {
      // The first unconsumed node in the queue
      Node* node = _divider.load(std::memory_order_acquire)->next;
      
      // Selects to cast the value to an rvalue reference (like std::move)
      // if and only if the value type can be move-assigned without an
      // exception occurring (via std::is_nothrow_move_assignable).
      using Target = typename std::conditional<std::is_nothrow_move_assignable<T>::value, T&&, T>::type;
      value = static_cast<Target>(node->value);
      
      // Consume this node
      _divider.store(node, std::memory_order_release);
      size.fetch_sub(1);
      
      return true;
    } else {
      return false;
    }
  }
  
  // Size of the internal list. Used for debugging
  std::atomic<std::size_t> size;
  
 private:
  /** \brief A node of the internal linked list */
  struct Node {
    Node* next;
    T value;
  };
  
  /// \brief The front end of the list, where the consumer will remove nodes
  Node* _head;
  
  /// \brief The tail end of the list, where the producer will add nodes
  std::atomic<Node*> _tail;
  
  /**
   * \brief Points to the first unconsumed node (nodes between \c _head and 
   *        \c _divider still need to be cleaned up)
   */
  std::atomic<Node*> _divider;
  
  /**
   * \brief Adds a node to the tail end of the queue.
   * \pre May only be called in the producer thread.
   * \see push
   */
  void appendNode(Node* node) noexcept {
    // Append new node
    _tail.load(std::memory_order_acquire)->next = node;
    _tail.store(node, std::memory_order_release);
    size.fetch_add(1);
    
    // Clean up old nodes
    while (_head != _divider) {
      Node* previous = _head;
      _head = _head->next;
      delete previous;
    }
  }
};

#endif // LOCKFREE_SPSC_QUEUE_HPP_
