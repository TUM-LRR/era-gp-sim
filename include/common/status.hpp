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

#ifndef ERAGPSIM_COMMON_STATUS_HPP
#define ERAGPSIM_COMMON_STATUS_HPP

#include <string>

/**
 * A generic status class holding a success status and an optional message.
 *
 * This class is essentially like an error, but holds a success value
 * for cases where exceptions are not allowed.
 */
class Status {
 public:
  enum class Code { OK, FAILURE, UNDEFINED };

  static constexpr Code OK = Code::OK;
  static constexpr Code FAILURE = Code::FAILURE;
  static constexpr Code UNDEFINED = Code::UNDEFINED;

  /**
   * Utility factory method to return a failure status with the given message.
   * \param message The message to fail with.
   * \return A failure status with the given message.
   */
  static Status Fail(const std::string& message);

  /**
   * Constructor.
   *
   * \param code The success code for the status.
   * \param message The optional message for the status.
   */
  Status(
      Code code = Code::UNDEFINED,
      const std::string& message = std::string());  // NOLINT(runtime/explicit)

  /**
   * Compares the status for equality with another instance.
   *
   * \param other The other instance to compare with.
   * \returns True if the status codes of the two instances match, else false.
   * The messages are not inspected.
   */
  bool operator==(const Status& other) const noexcept;

  /**
   * Compares a status instance for equality with a code.
   *
   * \param status The status instance to compare.
   * \param code The code to compare with.
   * \returns True if the code of the status matches the other code, else false.
   */
  friend bool operator==(const Status& status, Code code) noexcept;

  /**
   * Compares a status instance for equality with a code.
   *
   * \param code The code to compare with.
   * \param status The status instance to compare.
   * \returns True if the code of the status matches the other code, else false.
   */
  friend bool operator==(Code code, const Status& status) noexcept;

  /**
   * \returns The code of the status.
   */
  const Code& code() const noexcept;

  /**
   * \returns The message of the status.
   */
  const std::string& message() const noexcept;

  /**
   * \returns True if the status code is OK, else false.
   */
  explicit operator bool() const noexcept {
    return _code == Code::OK;
  }

 private:
  /** The code of the status. */
  Code _code;

  /** The message of the status. */
  std::string _message;
};

#endif /* ERAGPSIM_COMMON_STATUS_HPP*/
