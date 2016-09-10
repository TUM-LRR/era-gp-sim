/*
* C++ Assembler Interpreter
* Copyright (C) 2016 Chair of Computer Architecture
* at Technical University of Munich
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
}* the Free Software Foundation, either version 3 of the License, or
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

#ifndef ERAGPSIM_ARCH_INSTRUCTION_SET_HPP
#define ERAGPSIM_ARCH_INSTRUCTION_SET_HPP

#include <string>
#include <unordered_map>

#include "arch/common/information-interface.hpp"
#include "arch/common/instruction-information.hpp"
#include "common/container-adapter.hpp"
#include "common/utility.hpp"

using UnderlyingInstructionSetContainer =
    ContainerAdapter<std::unordered_map<std::string, InstructionInformation>>;

/**
 * This class holds a set of commands.
 *
 * It is really just a light-weight wrapper around a vector (it doesn't even
 * have a name).
 */
class InstructionSet : public UnderlyingInstructionSetContainer,
                       public InformationInterface {
 public:
  using super = UnderlyingInstructionSetContainer;
  using CONTAINER_ADAPTER_MEMBERS;
  using InitializerList = std::initializer_list<InstructionInformation>;

  /**
   * Deserializes and constructs an InstructionSet from the given data.
   *
   * \param data The serialized data.
   */
  explicit InstructionSet(InformationInterface::Format& data);

  /**
   * Constructs a new instruction set from a range of instructions.
   *
   * \tparam Range a range-like type.
   *
   * \param range A range of instructions to add to the set.
   */
  template <typename Range>
  explicit InstructionSet(const Range& range) {
    addInstructions(range);
  }

  /**
   * Constructs a new instruction set from the list of instructions.
   *
   * \param instructions A list of instructions to add to the set.
   */
  InstructionSet(InitializerList instructions = InitializerList());

  /**
   * Tests for equality of two instruction sets.
   *
   * All instructions in the sets must be equal.
   *
   * \param other The other instruction set.
   */
  bool operator==(const InstructionSet& other) const noexcept;

  /**
   * Tests for inequality of two instruction sets.
   *
   * All instructions in the sets must be equal.
   *
   * \param other The other instruction set.
   */
  bool operator!=(const InstructionSet& other) const noexcept;

  /**
   * Adds a range of InstructionInformation objects to this set.
   *
   * \tparam Range A range-like type.
   *
   * \param range A range of InstructionInformation objects.
   *
   * \return The current instruction object.
   *
   * \see addInstructions
   */
  template <typename Range>
  InstructionSet& operator+=(const Range& range) {
    return addInstructions(range);
  }

  /**
   * Adds another instruction set.
   *
   * \param other Another instruciton set object.
   *
   * \return The current instruction object.
   *
   * \see addInstructions()
   */
  InstructionSet& operator+=(const InstructionSet& other);

  /**
   * Returns the result of adding the instructions to this set, without
   * modifying this set.
   *
   * A copy is made of this object, then modified via +=, then returned.
   *
   * \tparam Range A range-like type.
   *
   * \param range The range of InstructionInformation objects to add.
   *
   * \return The current instruction object.
   */
  template <typename Range>
  InstructionSet operator+(const Range& other) const {
    auto temp = *this;
    temp += other;

    return temp;
  }

  /**
   * Returns the instruction with the given mnemonic in the set.
   *
   * Inclusion is asserted.
   *
   * \param mnemonic The mnemonic of the instruction to look for.
   *
   * \return The full instruction information object.
   *
   * \see getInstruction()
   * \see getInstructionKey()
   */
  const InstructionInformation& operator[](const std::string& mnemonic) const;

  /**
   * Returns the instruction with the given mnemonic in the set.
   *
   * Inclusion is asserted.
   *
   * \param mnemonic The mnemonic of the instruction to look for.
   *
   * \return The full instruction information object.
   *
   * \see operator[]()
   * \see getInstructionKey()
   */
  const InstructionInformation&
  getInstruction(const std::string& mnemonic) const;

  /**
   * Returns the key of the instruction with the given mnemonic in th set.
   *
   * Inclusion is asserted.
   *
   * \param mnemonic The mnemonic of the instruction to look for.
   *
   * \return Only the `InstructionKey` of the instruction information object.
   *
   * \see operator[]()
   * \see getInstruction()
   */
  const InstructionKey& getInstructionKey(const std::string& mnemonic) const;

  /**
   * Tests if an instruction with the given mnemonic exists in the set.
   *
   * \param mnemonic The mnemonic to test inclusion for.
   */
  bool hasInstruction(const std::string& mnemonic) const;

  /**
   * Deserializes an InstructionSet from the given data.
   *
   * \param data The serialized data.
   *
   * \return The current InstructionSet object.
   */
  InstructionSet& deserialize(InformationInterface::Format& data);

  /**
   * Adds a range of InstructionInformation objects to the unit.
   *
   * \tparam Range A range-like type.
   *
   * \param range A range of InstructionInformation objects.
   *
   * \return The current instruction object.
   *
   * \see operator+=
   */
  template <typename Range>
  InstructionSet& addInstructions(const Range& range) {
    for (const auto& instruction : range) {
      addInstruction(instruction);
    }
    return *this;
  }

  /**
   * Adds another instruction set.
   *
   * \param other Another instruciton set object.
   *
   * \return The current instruction object.
   *
   * \see operator+=()
   */
  InstructionSet& addInstructions(const InstructionSet& other);

  /**
   * Adds a single instruction to the set.
   *
   * \param instruction The instruction to add.
   *
   * \return The current instruction object.
   */
  InstructionSet& addInstruction(const InstructionInformation& instruction);

  /** \copydoc BuilderInterface::isValid() */
  bool isValid() const noexcept override;

 private:
  /**
   * Deserializes an InstructionSet from the given data.
   *
   * \param data The serialized data.
   */
  void _deserialize(InformationInterface::Format& data) override;
};

#endif /* ERAGPSIM_ARCH_INSTRUCTION_SET_HPP */
