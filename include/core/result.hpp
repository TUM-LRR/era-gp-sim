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

#ifndef CORE_RESULT_HPP
#define CORE_RESULT_HPP

#include <cassert>
#include <exception>

template <class ResultType>

/**
 * A class to store either a value of type ResultType or an exception
 *
 */
class Result {
 public:
  Result() : _empty(true) {
  }

  Result(ResultType&& result)
  : _result(std::forward<ResultType>(result)), _empty(false) {
  }

  Result(std::exception_ptr exception) : _exception(exception), _empty(false) {
  }

  void setValue(ResultType&& value) {
    _result = std::forward<ResultType>(value);
    _empty  = false;
  }

  void setException(std::exception_ptr exception) {
    _exception = exception;
    _empty     = false;
  }

  const bool isEmpty() const {
    return _empty;
  }

  ResultType get() const {
    assert(!isEmpty());
    if (hasException()) {
      std::rethrow_exception(_exception);
    }
    return _result;
  }

  bool hasException() const {
    return !_empty & !!_exception;
  }

 private:
  bool _empty;
  ResultType _result;
  std::exception_ptr _exception;
};

#endif /* CORE_RESULT_HPP */
