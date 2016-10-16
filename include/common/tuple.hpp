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

#ifndef ERAGPSIM_COMMON_TUPLE_HPP_
#define ERAGPSIM_COMMON_TUPLE_HPP_

#if __cpp_lib_apply >= 201603
#include <tuple>
#elif defined(__has_include) && __has_include(<experimental/tuple>)
#include <experimental/tuple>
#elif __cpp_lib_experimental_apply
#include <experimental/tuple>
#else
#include <functional>
#include <tuple>
#endif

namespace TupleApply {
#if __cpp_lib_apply >= 201603

using std::apply;

#elif defined(__has_include) && __has_include(<experimental/tuple>)

using std::experimental::apply;

#elif __cpp_lib_experimental_apply

using std::experimental::apply;

#else
//This was partly taken from the c++ standard reference implementation.////////
namespace Detail {
  template <class T>
  constexpr size_t tuple_size_v = std::tuple_size<T>::value;

  template <class F, class Tuple, size_t... I>
  constexpr decltype(auto)
  apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>) {
    return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
  }
}

template <class F, class Tuple>
constexpr decltype(auto) apply(F&& f, Tuple&& t) {
  return Detail::apply_impl(
      std::forward<F>(f),
      std::forward<Tuple>(t),
      std::make_index_sequence<Detail::tuple_size_v<std::decay_t<Tuple>>>{});
}
///////////////////////////////////////////////////////////////////////////////
#endif
}

#endif /* ERAGPSIM_COMMON_TUPLE_HPP_ */
