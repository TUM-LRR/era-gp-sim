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

#ifndef ERAGPSIM_COMMON_ASSERT_HPP
#define ERAGPSIM_COMMON_ASSERT_HPP

#include <cstddef>
#include <stdexcept>
#include <string>

#ifndef BLOCK_SYMBOLS_FOR_ASSERTION_TESTS
/**
 * A custom string literal operator to replace the missing standard one.
 *
 * With this function defined, you can append "_s" to any string literal to turn
 * it into a `std::string`.
 *
 * \param string The string literal to stringify.
 * \param <empty> A necessary dummy operator, required by the standard.
 *
 * \return The string literal, converted to a `std::string`.
 */
std::string operator""_s(const char* string_literal, std::size_t) {
  return string_literal;
}
#endif

namespace assert {

#ifndef BLOCK_SYMBOLS_FOR_ASSERTION_TESTS
/**
 * An Exception to be thrown by our custom assertion mechanism.
 */
struct AssertionError : public std::runtime_error {
  using super = std::runtime_error;
  explicit AssertionError(const std::string& what) : super(what) {
  }
};
#endif

#ifdef DEBUG
/**
 * A dummy function necessary for our assertion mechanism.
 *
 * Basically, when you call `assert::that`, the `that` macro must expand to
 * something valid that follows the `assert::` part. The only real options are a
 * function or a constant. However, since we want to pass some condition to the
 * `assert::that` call, only functions can work. Then, we must pass the code
 * checking the conditon to that function. This is done through the lambda
 * defined in the macro below. This `execute` function then does nothing more
 * than call that lambda to execute the condition checking code.
 *
 * \param function A function (lambda) containing the condition checking code.
 */
template <typename Function>
void execute(Function function) {
  function();
}

/**
 * This macro passes a lambda containing code to check the given expression.
 * It is a macro so we can use the "#<argument>" macro operator to turn the
 * condition into a string. If the condition evaluates to false, an
 * `AssertionError` is thrown with its message formatted as:
 * "Assertion Failed: (<condition>) at line <line number> in file <file name>".
 */
#define that(condition)                                             \
  execute([] {                                                      \
    if (!(condition)) {                                             \
      throw assert::AssertionError(                                 \
          ("Assertion failed: (" #condition ") at line "_s) +       \
          std::to_string(__LINE__) + " in file " + __FILE__ + "."); \
    }                                                               \
  })
#else
void unicorn() {
}
#define that(condition) unicorn()
#endif
}

#endif /* ERAGPSIM_COMMON_ASSERT_HPP_ */
