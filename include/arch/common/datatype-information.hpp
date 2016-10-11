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

#ifndef ERAGPSIM_ARCH_DATATYPE_INFORMATION_HPP
#define ERAGPSIM_ARCH_DATATYPE_INFORMATION_HPP

#include <string>

#include "arch/common/information-interface.hpp"
#include "common/builder-interface.hpp"

/**
 * Holds information about a data type.
 *
 * This is useful only for libraries that explicitly define different
 * data-types, like x86, where there are `qword`, `dword`, `word` and `byte`
 * types. Other architectures like RISC-V don't really have different data types
 * in the sense that you can modify an instruction by adding these data type
 * identifiers. the instructions all operate on fixed data-types, e.g. `add`
 * expects 32-bit integers for RVI32 and 64-bit integers for RVI64.
 *
 * A data type is defined by a name and size in bits, e.g. (dword, 32).
 *
 * The class' interface is intended to support the BuilderInterface pattern
 */
class DataTypeInformation : public InformationInterface {
 public:
  using size_t = unsigned short;

  DataTypeInformation() noexcept;

  /**
   * Deserializes the data type from the given data.
   *
   * \param data The data type to deserialize from.
   */
  explicit DataTypeInformation(InformationInterface::Format& data);

  /**
   * Constructs the data type information object.
   *
   * \param name The name of the data type, e.g. "dword".
   * \param size The size of the data type, in bits.
   */
  DataTypeInformation(const std::string& name, size_t size);

  /**
   * Tests for equality of two data types.
   *
   * \param other The other data type
   */
  bool operator==(const DataTypeInformation& other) const noexcept;

  /**
   * Tests for inequality of two data types.
   *
   * \param other The other data type.
   */
  bool operator!=(const DataTypeInformation& other) const noexcept;

  /**
   * \copydoc InformationInterface::deserialize()
   */
  DataTypeInformation& deserialize(InformationInterface::Format& data);

  /**
   * Sets the name of the data type.
   *
   * \param name The name for the data type.
   *
   * \return The current data type object.
   */
  DataTypeInformation& name(const std::string& name);

  /**
   * Returns the name of the data type.
   *
   * \return The name of the data type.
   */
  const std::string& getName() const;

  /**
   * Returns whether the data type has any name set.
   */
  bool hasName() const noexcept;

  /**
   * Sets the size of the data type, in bits.
   *
   * \param size The new size for the data type, in bits.
   *
   * \return The current data type object.
   */
  DataTypeInformation& size(size_t size);

  /**
   * Returns the size of the data type.
   *
   * \return The size of the data type.
   */
  size_t getSize() const;

  /**
   * Returns whether the data type has any size set.
   */
  bool hasSize() const noexcept;

  /** \copydoc BuilderInterface::isValid() */
  bool isValid() const noexcept override;

 private:
  /**
   * Deserializes the data type from the given data.
   *
   * \param data The data type to deserialize from.
   */
  void _deserialize(InformationInterface::Format& data) override;

  /** The name of the data type. */
  std::string _name;

  /** The size of the data type, in bits. */
  size_t _size;
};

#endif /* ERAGPSIM_ARCH_DATATYPE_INFORMATION_HPP */
