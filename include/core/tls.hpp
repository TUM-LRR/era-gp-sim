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

#ifndef ERAGPSIM_CORE_TLS_HPP_
#define ERAGPSIM_CORE_TLS_HPP_

#include <atomic>
#include <functional>
#include <set>
#include <type_traits>
#include <vector>


#ifdef ERAGPSIM_CORE_TLS_ENABLE_TRACKING
#include <iostream>
#include <ostream>
#include <sstream>
#include <typeinfo>
#endif // ERAGPSIM_CORE_TLS_ENABLE_TRACKING

/**
 * \brief A data structure to allow storage of custom data in \c thread_local 
 *        storage, which is usually reserved for static variables and members.
 */
namespace tls {
  
// Thread-local storage: internals
namespace detail {
  
/**
 * \brief A helper object calling any registered destructors when it is
 *        destroyed. This ensures that object destructors are called prior to
 *        deallocation of the TLS memory
 */
class TlsDestructionHelper {
 public:
  // TlsDestructionHelper can neither be copied nor moved, but can be default-constructed
  TlsDestructionHelper() = default;
  TlsDestructionHelper(const TlsDestructionHelper& other) = delete;
  TlsDestructionHelper(TlsDestructionHelper&& other) = delete;
  TlsDestructionHelper& operator=(const TlsDestructionHelper& other) = delete;
  TlsDestructionHelper& operator=(TlsDestructionHelper&& other) = delete;
 
  /**
   * \brief Destroys the \c TlsDestructionHelper, and calls the destructors
   *        stored within this object
   */
  ~TlsDestructionHelper() {
    for (const auto& destructor : _destructors) {
      destructor();
    }
  }
 
  /**
   * \brief Registers a destructor for an object of type \c T at the specified
   *        location in memory (\c pointer), to be invoked upon destruction of
   *        this object.
   * \tparam T The type of which the destructor will be called.
   * \param pointer A pointer to the targeted object.
   */
  template <class T>
  void add(void* pointer) {
    _destructors.emplace_back([pointer]() mutable {
      reinterpret_cast<T*>(pointer)->T::~T();
    });
  }
  
  /**
   * \brief Registers a separate cleanup function to be invoked upon destruction
   *        of this object.
   * \param function The function to register
   */
  void add(std::function<void()> function) {
    _destructors.push_back(function);
  }
 private:
  /// \brief Stores the registered destructors.
  std::vector<std::function<void()>> _destructors;
};

/// \brief The next offset in the last thread-local storage block
std::atomic<std::size_t>                          gTlsCurrentIndex;

/// \brief Helper object to destroy thread-local objects prior to deallocation
thread_local TlsDestructionHelper                 tTlsDestructionHelper;

/// \brief The actual thread-local storage
thread_local std::vector<std::unique_ptr<char[]>> tTlsMemory;

} // namespace detail


// Thread-local storage: tracking and verification
// These features can be enabled by defining ERAGPSIM_CORE_TLS_ENABLE_TRACKING
namespace tracking {

#ifdef ERAGPSIM_CORE_TLS_ENABLE_TRACKING

struct TlsTracking {
  std::string type;
  std::size_t size;
};

/// \brief Stores data about any object allocated
thread_local std::vector<TlsTracking> tTlsTracking;

/**
 * \brief Outputs the current usage of the TLS into the given stream
 * \param stream The \c ostream to write the data to
 * \pre Requires \c ERAGPSIM_CORE_TLS_ENABLE_TRACKING to be defined
 */
void report(std::ostream& stream) {
  // For now, simply output the stored objects
  for (std::size_t index = 0; index < tTlsTracking.size(); ++index) {
    if (tTlsTracking[index].size > 0) {
      stream << index << ": " << tTlsTracking[index].type << " ("
             << tTlsTracking[index].size << " bytes)" << std::endl;
    } else {
      stream << index << ": (empty)" << std::endl;
    }
  }
}

/**
 * \brief Tracks an allocation at index \c index.
 * \param page The TLS index the allocation took place at
 * \tparam T The type of the object allocated
 */
template <class T>
inline void track(std::size_t index) noexcept {
  // Create the tracking object
  TlsTracking data { typeid(T).name(), sizeof(T) };
  // If an object was already allocated here, complain violently.
  if (tTlsTracking.size() > index && tTlsTracking[index].size > 0) {
    // Build the error message
    std::ostringstream errorMessage;
    errorMessage << "Illegal allocation of an object of type " << data.type
                 << " (" << data.size << " bytes) in thread-local storage: "
                 << "Index " << index << " is occuped by an object of type "
                 << tTlsTracking[index].type << " (" << tTlsTracking[index].size
                 << " bytes)." << std::endl;
    report(errorMessage);
    // Write and terminate
    std::cerr << errorMessage.str() << std::flush;
    std::abort();
  }
  // Store the tracking object
  while (tTlsTracking.size() <= index) tTlsTracking.emplace_back();
  tTlsTracking[index] = std::move(data);
}

#else

// Tracking is disabled
template <class T>
inline void track(std::size_t /* index */) {}

#endif // ERAGPSIM_CORE_TLS_ENABLE_TRACKING

} // namespace tracking


/**
 * \brief A wrapper object for raw pointer to TLS memory which will be marked as
 *        invalid when the TLS is destroyed.
 * \tparam T The object type
 */
template <class T>
class Pointer {
 public:
  /**
   * \brief Creates a new TLS pointer wrapper from the specified raw pointer
   * \param pointer The raw pointer to wrap.
   * This constructor must be invoked in the thread the TLS object lives in.
   */
  Pointer(T* pointer) : _valid(true), _pointer(pointer) {
    // Register the cleanup function to mark this pointer as invalid when the
    // object is destroyed
    detail::tTlsDestructionHelper.add([this]() {
      this->_valid = false;
      this->_pointer = nullptr;
    });
  }
  Pointer() : _valid(false), _pointer(nullptr) {}
  
