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
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

namespace Utility {

template <typename T>
bool occupiesMoreBitsThan(const T& value, std::size_t numberOfBits) {
  if (numberOfBits == 0) return true;

  // Get the value one larger than the maximum allowed absolute value
  auto boundary = static_cast<std::int64_t>(1) << numberOfBits;

  // Note especially that signed numbers can occupy one more value
  // than positive numbers with the same bit width
  return (value < 0) ? value < -boundary : value >= boundary;
}

template <typename T>
bool fitsIntoBits(const T& value, std::size_t numberOfBits) {
  return !occupiesMoreBitsThan(value, numberOfBits);
}

template <typename IteratorTag>
struct isSinglePassIteratorTag {
  static constexpr bool value = false;
};

template <>
struct isSinglePassIteratorTag<std::input_iterator_tag> {
  static constexpr bool value = true;
};

template <>
struct isSinglePassIteratorTag<std::output_iterator_tag> {
  static constexpr bool value = true;
};

template <typename Iterator>
constexpr bool isSinglePassIterator = isSinglePassIteratorTag<
    typename std::iterator_traits<Iterator>::iterator_category>::value;

template <typename Range>
class View {
 public:
  using Iterator = decltype(std::declval<Range>().begin());
  using size_t = std::size_t;

  View(const Range& range, size_t beginIndex, size_t lastIndex)
  : _begin(std::begin(range)), _end(std::begin(range)) {
    std::advance(_begin, beginIndex);
    std::advance(_end, lastIndex);
  }

  auto begin() {
    return _begin;
  }

  auto begin() const {
    return _begin;
  }

  auto end() {
    return _end;
  }

  auto end() const {
    return _end;
  }

  std::enable_if_t<!isSinglePassIterator<Iterator>, size_t> size() const
      noexcept {
    return std::distance(_begin, _end);
  }

 private:
  Iterator _begin;
  Iterator _end;
};

template <typename Range>
View<Range> viewUpTo(Range& range, std::size_t index) {
  return {range, 0, index};
}

template <typename Range>
auto viewFrom(Range& range, std::size_t index) {
  auto size = std::distance(begin(range), end(range));
  return View<Range>(range, index, size);
}


std::string toLower(const std::string& string);
std::string toUpper(const std::string& string);

template <typename Range, typename Iterator, typename Transformer>
void transformInto(Range& range, Iterator destination, Transformer transform) {
  std::transform(begin(range), end(range), destination, transform);
}

template <typename Range, typename Transformer>
void transformInPlace(Range& range, Transformer transform) {
  transformInto(range, begin(range), transform);
}

template <typename InputRange,
          typename Transformer,
          typename OutputRange = InputRange>
OutputRange transform(const InputRange& range, Transformer transform) {
  using std::end;

  OutputRange output;
  transformInto(range, std::back_inserter(output), transform);

  return output;
}

template <typename DestinationRange, typename SourceRange>
void concatenate(DestinationRange& destination, const SourceRange& source) {
  destination.insert(end(destination), begin(source), end(source));
}

template <typename Range, typename Function>
void forEach(Range&& range, Function function) {
  using std::end;

  std::for_each(begin(std::forward<Range>(range)),
                end(std::forward<Range>(range)),
                function);
}

template <typename FirstRange, typename SecondRange, typename Predicate>
bool isEqual(const FirstRange& first,
             const SecondRange& second,
             Predicate predicate = std::equal_to<>{}) {
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

template <typename FirstRange, typename SecondRange, typename Key>
bool isEqualBy(const FirstRange& first, const SecondRange& second, Key key) {
  using std::end;

  // clang-format off
  return isEqual(first, second, [key] (const auto& first, const auto& second) {
    return key(first) == key(second);
  });
  // clang-format on
}

template <typename Range, typename Function>
bool allOf(const Range& range, Function function) {
  using std::end;

  return std::all_of(begin(range), end(range), function);
}

template <typename Range, typename Function>
bool noneOf(const Range& range, Function function) {
  using std::end;

  return std::none_of(begin(range), end(range), function);
}

template <typename Range, typename Function>
bool anyOf(const Range& range, Function function) {
  using std::end;

  return std::any_of(begin(range), end(range), function);
}

template <typename T, typename InputRange, typename OutputRange = InputRange>
OutputRange prepend(T&& value, const InputRange& range) {
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

// convert some integral value into a vector of boolean
template <typename T>
void convertToBin(std::vector<bool>& binary,
                  const T& value,
                  std::size_t minSize = 0) {
  T copyValue = value;

  std::vector<bool> tmp;

  while (copyValue != 0) {
    tmp.push_back(copyValue % 2);
    copyValue >>= 1;
  }

  auto size = tmp.size();
  for (int i = 0; i < size; i++) {
    binary.push_back(tmp.back());
    tmp.pop_back();
  }

  int k = minSize - binary.size();

  if (k > 0) binary.insert(binary.cbegin(), k, false);
}

// push_back n elements from the end of the src vector
void pushBackFromEnd(std::vector<bool>& dest,
                     const std::vector<bool>& src,
                     size_t n);
}

#endif /* ERAGPSIM_COMMON_UTILITY_HPP */
