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

#include "arch/common/information-interface.hpp"
#include "arch/common/instruction-key.hpp"
#include "common/builder-interface.hpp"

/**
 * Holds information about an instruction.
 *
 * The information is currently constrained only to the mnemonic and key of the
 * instruction, as everything else (e.g. allowed operands) is hard-coded in the
 * respective implementation class.
 *
 * The class' interface is intended to support the BuilderInterface pattern.
 */
class InstructionInformation : public InformationInterface {
 public:
  /**
  * Deserializes and constructs the `InstructionInformation` from the given
  * data.
  *
  * \param data The serialized representation of the `InstructionInformation`.
  */
  explicit InstructionInformation(InformationInterface::Format& data);

  /**
   * Constructs an instruction with a mnemonic.
   *
   * \param mnemonic The mnemonic of the instruction (e.g. "add")
   */
  explicit InstructionInformation(const std::string& mnemonic = std::string());

  /**
   * Constructs an instruction with a mnemonic and key.
   *
   * \param mnemonic The mnemonic of the instruction (e.g. "add")
   * \param key The key of the instruction.
   */
  InstructionInformation(const std::string& mnemonic,
                         const InstructionKey& key);

  /**
   * Tests for equality of two instructions.
   *
   * The names and keys must match for two instructions to be equal.
   *
   * \param other The other instruction.
   */
  bool operator==(const InstructionInformation& other) const noexcept;

  /**
   * Tests for inequality of two instructions.
   *
   * The names and keys must match for two instructions to be equal.
   *
   * \param other The other instruction.
   */
  bool operator!=(const InstructionInformation& other) const noexcept;

  /**
  * Deserializes and constructs the `InstructionInformation` from the given
  * data.
  *
  * \param data The serialized representation of the `InstructionInformation`.
  *
  * \return The current instruction object.
  */
  InstructionInformation& deserialize(InformationInterface::Format& data);

  /**
   * Sets the mnemonic of the instruction.
   *
   * \param mnemonic The new mnemonic for the instruction.
   *
   * \return The current instruction object.
   */
  InstructionInformation& mnemonic(const std::string& mnemonic);

  /**
   * Returns the mnemonic of the instruction.
   */
  const std::string& getMnemonic() const;

  /**
   * Tests if the instruction has a mnemonic set.
   */
  bool hasMnemonic() const noexcept;

  /**
   * Sets the key of the instruction.
   *
   * \param key The new key for the instruction.
   *
   * \return The current instruction object.
   */
  InstructionInformation& key(const InstructionKey& key);

  /**
   * Returns the key of the instruction.
   */
  const InstructionKey& getKey() const;

  /**
   * Tests if the instruction has an instruction key set.
   */
  bool hasKey() const noexcept;

  /** \copydoc BuilderInterface::isValid() */
  bool isValid() const noexcept override;

 private:
  /**
  * Deserializes and constructs the `InstructionInformation` from the given
  * data.
  *
  * \param data The serialized representation of the `InstructionInformation`.
  */
  void _deserialize(InformationInterface::Format& data) override;

  /** The mnemonic of the instruction. */
  std::string _mnemonic;

  /** The key of the instruction. */
  InstructionKey _key;
};

#endif /* ERAGPSIM_ARCH_INSTRUCTION_INFORMATION_HPP */
