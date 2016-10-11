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

#ifndef ERAGPSIM_ARCH_COMMON_ARCHITECTURE_HPP
#define ERAGPSIM_ARCH_COMMON_ARCHITECTURE_HPP

#include <memory>
#include <string>
#include <vector>

#include "arch/common/architecture-properties.hpp"
#include "arch/common/extension-information.hpp"
#include "arch/common/information-interface.hpp"
#include "arch/common/instruction-set.hpp"
#include "arch/common/node-factory-collection.hpp"
#include "arch/common/unit-container.hpp"
#include "arch/common/unit-information.hpp"
#include "common/builder-interface.hpp"

class ArchitectureFormula;

/**
 * This class holds all information about an architecture.
 *
 * Due to the modular design an architecture is really nothing else than a named
 * union of extensions. There is one "base extension" which must be supplied,
 * while other extension may be added optionally.
 *
 * An `Architecture` is really just a light-weight adapter for an extension.
 *
 */
class Architecture : public BuilderInterface {
 public:
  using ExtensionNameCollection = ExtensionInformation::ExtensionNameCollection;
  using Endianness              = ArchitectureProperties::Endianness;
  using AlignmentBehavior       = ArchitectureProperties::AlignmentBehavior;
  using SignedRepresentation    = ArchitectureProperties::SignedRepresentation;
  using word_size_t             = ArchitectureProperties::word_size_t;

  /**
   * Brews an architecture given a formula.
   *
   * This will probably be the primary entry point for creating architectures,
   * as it is the main interface for loading serialized ISA models. It is a
   * factory method that takes an `Architecture::Formula`, describing which
   * extensions should make up the architecture. Those extensions will then be
   * loaded and deserialized to produce an Architecture (via its builder
   * interface).
   *
   * \param formula The formula describing how to brew the architecture.
   *
   * \return A complete architecture instance.
   */
  static Architecture Brew(const ArchitectureFormula& formula);

  /**
   * Constructs an architecture with the given name.
   *
   * \param name The name of the architecture.
   */
  explicit Architecture(const std::string& name = std::string());

  /**
   * Constructs an architecture with the given name and base extension.
   *
   * Every architecture must have a name and consist of at least one (base)
   * extension. The base extension must return true for its `isComplete()`
   * method.
   *
   * \param name The name of the architecture.
   * \param base The base extension of the architecture.
   */

  Architecture(const std::string& name, const ExtensionInformation& base);

  /**
   * Adds an extension to the architecture.
   *
   * \param extension The extension to extend the architecture with.
   *
   * \return The current architecture instance.
   *
   * \see extendBy()
   * \see operator+()
   */
  Architecture& operator+=(const ExtensionInformation& extension);

  /**
   * Returns a copy of the result of extending this architecture.
   *
   * \param extension The extension to extend the architecture with.
   *
   * \return The result of the addition.
   *
   * \see extendBy()
   * \see operator+=()
   */
  Architecture operator+(const ExtensionInformation& extension) const;


  /**
   * Adds an extension to the architecture.
   *
   * Extending an architecture does the same thing as merging an extension with
   * another. That is, units and instructions are merged while alignment,
   * endianness and similar general properties (of which an architecture must
   * have only one) are overriden by the properties of the extension.
   *
   * \param extension The extension to extend the architecture with.
   *
   * \return The current architecture instance.
   *
   * \see operator+=()
   * \see operator+()
   */
  Architecture& extendBy(const ExtensionInformation& extension);

  /**
   * Sets the name of the archiecture.
   *
   * \param name The new name for the architecture.
   *
   * \return The current architecture instance.
   */
  Architecture& name(const std::string& name);

  /**
   * Returns the name of the architecture.
   *
   */
  const std::string& getName() const;

  /**
   * Returns the endianness of the architecture.
   *
   * This property must have already been set by extending the architecture with
   * an extension and validated by calling `validate()`. This is asserted!
   */
  Endianness getEndianness() const;

  /**
   * Returns the signed representation of the architecture.
   *
   * This property must have already been set by extending the architecture with
   * an extension and validated by calling `validate()`. This is asserted!
   */
  SignedRepresentation getSignedRepresentation() const;

  /**
   * Returns the alignment behavior of the architecture.
   *
   * This property must have already been set by extending the architecture with
   * an extension and validated by calling `validate()`. This is asserted!
   */
  AlignmentBehavior getAlignmentBehavior() const;


  /**
   * Returns the word size of the architecture.
   *
   * This property must have already been set by extending the architecture with
   * an extension and validated by calling `validate()`. This is asserted!
   */
  word_size_t getWordSize() const;

  /**
   * Returns the units of the architecture.
   *
   * This property must have already been set by extending the architecture with
   * an extension and validated by calling `validate()`. This is asserted!
   */
  const UnitContainer& getUnits() const;

  /**
   * Returns the instructions of the architecture.
   *
   * This property must have already been set by extending the architecture with
   * an extension and validated by calling `validate()`. This is asserted!
   */
  const InstructionSet& getInstructions() const;

  /**
   * Sets the architecture's node factory collection.
   *
   * This is a template method to more flexibly copy *or* move another
   * collection with only a single method rather than two.
   *
   * \tparam An NodeFactoryCollection lvalue or rvalue.
   *
   * \param factoryCollection The factoryCollection to move or assign.
   *
   * \return The architecture instance.
   */
  template <typename FactoryCollection>
  Architecture& nodeFactories(FactoryCollection&& factoryCollection) {
    _factories = std::forward<FactoryCollection>(factoryCollection);
    return *this;
  }

  /**
   * Returns the architecture's node factory collection.
   */
  const NodeFactoryCollection& getNodeFactories() const;

  /**
   * Tests if the architecture is based on a certain extension.
   */
  bool isBasedOn(const std::string& extension_name) const noexcept;

  /**
   * Returns a collection of names of the extensions the
   * architecture is based on.
   */
  const ExtensionNameCollection& getBaseExtensionNames() const noexcept;

  /**
   * Validates the completeness of the architecture.
   *
   * An architecture is valid if its base extension, extended by all further
   * extensions, is still a valid base extension. That is,
   * `ExtensionInformation::isComplete()` return true. See the documentation
   * for that method to see what constraints are placed on a valid base
   * extension.
   *
   * \return The current architecture instance.
   */
  Architecture& validate();

  /**
   * Whether or not this architecture has been validated yet.
   *
   * Returns true if `validate()` was called *after the last extension was
   * added* (if any other than the base was added). If it returns true, it is
   * safe to access this architecture's getter methods.
   */
  bool isValidated() const noexcept;

  /** \copydoc BuilderInterface::isValid() */
  bool isValid() const noexcept override;

 private:
  /** The name of the architecture. */
  std::string _name;

  /** The base extension of the architecture. */
  ExtensionInformation _base;

  /** The abstract node factories. */
  NodeFactoryCollection _factories;

  /** Boolean indicating whether the architecture has been validated. */
  bool _validated;
};

#endif /* ERAGPSIM_ARCH_COMMON_ARCHITECTURE_HPP */
