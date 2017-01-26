/*
* ERASIM Assembler Interpreter
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

#ifndef ERASIM_COMMON_HASH_HPP
#define ERASIM_COMMON_HASH_HPP

#include <tuple>
#include <utility>

/**
 * This file implements specialization of `std::hash` for various utility
 * types we need for our classes (like `std::pair` or `std::tuple`).
 */

namespace std {

/**
 * Hashes arbitary tuples.
 *
 * This hash function recurses through the members of the tuple, hashes each
 * and mixes up the result a little.
 *
 * @see
 * http://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
 */
template <typename... Ts>
struct hash<std::tuple<Ts...>> {
  using argument_type = std::tuple<Ts...>;
  using result_type = std::size_t;

  result_type operator()(const argument_type& argument) const {
    return _hash(argument, std::make_index_sequence<sizeof...(Ts)>());
  }

 private:
  template <std::size_t I, std::size_t... Is>
  result_type
  _hash(const argument_type& tuple, std::index_sequence<I, Is...>) const {
    using Hash = std::hash<std::tuple_element_t<I, argument_type>>;

    auto result = _hash(tuple, std::index_sequence<Is...>());
    auto value = std::get<I>(tuple);

    return Hash()(value) + 0x9e3779b9 + (result << 6) + (result >> 2);
  }

  result_type _hash(const argument_type& tuple, std::index_sequence<>) const {
    return 0;
  }
};

template <typename First, typename Second>
struct hash<std::pair<First, Second>> {
  using argument_type = std::pair<First, Second>;
  using result_type = std::size_t;

  result_type operator()(const argument_type& argument) const {
    return std::hash<std::tuple<First, Second>>{}(argument);
  }
};
}

#endif /* ERASIM_COMMON_HASH_HPP */
