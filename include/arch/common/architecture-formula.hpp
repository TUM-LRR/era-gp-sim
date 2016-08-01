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
#include <vector>

#include "common/builder-interface.hpp"
#include "common/container-adapter.hpp"
#include "common/utility.hpp"

/**
 * Describes an Architecture formula.
 *
 * A formula is just a list of extension names. There must be one base extension
 * and optionally any number of further extensions.
 */
class ArchitectureFormula : public ContainerAdapter<std::vector<std::string>>,
                            public BuilderInterface {
 public:
  using super = ContainerAdapter<std::vector<std::string>>;
  using CONTAINER_ADAPTER_MEMBERS;
  using index_t = typename Underlying::size_type;

  /**
   * Constructs a new ArchitectureFormula.
   *
   * \param architectureName The name of the architecture.
   * \param list A list of extension names.
   */
  explicit ArchitectureFormula(
      const std::string& architectureName = std::string(),
      InitializerList list                = InitializerList());

  /**
   Constructs a new ArchitectureFormula.
   *
   * The first element of the range must be the base.
   *
   * \tparam Range A range-like sequence type.
   *
   * \param all The range of extensions for the formula.
   */
  template <typename Range>
  explicit ArchitectureFormula(const std::string& architectureName,
                               const Range& range)
  : super(range) {
    assert(!isEmpty());
  }

  /**
   * Tests for equality of two formulae.
   *
   * \param other The other formula
   */
  bool operator==(const ArchitectureFormula& other) const noexcept;

  /**
   * Tests for inequality of two formulae.
   *
   * \param other The other formula
   */
  bool operator!=(const ArchitectureFormula& other) const noexcept;

  /**
   * Adds an extension name to the formula.
   *
   * \param name The name of the extension to add.
   *
   * \return The current formula.
   */
  ArchitectureFormula& addExtension(const std::string& name);

  /**
   * Tests if the formula has any extension (names) set.
   */
  bool hasExtensions() const noexcept;

  /**
   * Sets the architecture's name.
   *
   * \param name The new architecture name.
   *
   * \return The current formula.
   */
  ArchitectureFormula& architectureName(const std::string& name);

  /**
   * Tests if the formula has an architecture name set.
   */
  bool hasArchitectureName() const noexcept;

  /**
   * Returns the architecture's identifier (name).
   */
  const std::string& getArchitectureName() const noexcept;

  /**
   * Returns the path to the formula's configuration data.
   *
   * This is the absolute path to .isa file containing the architecture's
   * extensions. It consists of the global root path and the formula's
   * folderName.
   *
   * \see getFolderName()
   */
  std::string getPath() const noexcept;

  /**
   * Returns the relative folder name for the formula's configuration data.
   *
   * This is the name of the architecture for the formula with a ".isa"
   * extension.
   *
   * \see getPath()
   */
  std::string getFolderName() const noexcept;

  /** \copydoc Builder::isValid() */
  bool isValid() const noexcept override;

 private:
  /** The name of the architecture. */
  std::string _architectureName;
};

#endif /* ERAGPSIM_ARCH_COMMON_ARCHITECTURE_FORMULA_HPP */
