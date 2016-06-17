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

#include "arch/common/information.hpp"
#include "arch/common/instruction-set.hpp"
#include "arch/common/unit-information.hpp"
#include "common/builder.hpp"

class ExtensionInformation;

/**
 * This class holds all information about an architecture.
 *
 * Due to the modular design an architecture is really nothing else than a named
 * union of extensions. There is one "base extension" which must be supplied,
 * while other extension may be added optionally.
 *
 * An `Architecture` is really just a light-weight adapter for an extension.
 */
class Architecture : public Builder {
 public:
	using UnitContainer = std::vector<UnitInformation>;
	using Endianness = Information::Endianness;
	using Alignment = Information::Alignment;
	using word_size_t = Information::word_size_t;

	Architecture(const std::string& name, const ExtensionInformation& base);

	Architecture(const Architecture& other);
	Architecture(Architecture&& other) noexcept;
	Architecture& operator=(Architecture& other);
	~Architecture();

	void swap(Architecture& other) noexcept;
	friend void swap(Architecture& first, Architecture& second) noexcept;

	Architecture& operator+=(const ExtensionInformation& extension);
	Architecture operator+(const ExtensionInformation& extension) const;

	Architecture& extend(const ExtensionInformation& extension);

	/**
	 * Sets the name of the .
	 *
	 * @param name The new name for the architecture.
	 *
	 * @return The current `Architecture` object.
	 */
	Architecture& name(const std::string& name);
	const std::string& getName() const noexcept;

	Endianness getEndianness() const noexcept;
	Alignment getAlignment() const noexcept;

	/**
	 * Returns the word size of the extension in bits.
	 */
	word_size_t getWordSize() const noexcept;

	const UnitContainer& getUnits() const;
	const InstructionSet& getInstructions() const;

	Architecture& validate();
	bool isValidated() const noexcept;

	/** @copydoc Builder::isValid() */
	bool isValid() const noexcept override;

 private:
	Architecture();

	/** The name of the architecture. */
	std::string _name;

	/** The base extension of the architecture. */
	std::unique_ptr<ExtensionInformation> _base;

	/** Boolean indicating whether the architecture has been validated. */
	bool _validated;
};

#endif /* ERAGPSIM_ARCH_COMMON_ARCHITECTURE_HPP */
