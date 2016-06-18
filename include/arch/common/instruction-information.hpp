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

#ifndef ERAGPSIM_ARCH_INSTRUCTION_INFORMATION_HPP
#define ERAGPSIM_ARCH_INSTRUCTION_INFORMATION_HPP

#include <string>

#include "common/builder.hpp"
#include "common/optional.hpp"

/**
 * Holds information about an instruction.
 *
 * The information is currently constrained only to the name and key of the
 * instruction, as everything else (e.g. allowed operands) is hard-coded in the
 * respective implementation class.
 *
 * The class' interface is intended to support the Builder pattern.
 */
class InstructionInformation : public Builder {
 public:
	// For now
	using InstructionKey = unsigned short;

	/**
	 * Constructs an instruction with a name.
	 *
	 * @param name The name of the instruction (e.g. "add")
	 */
	InstructionInformation(const std::string& name);

	/**
	 * Constructs an instruction with a name and key.
	 *
	 * @param name The name of the instruction (e.g. "add")
	 * @param key The key of the instruction.
	 */
	InstructionInformation(const std::string& name, const InstructionKey& key);

	/**
	 * Sets the name of the instruction.
	 *
	 * @param name The new name for the instruction.
	 *
	 * @return The current instruction object.
	 */
	InstructionInformation& name(const std::string& name);

	/**
	 * Returns the name of the instruction.
	 */
	const std::string& getName() const noexcept;

	/**
	 * Sets the key of the instruction.
	 *
	 * @param key The new key for the instruction.
	 *
	 * @return The current instruction object.
	 */
	InstructionInformation& key(const InstructionKey& key);

	/**
	 * Returns the key of the instruction.
	 */
	const InstructionKey& getKey() const noexcept;

	/** @copydoc Builder::isValid() */
	bool isValid() const noexcept override;

 private:
	/** The name of the instruction. */
	std::string _name;

	/** The key of the instruction. */
	Optional<InstructionKey> _key;
};

#endif /* ERAGPSIM_ARCH_INSTRUCTION_INFORMATION_HPP */
