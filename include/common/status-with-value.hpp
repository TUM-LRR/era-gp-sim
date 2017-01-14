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

#ifndef ERAGPSIM_COMMON_STATUS_WITH_VALUE_HPP
#define ERAGPSIM_COMMON_STATUS_WITH_VALUE_HPP

#include <type_traits>

#include "common/assert.hpp"
#include "common/optional.hpp"
#include "common/status.hpp"

/**
 * Allows returning a value along with a status code.
 *
 * This class is necessary in the case where we must propagate
 * an exception for an exceptional condition that we expect to be caught,
 * but cannot use exceptions (such as in Qt contexts). It allows storage
 * of both value as well as reference types. The Status type may not be
 * supplied externally, as it would disallow usage of default parameters
 * in the implementation.
 */
template <typename T>
class StatusWithValue {
 public:
  static_assert(!std::is_rvalue_reference<T>::value,
                "Cannot store rvalue reference in StatusWithValue");

  /**
   * Constructor.
   *
   * \param status The status to construct the instance with.
   */
  StatusWithValue(const Status& status)  // NOLINT(runtime/explicit)
      : _status(status) {
    // Default-construct the object in-place
  }

  /**
   * Constructor.
   *
   * \param value The value to construct the stored value with.
   * \param status The status for the instance.
   */
  template <typename U,
            typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  StatusWithValue(
      U&& value,
      const Status& status = Status::OK)  // NOLINT(runtime/explicit)
      : _value(std::forward<U>(value)),
        _status(status) {
  }

  /**
   * \returns The value stored, if any.
   */
  T value() noexcept {
    assert::that(static_cast<bool>(_value));
    return *_value;
  }

  /**
   * \returns The value stored, if any.
   */
  const T value() const noexcept {
    assert::that(static_cast<bool>(_value));
    return *_value;
  }

  /**
   * \returns The status.
   */
  const Status& status() const noexcept {
    return _status;
  }

  /**
   * \returns The message of the status.
   */
  auto message() const noexcept(noexcept(_status.message())) {
    return _status.message();
  }

  /**
   * \returns The status.
   */
  explicit operator bool() const noexcept {
    return _status == Status::OK;
  }

 private:
  /** The status of the instance. */
  Status _status;

  /**
   * The optional value, which should always exist on success,
   * but may not exist on failure. If T is a reference, a reference
   * wrapper is maintained, else an instance of type T directly.
   */
  // clang-format off
  Optional<
    std::conditional_t<
      std::is_reference<T>::value,
      std::reference_wrapper<std::remove_reference_t<T>>,
      T>
  > _value;
  // clang-format on
};

#endif /* ERAGPSIM_COMMON_STATUS_WITH_VALUE_HPP */
