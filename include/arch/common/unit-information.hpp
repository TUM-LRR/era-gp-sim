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

#ifndef ERAGPSIM_ARCH_UNIT_INFORMATION_HPP
#define ERAGPSIM_ARCH_UNIT_INFORMATION_HPP

#include <string>

#include "arch/common/register-information.hpp"
#include "common/builder.hpp"
#include "common/container-adapter.hpp"
#include "common/utility.hpp"

/**
 * This class holds information about a *unit*.
 *
 * A *unit* is basically a named container of registers. For example, the "CPU"
 * unit brings integer registers and the "FPU" unit brings floating point
 * registers.
 */
class UnitInformation : public ContainerAdapter<RegisterInformation>,
                        public Builder {
 public:
  using super = ContainerAdapter<RegisterInformation>;
  using super::_container;
  using super::begin;
  using super::cbegin;
  using super::end;
  using super::cend;
  using super::clear;
  using super::size;
  using super::isEmpty;

  /**
   * Constructs a new unit.
   *
   * @param name The name of the unit, e.g. "cpu".
   */
  explicit UnitInformation(const std::string& name);

  /**
   * Adds a range of RegisterInformation objects to the unit.
   *
   * @tparam Range A range-like type.
   *
   * @param range A range of RegisterInformation objects.
   *
   * @return The current unit object.
   *
   * @see addInstructions
   */
  template <typename Range>
  UnitInformation& operator+=(const Range& range) {
    return addRegisters(range);
  }

  /**
   * Returns the result of adding the registers to this unit, without
   * modifying this unit.
   *
   * A copy is made of this object, then modified via +=, then returned.
   *
   * @tparam Range A range-like type.
   *
   * @param range The range of RegisterInformation objects to add.
   *
   * @return The current unit object.
   */
  template <typename Range>
  UnitInformation operator+(const Range& other) const {
    auto temp = *this;
    temp += other;

    return temp;
  }

  /**
   * Sets the name of the unit.
   *
   * This would usually be "cpu", "fpu" (floats) or "vpu" (vector processing).
   *
   * @param name The new name for the unit.
   *
   * @return The current unit object.
   */
  UnitInformation& name(const std::string& name) noexcept;

  /**
   * Returns the name of the unit.
   *
   * @return The name of the unit.
   */
  const std::string& getName() const noexcept;

  /**
   * Adds a range of RegisterInformation objects to the unit.
   *
   * @tparam Range A range-like type.
   *
   * @param range A range of RegisterInformation objects.
   *
   * @return The current unit object.
   */
  template <typename Range>
  UnitInformation& addRegisters(const Range& range) {
    Utility::concatenate(_container, range);

    return *this;
  }

  /**
   * Adds a list of RegisterInformation objects to the unit.
   *
   * @param regs A list of RegisterInformation objects.
   *
   * @return The current unit object.
   */
  UnitInformation& addRegisters(List regs);

  /**
   * Adds a single RegisterInformation object to the unit.
   *
   * @param reg The RegisterInformation object to add.
   *
   * @return the current object.
   */
  UnitInformation& addRegister(const RegisterInformation& reg);

  /** @copydoc Builder::isValid() */
  bool isValid() const noexcept override;

 private:
  /** The name of the unit, e.g. "CPU". */
  std::string _name;
};

#endif /* ERAGPSIM_ARCH_UNIT_INFORMATION_HPP */
