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

#ifndef ERASIM_ARCH_UNIT_INFORMATION_HPP
#define ERASIM_ARCH_UNIT_INFORMATION_HPP

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "arch/common/register-information.hpp"
#include "common/builder-interface.hpp"
#include "common/container-adapter.hpp"
#include "common/utility.hpp"

using UnderlyingRegisterContainer = ContainerAdapter<
    std::unordered_map<RegisterInformation::id_t, RegisterInformation>>;

/**
 * This class holds information about a *unit*.
 *
 * A *unit* is basically a named container of registers. For example, the
 * "CPU"
 * unit brings integer registers and the "FPU" unit brings floating point
 * registers.
 */
class UnitInformation : public UnderlyingRegisterContainer,
                        public InformationInterface {
 public:
  using super = UnderlyingRegisterContainer;
  using CONTAINER_ADAPTER_MEMBERS;
  using Type = RegisterInformation::Type;
  using id_t = RegisterInformation::id_t;
  using InitializerList = std::initializer_list<RegisterInformation>;
  using SpecialMap = std::unordered_map<Type, RegisterInformation>;
  using Compare = std::function<bool(const RegisterInformation&,
                                     const RegisterInformation&)>;
  using SortedResult =
      std::vector<std::reference_wrapper<const RegisterInformation>>;

  /**
  * Convenience comparator for sorting RegisterInformation in ascending
  * alphabetical order (by name)
  */
  struct AlphabeticOrder {
   public:
    bool operator()(const RegisterInformation& first,
                    const RegisterInformation& second) const {
      return first.getName() < second.getName();
    }
  };

  /**
   * Convenience comparator for sorting RegisterInformation in ascending
   * numerical order (by ID)
   */
  struct IdOrder {
   public:
    bool operator()(const RegisterInformation& first,
                    const RegisterInformation& second) const {
      return first.getID() < second.getID();
    }
  };

  /**
   * Convenience comparator for sorting RegisterInformation in a custom order
   * defined
   * by type.
   */
  struct TypeOrder {
    using TypeList = std::initializer_list<Type>;

    /**
     * Creates a new Type Comparator
     * \param types List of types that define the sorting order. {A, B, C} means
     * the following order:
     * A < B < C
     * @param compareWhenEqual Another Comparator that is used for defining an
     * order inside a type group.
     * {A, B, C} & AlphabeticOrder results into: <a10, a20, a30, b1, b2, b3, c1>
     */
    TypeOrder(TypeList types = TypeList(),
              Compare compareWhenEqual = AlphabeticOrder())
    : _typeOrder(types), _equalCompare(compareWhenEqual) {
    }

    bool operator()(const RegisterInformation& first,
                    const RegisterInformation& second) const {
      auto indexFirst =
          std::find(_typeOrder.cbegin(), _typeOrder.cend(), first.getType());
      auto indexSecond =
          std::find(_typeOrder.cbegin(), _typeOrder.cend(), second.getType());
      if (indexFirst == indexSecond) {
        // same type
        return _equalCompare(first, second);
      }
      // different type
      return indexFirst < indexSecond;
    }

   private:
    std::vector<Type> _typeOrder;
    Compare _equalCompare;
  };

  /**
   * Deserializes the `UnitInformation` from the given data.
   *
   * \param data The data to deserialize the unit-information from.
   */
  explicit UnitInformation(InformationInterface::Format& data);

  /**
   * Constructs a new unit with the give name.
   *
   * \param name The name of the unit, e.g. "cpu".
   */
  explicit UnitInformation(const std::string& name = std::string());

  /**
   * Constructs a new unit with the given name and list of registers.
   *
   * \param name The name of the unit, e.g. "cpu".
   * \param list A list of registers to add to the unit.
   */
  UnitInformation(const std::string& name, InitializerList list);

  /**
   * Constructs a new unit with the given name and range of registers.
   *
   * \tparam Range A range-like type.
   *
   * \param name The name of the unit, e.g. "cpu".
   * \param range The range of RegisterInformation objects to add.
   */
  template <typename Range>
  UnitInformation(const std::string& name, const Range& range) : _name(name) {
    addRegisters(range);
  }

  /**
   * Tests for equality of two units.
   *
   * The name of the unit and the registers in the unit must be equal.
   *
   * \param other The other unit.
   */
  bool operator==(const UnitInformation& other) const noexcept;

  /**
   * Tests for inequality of two units.
   *
   * The name of the unit and the registers in the unit must be equal.
   *
   * \param other The other unit.
   */
  bool operator!=(const UnitInformation& other) const noexcept;

  /**
   * Adds the registers in the other unit to this one.
   *
   * \param other The other unit to get the registers from.
   *
   * \return The current unit object.
   *
   * \see addInstructions
   */
  UnitInformation& operator+=(const UnitInformation& other);

  /**
   * Adds a range of RegisterInformation objects to the unit.
   *
   * \tparam Range A range-like type.
   *
   * \param range A range of RegisterInformation objects.
   *
   * \return The current unit object.
   *
   * \see addInstructions
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
   * \tparam Range A range-like type.
   *
   * \param range The range of RegisterInformation objects to add.
   *
   * \return The current unit object.
   */
  template <typename Range>
  UnitInformation operator+(const Range& other) const {
    auto temp = *this;
    temp += other;

    return temp;
  }

  /**
   * Deserializes the `UnitInformation` from the given data.
   *
   * \param data The data to deserialize the unit-information from.
   *
   * \return The current unit object.
   */
  UnitInformation& deserialize(InformationInterface::Format& data);

  /**
   * Sets the name of the unit.
   *
   * This would usually be "cpu", "fpu" (floats) or "vpu" (vector processing).
   *
   * \param name The new name for the unit.
   *
   * \return The current unit object.
   */
  UnitInformation& name(const std::string& name);

  /**
   * Returns the name of the unit.
   */
  const std::string& getName() const;

  /**
   * Tests whether this unit has any name set.
   */
  bool hasName() const noexcept;

  /**
   * Returns the mapping from special register types
   * to special register information objects.
   */
  const SpecialMap& getSpecialRegisters() const noexcept;

  /**
   * Returns information about a type of special register, if any such register
   * was registered.
   */
  const RegisterInformation& getSpecialRegister(Type type) const;

  /**
   * Returns whether any special register of the given type was set for the
   * unit.
   */
  bool hasSpecialRegister(Type type) const noexcept;

  /**
   * Returns whether the unit has any special registers at all.
   */
  bool hasSpecialRegisters() const noexcept;

  /**
   * Adds a range of RegisterInformation objects to the unit.
   *
   * \tparam Range A range-like type.
   *
   * \param range A range of RegisterInformation objects.
   *
   * \return The current unit object.
   */
  template <typename Range>
  UnitInformation& addRegisters(const Range& range) {
    _container.reserve(range.size());

    // Add them individually instead of via range insertion
    // so that we can check for special registers
    for (auto& registerInformation : range) {
      addRegister(registerInformation);
    }

    return *this;
  }

  /**
   * Adds a list of RegisterInformation objects to the unit.
   *
   * \param registers A list of RegisterInformation objects.
   *
   * \return The current unit object.
   */
  UnitInformation& addRegisters(InitializerList registers);

  /**
   * Adds a single RegisterInformation object to the unit.
   *
   * \param registerInformation The RegisterInformation object to add.
   *
   * \return the current object.
   */
  UnitInformation& addRegister(const RegisterInformation& registerInformation);

  /**
   * Returns the information object for the given register identifier.
   *
   * \param registerID The identifier of the register to return.
   *
   * \return The register in the unit with the given identifier.
   */
  const RegisterInformation& getRegister(id_t registerID) const;

  /**
   * Tests if the unit has the register with the given ID  or not.
   */
  bool hasRegister(id_t registerID) const noexcept;

  /**
   * Returns all registers that aren't special registers.
   * The result is sorted using the given Comparator
   * \param comparator See AlphabeticOrder, IdOrder and TypeOrder
   */
  SortedResult getRegisterSorted(const Compare& comparator) const;
  /**
   * Returns all registers that are special registers.
   * The result is sorted using the given Comparator
   * \param comparator See AlphabeticOrder, IdOrder and TypeOrder
   */
  SortedResult getSpecialRegisterSorted(const Compare& comparator) const;
  /**
   * Returns all registers (both special and not special).
   * The result is sorted using the given Comparator
   * \param comparator See AlphabeticOrder, IdOrder and TypeOrder
   */
  SortedResult getAllRegisterSorted(const Compare& comparator) const;

  /** \copydoc BuilderInterface::isValid() */
  bool isValid() const noexcept override;

 private:
  /**
   * Returns a sorted vector containing all elements of range using the given
   * comparator
   * \tparam Range Container type
   * \param range
   * \param comparator
   * \return SortedResult containing all elements of range, sorted using
   * comparator
   */
  template <class Range>
  SortedResult _getSorted(Range& range, const Compare& comparator) const {
    SortedResult result;
    for (const auto& element : range) {
      result.emplace_back(
          std::reference_wrapper<const RegisterInformation>(element.second));
    }

    std::sort(result.begin(), result.end(), comparator);

    return result;
  }

  /**
   * Deserializes the `UnitInformation` from the given data.
   *
   * \param data The data to deserialize the unit-information from.
   */
  void _deserialize(InformationInterface::Format& data) override;

  /** The name of the unit, e.g. "CPU". */
  std::string _name;

  /** A map of special registers, if any. */
  SpecialMap _specialRegisters;
};

#endif /* ERASIM_ARCH_UNIT_INFORMATION_HPP */
