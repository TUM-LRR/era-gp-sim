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

#ifndef ERAGPSIM_COMMON_UTILITY_HPP
#define ERAGPSIM_COMMON_UTILITY_HPP

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>

namespace Utility {

template <typename DestinationRange, typename SourceRange>
void concatenate(DestinationRange& destination, const SourceRange& source) {
  using std::begin;
  using std::end;
  destination.insert(end(destination), begin(source), end(source));
}

template <typename Range, typename Function>
void forEach(Range&& range, Function function) {
  using std::begin;
  using std::end;

  std::for_each(begin(std::forward<Range>(range)),
                end(std::forward<Range>(range)),
                function);
}

template <typename FirstRange, typename SecondRange>
bool isEqual(const FirstRange& first, const SecondRange& second) {
  using std::begin;
  using std::end;

  return std::equal(begin(first), end(first), begin(second), end(second));
}

template <typename FirstRange, typename SecondRange, typename Predicate>
bool isEqual(const FirstRange& first,
             const SecondRange& second,
             Predicate predicate) {
  using std::begin;
  using std::end;

  // clang-format off
  return std::equal(
    begin(first),
    end(first),
    begin(second),
    end(second),
    predicate
  );
  // clang-format on
}

template <typename Range, typename Function>
bool allOf(const Range& range, Function function) {
  using std::begin;
  using std::end;

  return std::all_of(begin(range), end(range), function);
}

template <typename Range, typename Function>
bool noneOf(const Range& range, Function function) {
  using std::begin;
  using std::end;

  return std::none_of(begin(range), end(range), function);
}

template <typename Range, typename Function>
bool anyOf(const Range& range, Function function) {
  using std::begin;
  using std::end;

  return std::any_of(begin(range), end(range), function);
}

template <typename T, typename InputRange, typename OutputRange = InputRange>
OutputRange prepend(T&& value, const InputRange& range) {
  using std::begin;
  using std::end;

  // Construct with enough space
  OutputRange copy(range.size() + 1);
  copy.insert(end(copy), std::forward<T>(value));
  copy.insert(end(copy), begin(range), end(range));

  return copy;
}

template <typename OutputRange, typename T, typename InputRange>
OutputRange prependOther(T&& value, const InputRange& range) {
  return prepend<T, InputRange, OutputRange>(std::forward<T>(value), range);
}

template <typename T, typename InputRange, typename OutputRange = InputRange>
OutputRange append(T&& value, const InputRange& range) {
  using std::begin;
  using std::end;

  // Construct with enough space
  OutputRange copy(range.size() + 1);
  copy.insert(end(copy), begin(range), end(range));
  copy.insert(end(copy), std::forward<T>(value));

  return copy;
}


template <typename OutputRange, typename T, typename InputRange>
OutputRange appendOther(T&& value, const InputRange& range) {
  return append<T, InputRange, OutputRange>(std::forward<T>(value), range);
}

template <typename Container, typename Key, typename Action>
void doIfThere(Container& container, const Key& key, Action action) {
  auto iterator = container.find(key);
  if (iterator != container.end()) {
    action(*iterator);
  }
}

std::string rootPath();
std::string joinPaths(const std::string& single);

template <typename... Tail>
std::string
joinPaths(const std::string& first, const std::string& second, Tail&&... tail) {
  auto intermediary = first;

  if (intermediary.back() != '/' && second.front() != '/') {
    intermediary += "/";
  } else if (intermediary.back() == '/' && second.front() == '/') {
    intermediary.pop_back();
  }

  intermediary += second;

  return joinPaths(intermediary, std::forward<Tail>(tail)...);
}

template <typename... Paths>
std::string joinToRoot(Paths&&... paths) {
  return joinPaths(rootPath(), std::forward<Paths>(paths)...);
}

std::string loadFromFile(const std::string& filePath);

template <typename Data>
void storeToFile(const std::string& filePath, Data&& data) {
  std::ofstream file(filePath);
  assert(static_cast<bool>(file));
  file << std::forward<Data>(data);
  assert(static_cast<bool>(file));
}

template <typename T>
auto copyPointer(const std::unique_ptr<T>& pointer) {
  assert(static_cast<bool>(pointer));
  return std::make_unique<T>(*pointer);
}

// C++17
// template<typename... Paths>
// std::string joinStrings(Paths&&... paths) {
//   return (paths + ...);
// }
}

#endif /* ERAGPSIM_COMMON_UTILITY_HPP */
