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

#include <cstddef>
#include <string>

#include "arch/common/information-interface.hpp"
#include "arch/common/instruction-key.hpp"
#include "common/builder-interface.hpp"
#include "common/optional.hpp"

/**
 * Holds information about an instruction.
 *
 * This information includes the *instruction key* of the instruction, which
 * holds the opcode and other codes to identify an instruction in the ISA, the
 * mnemonic string describing the instruction as well as the instruction format
 * specifier. The latter is an architecture-dependent string such as "F", for
 * the F-type instruction format in the RISC-V architecture.
 *
 * The class' interface is intended to support the BuilderInterface pattern.
 */
class InstructionInformation : public InformationInterface {
 public:
  using length_t = std::size_t;

  /**
  * Deserializes and constructs the `InstructionInformation` from the given
  * data.
  *
  * \param data The serialized representation of the `InstructionInformation`.
  */
  explicit InstructionInformation(InformationInterface::Format& data);

  /**
   * Constructs an instruction with a mnemonic and key.
   *
   * \param mnemonic The mnemonic of the instruction (e.g. "add")
   * \param key The key of the instruction.
   * \param format The format of the instruction.
   */
  InstructionInformation(const std::string& mnemonic = std::string());

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


  /**
   * Sets the format of the instruction.
   *
   * This format is an architecture-dependent string, such as "F" for F-type
   * instructions in RISC-V.
   *
   * \param format The format string specifier.
   *
   * \return The current instruction object.
   */
  InstructionInformation& format(const std::string& format);

  /**
   * Returns the format string of the instruction.
   */
  const std::string& getFormat() const;

  /**
   * \return True if the instruction has a format set, else false.
   */
  bool hasFormat() const noexcept;

  /**
   * Sets the length of the instruction in bits.
   *
   * This format is an architecture-dependent string, such as "F" for F-type
   * instructions in RISC-V.
   *
   * \param format The format string specifier.
   *
   * \return The current instruction object.
   */
  InstructionInformation& length(length_t length);

  /**
   * \return The length of the instruction in bits.
   */
  length_t getLength() const;

  /**
   * @return True if the instruction has a length set, else false.
   */
  bool hasLength() const noexcept;

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

  /** The format of the instruction (e.g. type "F"). */
  std::string _format;

  /** The key of the instruction. */
  InstructionKey _key;

  /** The length of the instruction, in bits. */
  Optional<length_t> _length;
};

#endif /* ERAGPSIM_ARCH_INSTRUCTION_INFORMATION_HPP */
