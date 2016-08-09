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

#include "common/builder.hpp"

/**
 * Holds information about a datatype.
 *
 * This is useful only for libraries that explicitly define different
 * data-types, like x86, where there are `qword`, `dword`, `word` and `byte`
 * types. Other architectures like RISC-V don't really have different data types
 * in the sense that you can modify an instruction by adding these data type
 * identifiers. the instructions all operate on fixed data-types, e.g. `add`
 * expects 32-bit integers for RVI32 and 64-bit integers for RVI64.
 *
 * A datatype is defined by a name and size in bits, e.g. (dword, 32).
 *
 * The class' interface is intended to support the Builder pattern
 */
class DataTypeInformation : public Builder {
 public:
  using size_t = unsigned short;

  /**
   * Constructs the datatype information object.
   *
   * @param name The name of the datatype, e.g. "dword".
   * @param size The size of the datatype, in bits.
   */
  DataTypeInformation(const std::string& name, size_t size);

  /**
   * Sets the name of the datatype.
   *
   * @param name The name for the data type.
   *
   * @return The current datatype object.
   */
  DataTypeInformation& name(const std::string& name);

  /**
   * Returns the name of the datatype.
   *
   * @return The name of the datatype.
   */
  const std::string& getName() const noexcept;

  /**
   * Sets the size of the datatype, in bits.
   *
   * @param size The new size for the datatype, in bits.
   *
   * @return The current datatype object.
   */
  DataTypeInformation& size(size_t size);

  /**
   * Returns the size of the datatype.
   *
   * @return The size of the datatype.
   */
  size_t size() const noexcept;

  /** @copydoc Builder::isValid() */
  bool isValid() const noexcept override;

 private:
  /** The name of the datatype. */
  std::string _name;

  /** The size of the datatype, in bits. */
  size_t _size;
};

#endif /* ERAGPSIM_ARCH_DATATYPE_INFORMATION_HPP */