  /**
   * \brief Dereferences this pointer
   * \pre The pointer must be \c valid()
   */
  T& operator*() {
    return *_pointer;
  }
  /**
   * \brief Dereferences this pointer to a \c const reference
   * \pre The pointer must be \c valid()
   */
  const T& operator*() const {
    return *_pointer;
  }
  
  /**
   * \brief Allows member access to the wrapped pointer
   * \pre The pointer must be \c valid()
   */
  T* operator->() {
    return _pointer;
  }
  /**
   * \brief Allows \c const member access to the wrapped pointer
   * \pre The pointer must be \c valid()
   */
  const T* operator->() const {
    return _pointer;
  }
  
  /**
   * \brief Determines whether the pointer is still valid, or whether the
   *        associated TLS object has already been destroyed
   * \returns Whether the pointer is still valid
   */
  bool valid() const {
    return _valid && (_pointer != nullptr);
  }
  
 private:
  /// \brief Whether this pointer is still valid
  bool _valid;
  /// \brief The actual raw pointer
  T* _pointer;
};

/**
 * \brief A wrapper for an object in thread-local storage.
 * \tparam T The object type
 * If the object wrapped by an object of this type has not yet been created in
 * the thread this wrapper is used in, it will be created on first use.
 */
template <class T>
class ThreadLocal {
 public:
  /**
   * \brief Creates a new TLS wrapper from the specified offset in the TLS
   * \param index The TLS index this object is allocated at
   */
  ThreadLocal(std::size_t index) : _index(index) {}
  ThreadLocal() = default;
 
  /**
   * \brief Returns a wrapped pointer to the object in TLS.
   * This may allow other threads to access this object, and therefore introduce
   * race conditions
   */
  Pointer<T> get() {
    return Pointer<T>(raw());
  }
  
  /**
   * \brief Returns a wrapped \c const pointer to the object in TLS.
   * This may allow other threads to access this object, and therefore introduce
   * race conditions
   */
  Pointer<const T> get() const {
    return Pointer<const T>(raw());
  }
  
  /**
   * \brief Allows access to the underlying object
   */
  T& operator*() {
    return *raw();
  }
  /**
   * \brief Allows \c const access to the underlying object
   */
  const T& operator*() const {
    return *raw();
  }
 
  /**
   * \brief Allows member access to the underlying object
   */
  T* operator->() {    
    return raw();
  }
  /**
   * \brief Allows \c const member access to the underlying object
   */
  const T* operator->() const {
    return raw();
  }
  
 private:
  /// \brief The TLS index this object is located at
  std::size_t _index;
 
  /**
   * \brief Returns the raw pointer to the object in TLS.
   * \returns A \c T* to the targeted object
   */
  T* raw() const {
    // Add pointers if the current index does not even have a pointer here
    while (detail::tTlsMemory.size() <= _index) {
      detail::tTlsMemory.emplace_back(nullptr);
    }
    
    // Create the object if it isn't yet created
    if (!detail::tTlsMemory[_index]) {
      // Allocate memory
      detail::tTlsMemory[_index] = std::unique_ptr<char[]>(new char[sizeof(T)]);
      // Create the object
      T* t = new (detail::tTlsMemory[_index].get()) T;
      tracking::track<T>(_index);
      // Return the pointer
      return t;
    }
    
    // Otherwise, cast the actual memory location to the proper pointer type
    return reinterpret_cast<T*>(detail::tTlsMemory[_index].get());
  }
};

/**
 * \brief Allocates an object in thread-local storage
 * \tparam T The type of the object to be allocated.
 * \returns A \c tls::ThreadLocal<T>
 */
template <class T>
ThreadLocal<T> allocate() {
  // Fetch the next free index in the TLS
  std::size_t index = detail::gTlsCurrentIndex.fetch_add(1, std::memory_order_relaxed);
  
  // Add pointers until we arrive at our current index
  while (detail::tTlsMemory.size() <= index) {
    detail::tTlsMemory.emplace_back(nullptr);
  }
  
  // Allocate memory and construct the object
  detail::tTlsMemory[index] = std::unique_ptr<char[]>(new char[sizeof(T)]);
  new (detail::tTlsMemory[index].get()) T;
  tracking::track<T>(index);
  
  // Return a wrapper object
  return ThreadLocal<T>(index);
}
  
} // namespace tls

#endif // ERAGPSIM_CORE_LOCKFREE_THREAD_MAP_HPP_
