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

#ifndef ERAGPSIM_ARCH_COMMON_ARCHITECTURE_FORMULA_HPP
#define ERAGPSIM_ARCH_COMMON_ARCHITECTURE_FORMULA_HPP

#include <cassert>
#include <string>

#include "arch/common/architecture.hpp"
#include "common/container-adapter.hpp"
#include "common/utility.hpp"

/**
 * Describes an Architecture formula.
 *
 * A formula is just a list of extension names. There must be one base extension
 * and optionally any number of further extensions.
 */
class Architecture::Formula
    : public ContainerAdapter<std::string, std::vector, true> {
 public:
  using ExtensionName = std::string;
  using super         = ContainerAdapter<std::string, std::vector, true>;

  /**
   * Constructs a new Formula.
   *
   * @tparam Range A range-like sequence type.
   *
   * @param base The name of the base extension.
   * @param extensions A range of other extensions.
   */
  template <typename Range>
  Formula(const ExtensionName& base, const Range& extensions)
  : super(Utility::prependOther<Underlying>(base, extensions)) {
    assert(!base.empty());
  }

  /**
   Constructs a new Formula.
   *
   * The first element of the range must be the base.
   *
   * @tparam Range A range-like sequence type.
   *
   * @param all The range of extensions for the formula.
   */
  template <typename Range>
  explicit Formula(const Range& range) : super(range) {
  }

  /**
   * Returns the base extension's name.
   *
   * Corresponds to the first element in the sequence.
   */
  const ExtensionName& base() const noexcept {
    return _container.front();
  }
};

#endif /* ERAGPSIM_ARCH_COMMON_ARCHITECTURE_FORMULA_HPP */
