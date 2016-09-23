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

#ifndef ERAGPSIM_ARCH_COMMON_CONSTITUENT_INFORMATION_HPP
#define ERAGPSIM_ARCH_COMMON_CONSTITUENT_INFORMATION_HPP

#include <cstddef>

#include "arch/common/information-interface.hpp"
#include "common/optional.hpp"

/**
 * Stores information about a constituent register.
 *
 * In our architecture model, a register may have *constituent* registers,
 * such as AX, AH and AL for EAX in x86. This "parent" register then becomes
 * known as the *enclosing* register for these "children". This class stores
 * information about such a constituent-enclosing relationship in form of the
 * the register ID fo the constituent and its index within the enclosing
 * register. This information is then stored in the *enclosing* register.
 */
class ConstituentInformation final : public InformationInterface {
 public:
  using id_t        = std::size_t;
  using bit_index_t = unsigned short;
  using super       = InformationInterface;

  /**
   * Deserializes the constituent information from a data format.
   *
   * \param data The data to deserialize from.
   */
  explicit ConstituentInformation(InformationInterface::Format& data);

  /**
   * Constructs an empty constituent information object.
   */
  ConstituentInformation() noexcept;

  /**
   * Constructs a constituent information object.
   *
   * \param id The ID of the constituent register.
   * \param enclosingIndex The bit index of the constituent within the
   *                       enclosing register.
   */
  ConstituentInformation(id_t id, bit_index_t enclosingIndex) noexcept;

  /**
   * Deserializes the constituent information from the given data.
   *
   * \param data The data to deserialize from.
   *
   * \return The current constituent information instance.
   */
  ConstituentInformation& deserialize(InformationInterface::Format& data);

  /**
   * Tests for equality between this object and another instance.
   *
   * \param other The constituent information to compare to.
   *
   * \return True if the ID and bit index of the two constituents are equal,
   *         else false.
   */
  bool operator==(const ConstituentInformation& other) const noexcept;

  /**
   * Tests for inequality between this object and another instance.
   *
   * \param other The constituent information to compare to.
   *
   * \return True if the ID or bit index of the two constituents are unequal,
   *         else false.
   */
  bool operator!=(const ConstituentInformation& other) const noexcept;

  /**
   * Sets the id of the constituent information object.
   *
   * \param id The new ID of the constituent information.
   *
   * \return The current constituent information instance.
   */
  ConstituentInformation& id(id_t id) noexcept;

  /**
   * Tests if the instance has a constituent information object.
   */
  bool hasID() const noexcept;

  /**
   * Returns the Id fo the constituent register.
   */
  id_t getID() const;

  /**
   * Sets the id of the constituent information object.
   *
   * \param id The new ID of the constituent information.
   *
   * \return The current constituent information instance.
   */
  ConstituentInformation& enclosingIndex(bit_index_t index) noexcept;

  /**
   * Tests if the instance has an enclosing index set.
   */
  bool hasEnclosingIndex() const noexcept;

  /**
   * Returns the bit index of the contituent
   * register within its enclosing register.
   */
  bit_index_t getEnclosingIndex() const;

  /**
   * Tests if the constituent information object has all required members set.
   */
  bool isValid() const noexcept override;

 private:
  /** \copydoc InformationInterface::_deserialize() */
  void _deserialize(InformationInterface::Format& data) override;

  /** The ID of the constituent register. */
  Optional<id_t> _id;

  /** The index of the constituent register within its enclosing register. */
  Optional<bit_index_t> _enclosingIndex;
};

#endif /* ERAGPSIM_ARCH_COMMON_CONSTITUENT_INFORMATION_HPP */
