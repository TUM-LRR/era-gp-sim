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
#include <iterator>

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
}

#endif /* ERAGPSIM_COMMON_UTILITY_HPP */
