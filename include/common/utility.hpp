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
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>
#include <string>
#include <type_traits>
#include <vector>

#include "common/assert.hpp"

class MemoryValue;

#define STRINGIFY_INTERNAL(str) #str
#define STRINGIFY(str) STRINGIFY_INTERNAL(str)

namespace Utility {

template <typename T>
struct CompletelyOrdered {
  virtual bool operator==(const T &other) const noexcept = 0;

  virtual bool operator<(const T &other) const = 0;

  virtual bool operator!=(const T &other) const noexcept {
    return !(*this == other);
  }

  virtual bool operator>(const T &other) const {
    return !(operator==(other)) && !(operator<(other));
  }

  virtual bool operator<=(const T &other) const {
    return operator<(other) || operator==(other);
  }

  virtual bool operator>=(const T &other) const {
    return operator>(other) || operator==(other);
  }
};

template <typename T = long>
struct LazyRange {
  class Iterator : public CompletelyOrdered<Iterator>,
                   public std::iterator<std::bidirectional_iterator_tag, T> {
   public:
    bool operator==(const Iterator &other) const noexcept {
      return this->_index == other._index;
    }

    bool operator<(const Iterator &other) const noexcept {
      return this->_index < other._index;
    }

    T operator*() const noexcept {
      return _index;
    }

    Iterator &operator++() noexcept {
      _index += _step;
      return *this;
    }

    Iterator operator++(int)noexcept {
      auto copy = _index;
      ++*this;
      return copy;
    }

    Iterator &operator--() noexcept {
      _index -= _step;
      return *this;
    }

    Iterator operator--(int)noexcept {
      auto copy = _index;
      --*this;
      return copy;
    }


   private:
    friend struct LazyRange;

    Iterator(T index, T step) : _index(index), _step(step) {
    }

    T _index;
    T _step;
  };

  LazyRange(const T &start, const T &end, const T &step)
  : _start(start), _end(end), _step(step) {
    // Check that we wouldn't iterate forever
    assert::that(((end - start) / step) >= 0);

    // Round up to the next multiple of the step
    _end = (end + step - 1) - ((end + step - 1) % step);
  }

  Iterator begin() const noexcept {
    return {_start, _step};
  }

  Iterator end() const noexcept {
    return {_end, _step};
  }

