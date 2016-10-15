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

#ifndef ERAGPSIM_CORE_RESULT_HPP
#define ERAGPSIM_CORE_RESULT_HPP

#include <cassert>
#include <exception>

/**
 * \brief A class to store either a value of type ResultType or an exception.
 * Can be used if a task is executed in another thread and a possible exception
 * should be rethrown in another thread, where the return value of the task is
 * used.
 *
 */
template <class ResultType>
class Result {
 public:
  /**
   * Creates a new, empty Result object
   * You have to set a value or an exception before calling get(), this is
   * asserted.
   *
   */
  Result() : _empty(true) {
  }

  /**
   * Creates a new Result object and sets a value, get() can be called
   * immediately.
   * \param result the result value
   *
   */
  Result(ResultType&& result)
  : _result(std::forward<ResultType>(result)), _empty(false) {
  }

  /**
   * Creates a new Result object and sets an exception, get() can be called
   * immediately.
   * \param exception std::exception_ptr object to rethrow the exception
   *
   */
  Result(std::exception_ptr exception) : _exception(exception), _empty(false) {
  }

  /**
   * Sets the result value
   * \param value the desired result value
   *
   */
  void setValue(ResultType&& value) {
    _result = std::forward<ResultType>(value);
    _empty  = false;
  }

  /**
   * Sets a exception to rethrow
   * \param exception the std::exception_ptr to rethrow the exception
   *
   */
  void setException(std::exception_ptr exception) {
    _exception = exception;
    _empty     = false;
  }

  /**
   * Returns true if this object has no value or exception set
   *
   */
  const bool isEmpty() const {
    return _empty;
  }

  /**
   * Returns a value of ResultType or, if there is a exception_ptr set, rethrows
   * an exception. If this object is empty, an assertion will fail.
   *
   */
  ResultType get() const {
    assert(!isEmpty());
    if (hasException()) {
      std::rethrow_exception(_exception);
    }
    return _result;
  }

  /**
   * Returns true if an exception_ptr is stored in this object, otherwise false.
   *
   */
  bool hasException() const {
    return !_empty & !!_exception;
  }

 private:

  /** flag indicating if this object has a stored value or exception */
  bool _empty;

  /** object to store a value of ResultType */
  ResultType _result;

  /** std::exception_ptr to rethrow a exception */
  std::exception_ptr _exception;
};

#endif /* ERAGPSIM_CORE_RESULT_HPP */