 private:
  T _start;
  T _end;
  T _step;
};

template <typename T = long>
auto range(const T &start, const T &end, const T &step = 1) {
  return LazyRange<T>(start, end, step);
}

template <typename T = std::size_t, typename Output = std::vector<T>>
Output rangeContainer(T start, T end, const T &step = 1) {
  using Relation = std::function<bool(const T &, const T &)>;

  Relation relation;

  if (step < 0) {
    // If the step is negative, we go backwards, so loop while start > end
    relation = std::greater<>{};
  } else {
    // If the step is positive, we go forward, so loop while start < end
    relation = std::less<>{};
  }

  auto amount = (end - start) / step;

  if (amount == 0) return {};

  assert::that(amount > 0);
  Output output(std::ceil(amount));

  for (std::size_t index = 0; relation(start, end); start += step, ++index) {
    output[index] = start;
  }

  return output;
}

template <typename T = std::size_t, typename Output = std::vector<T>>
auto rangeContainer(const T &end) {
  return range<T, Output>(0, end);
}

/**
 * Expand these false (UNSIGNED) and true (SIGNED) alternatives
 * into the Utility namespace for more expressive function calls.
 */
enum { UNSIGNED, SIGNED };

template <typename T>
bool occupiesMoreBitsThan(const T &value, std::size_t numberOfBits) {
  if (numberOfBits == 0) return true;

  // Get the value one larger than the maximum allowed absolute value
  auto boundary = static_cast<std::uintmax_t>(1) << numberOfBits;

  // Note especially that signed numbers can occupy one more value
  // than positive numbers with the same bit width
  return (value < 0) ? value < -boundary : value >= boundary;
}

bool occupiesMoreBitsThan(const MemoryValue &value,
                          size_t numberOfBits,
                          bool isSigned = true);

template <typename T>
bool fitsIntoBits(const T &value, std::size_t numberOfBits) {
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

  View(const Range &range, size_t beginIndex, size_t lastIndex)
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
View<Range> viewUpTo(Range &range, std::size_t index) {
  return {range, 0, index};
}

template <typename Range>
auto viewFrom(Range &range, std::size_t index) {
  auto size = std::distance(begin(range), end(range));
  return View<Range>(range, index, size);
}


std::string toLower(const std::string &string);

std::string toUpper(const std::string &string);

template <typename Range, typename Iterator, typename Transformer>
void transformInto(Range &range, Iterator destination, Transformer transform) {
  std::transform(begin(range), end(range), destination, transform);
}

template <typename Range, typename Transformer>
void transformInPlace(Range &range, Transformer transform) {
  transformInto(range, begin(range), transform);
}

template <typename InputRange,
          typename Transformer,
          typename OutputRange = InputRange>
OutputRange transform(const InputRange &range, Transformer transform) {
  using std::end;

  OutputRange output;
  transformInto(range, std::back_inserter(output), transform);

  return output;
}

template <typename DestinationRange, typename SourceRange>
void concatenate(DestinationRange &destination, const SourceRange &source) {
  destination.insert(end(destination), begin(source), end(source));
}

template <typename Range, typename Function>
void forEach(Range &&range, Function function) {
  using std::end;

  std::for_each(begin(std::forward<Range>(range)),
                end(std::forward<Range>(range)),
                function);
}

template <typename FirstRange, typename SecondRange, typename Predicate>
bool isEqual(const FirstRange &first,
             const SecondRange &second,
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
bool isEqualBy(const FirstRange &first, const SecondRange &second, Key key) {
  using std::end;

  // clang-format off
        return isEqual(first, second, [key](const auto &first, const auto &second) {
            return key(first) == key(second);
        });
  // clang-format on
}

template <typename Range, typename Function>
bool allOf(const Range &range, Function function) {
  using std::begin;
  using std::end;

  return std::all_of(begin(range), end(range), function);
}

template <typename Range, typename Function>
bool noneOf(const Range &range, Function function) {
  using std::begin;
  using std::end;

  return std::none_of(begin(range), end(range), function);
}

template <typename Range, typename Function>
bool anyOf(const Range &range, Function function) {
  using std::begin;
  using std::end;

  return std::any_of(begin(range), end(range), function);
}

template <typename T, typename InputRange, typename OutputRange = InputRange>
OutputRange prepend(T &&value, const InputRange &range) {
  using std::begin;
  using std::end;

  // Construct with enough space
  OutputRange copy(range.size() + 1);
  copy.insert(end(copy), std::forward<T>(value));
  copy.insert(end(copy), begin(range), end(range));

  return copy;
}

template <typename OutputRange, typename T, typename InputRange>
OutputRange prependOther(T &&value, const InputRange &range) {
  return prepend<T, InputRange, OutputRange>(std::forward<T>(value), range);
}

template <typename T, typename InputRange, typename OutputRange = InputRange>
OutputRange append(T &&value, const InputRange &range) {
  using std::begin;
  using std::end;

  // Construct with enough space
  OutputRange copy(range.size() + 1);
  copy.insert(end(copy), begin(range), end(range));
  copy.insert(end(copy), std::forward<T>(value));

  return copy;
}


template <typename OutputRange, typename T, typename InputRange>
OutputRange appendOther(T &&value, const InputRange &range) {
  return append<T, InputRange, OutputRange>(std::forward<T>(value), range);
}

template <typename Container, typename Key, typename Action>
void doIfThere(Container &container, const Key &key, Action action) {
  auto iterator = container.find(key);
  if (iterator != container.end()) {
    action(*iterator);
  }
}

template <typename Range, typename T>
bool contains(const Range &range, T &&element) {
  using std::begin;
  using std::end;

  auto iterator = std::find(begin(range), end(range), std::forward<T>(element));

  return iterator != end(range);
}

std::string rootPath();

std::string joinPaths(const std::string &single);

template <typename... Tail>
std::string joinPaths(const std::string &first,
                      const std::string &second,
                      Tail &&... tail) {
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
std::string joinToRoot(Paths &&... paths) {
  return joinPaths(rootPath(), std::forward<Paths>(paths)...);
}

std::string loadFromFile(const std::string &filePath);

template <typename Data>
void storeToFile(const std::string &filePath, Data &&data) {
  std::ofstream file(filePath);
  file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
  file << std::forward<Data>(data);
}

template <typename T>
auto copyPointer(const std::unique_ptr<T> &pointer) {
  assert::that(static_cast<bool>(pointer));
  return std::make_unique<T>(*pointer);
}

template <std::size_t numberOfBits,
          typename T,
          typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr T lowerNBits(const T &value) {
  constexpr auto mask = (static_cast<std::size_t>(1) << numberOfBits) - 1;
  return value & mask;
}

template <typename T, template <typename> class Cond>
using TypeBarrier = typename std::enable_if<Cond<T>::value, T>::type;

// C++17
// template<typename... Paths>
// std::string joinStrings(Paths&&... paths) {
//   return (paths + ...);
// }

// convert some integral value into a vector of boolean
template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
std::vector<bool> convertToBinary(T value, std::size_t minSize = 0) {
  std::vector<bool> binary;
  std::vector<bool> tmp;

  while (value != 0) {
    tmp.push_back(value % 2);
    value >>= 1;
  }

  auto size = tmp.size();
  for (int i = 0; i < size; i++) {
    binary.push_back(tmp.back());
    tmp.pop_back();
  }

  int k = minSize - binary.size();

  if (k > 0) binary.insert(binary.cbegin(), k, false);

  return binary;
}

// push_back n elements from the end of the src vector
void pushBackFromEnd(std::vector<bool> &dest,
                     const std::vector<bool> &src,
                     size_t n);

template <typename T>
struct IterableRevert {
  T &iterable;
  auto begin() {
    return iterable.rbegin();
  }

  auto end() {
    return iterable.rend();
  }

  IterableRevert(T &iterable) : iterable(iterable) {
  }
};

template <typename T>
IterableRevert<T> revertIterable(T &iterable) {
  return IterableRevert<T>(iterable);
}


template <typename T, typename S = T>
constexpr auto divideCeiling(const T &value, const S &divider) {
  return (value + divider - 1) / divider;
}

// Only for completeness.
template <typename T, typename S = T>
constexpr auto divideFloor(const T &value, const S &divider) {
  return value / divider;
}

template <typename T, typename S = T>
constexpr auto discreteCeiling(const T &value, const S &divider) {
  return divideCeiling(value, divider) * divider;
}

template <typename T, typename S = T>
constexpr auto discreteFloor(const T &value, const S &divider) {
  return divideFloor(value, divider) * divider;
}
/**
 * \brief roundToBoundary Rounds a given value up to a multiple of the
 * given boundary.
 * \param value Value to be rounded.
 * \param boundary The value is rounded up to a multiple of the
 * boundary.
 * \return The rounded value.
 */
template <typename T>
T roundToBoundary(const T &value, const T &boundary) {
  return discreteCeiling(value, boundary);
}

template <typename Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
struct EnumHash {
  using argument_type = Enum;
  using result_type = std::size_t;
  using underlying_type = std::underlying_type_t<Enum>;

  result_type operator()(const argument_type &argument) const {
    const auto ordinal = static_cast<underlying_type>(argument);
    return std::hash<underlying_type>{}(ordinal);
  }
};

/**
 * Creates a mask of the given number of bits set.
 *
 * \tparam T the integral type the mask should have.
 * \param numberOfBits The width the mask should have.
 * \return A mask of `numberOfBits` exactly set (1) bits.
 */
template <typename T>
constexpr T bitMask(std::size_t numberOfBits) noexcept {
  return (static_cast<T>(1) << numberOfBits) - 1;
}

/**
 * Appends N bits of the first argument to the given second argument.
 *
 * \tparam numerOfBits The number of bits to append.
 * \param original The original value to append to.
 * \param value The value whose bits to append.
 * \return The result of the operation.
 */
template <std::size_t numberOfBits, typename T, typename U>
constexpr T appendBits(const T &original, const U &value) noexcept {
  constexpr auto mask = bitMask<U>(numberOfBits);

  // clang-format off
  static_assert(
    numberOfBits <= std::numeric_limits<T>::digits,
    "Attempting to append more bits than possible"
  );
  // clang-format on

  // We left-shift the original value by x bits and OR in
  // exactly x bits at the right end (least-significant) bits
  return (original << numberOfBits) | (value & mask);
}

/**
 * Slices out a range of bits from a value.
 *
 * \tparam firstBit The bit at which to start the slice.
 * \tparam lastBit The bit at which to end the slice.
 * \param original The value to slice.
 * \return The bits between `firstBit` and `lastBit` within the given `original`
 * value.
 */
template <std::size_t firstBit, std::size_t lastBit, typename T>
constexpr T sliceBits(const T &original) noexcept {
  constexpr std::size_t numberOfBits = lastBit - firstBit + 1;

  // clang-format off
  static_assert(
    numberOfBits <= std::numeric_limits<T>::digits,
    "Attempting to append more bits than possible"
  );
  // clang-format on

  constexpr auto mask = bitMask<T>(numberOfBits);

  return (original & (mask << firstBit)) >> firstBit;
}

/**
 * Appends a slice of bits from one number to the back of another number.
 */
template <std::size_t firstBit, std::size_t lastBit, typename T, typename U>
T appendBitSlice(const T &original, const U &value) {
  auto slice = sliceBits<firstBit, lastBit>(value);
  return appendBits<lastBit - firstBit + 1>(original, slice);
}

template <typename T>
constexpr T mostSignificantBit(const T &value) {
  constexpr auto width = sizeof(T) * 8;
  return value & (T(1) << (width - 1));
}

template <typename SubType, typename BaseType>
bool isInstance(const BaseType *ptr) {
  return dynamic_cast<SubType *>(ptr) != nullptr;
}

template <typename SubType, typename BaseType>
bool isInstance(const std::shared_ptr<BaseType> &ptr) {
  return dynamic_cast<SubType *>(ptr.get()) != nullptr;
}

template <typename SubType, typename BaseType>
bool isInstance(const std::unique_ptr<BaseType> &ptr) {
  return dynamic_cast<SubType *>(ptr.get()) != nullptr;
}

template <typename Range>
Range sorted(Range range) {
  using std::begin;
  using std::end;

  std::sort(begin(range), end(range));

  return range;
}

template <typename Range, typename T>
auto accumulate(const Range &range, const T &initial) {
  using std::begin;
  using std::end;

  return std::accumulate(begin(range), end(range), initial);
}

}  // namespace Utility

#endif /* ERAGPSIM_COMMON_UTILITY_HPP */
