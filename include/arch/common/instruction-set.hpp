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

#include <vector>

#include "arch/common/information.hpp"
#include "arch/common/instruction-information.hpp"
#include "common/container-adapter.hpp"
#include "common/utility.hpp"

/**
 * This class holds a set of commands.
 *
 * It is really just a light-weight wrapper around a vector (it doesn't even
 * have a name).
 */
class InstructionSet
    : public ContainerAdapter<std::vector<InstructionInformation>>,
      public Builder,
      public Information {
 public:
  using super = ContainerAdapter<std::vector<InstructionInformation>>;
  using super::_container;
  using super::InitializerList;
  using super::begin;
  using super::cbegin;
  using super::end;
  using super::cend;
  using super::clear;
  using super::size;
  using super::isEmpty;

  /**
   * Constructs a new empty construction set.
   */
  InstructionSet() noexcept;

  /**
   * Deserializes and constructs an InstructionSet from the given data.
   *
   * @param data The serialized data.
   */
  explicit InstructionSet(const Information::Format& data);

  /**
   * Constructs a new instruction set from a range of instructions.
   *
   * @tparam Range a range-like type.
   *
   * @param range A range of instructions to add to the set.
   */
  template <typename Range>
  explicit InstructionSet(const Range& range) : super(range) {
  }

  /**
   * Constructs a new instruction set from the list of instructions.
   *
   * @param instructions A list of instructions to add to the set.
   */
  explicit InstructionSet(InitializerList instructions);

  /**
   * Deserializes an InstructionSet from the given data.
   *
   * @param data The serialized data.
   *
   * @return The current InstructionSet object.
   */
  InstructionSet& deserialize(const Information::Format& data);

  /**
   * Adds a range of InstructionInformation objects to the unit.
   *
   * @tparam Range A range-like type.
   *
   * @param range A range of InstructionInformation objects.
   *
   * @return The current instruction object.
   *
   * @see addInstructions
   */
  template <typename Range>
  InstructionSet& operator+=(const Range& range) {
    return addInstructions(range);
  }

  /**
   * Returns the result of adding the instructions to this set, without
   * modifying this set.
   *
   * A copy is made of this object, then modified via +=, then returned.
   *
   * @tparam Range A range-like type.
   *
   * @param range The range of InstructionInformation objects to add.
   *
   * @return The current instruction object.
   */
  template <typename Range>
  InstructionSet operator+(const Range& other) const {
    auto temp = *this;
    temp += other;

    return temp;
  }

  /**
   * Adds a range of InstructionInformation objects to the unit.
   *
   * @tparam Range A range-like type.
   *
   * @param range A range of InstructionInformation objects.
   *
   * @return The current instruction object.
   *
   * @see operator+=
   */
  template <typename Range>
  InstructionSet& addInstructions(const Range& range) {
    Utility::concatenate(_container, range);
    return *this;
  }

  /**
   * Adds the list of instructions to the set.
   *
   * @param list A list of instructions to add to the set.
   *
   * @return The current instruction object.
   */
  InstructionSet& addInstructions(InitializerList instructions);

  /**
   * Adds a single instruction to the set.
   *
   * @param instruction The instruction to add.
   *
   * @return The current instruction object.
   */
  InstructionSet& addInstruction(const InstructionInformation& instruction);

  /** @copydoc Builder::isValid() */
  bool isValid() const noexcept override;

 private:
  /**
   * Deserializes an InstructionSet from the given data.
   *
   * @param data The serialized data.
   */
  void _deserialize(const Information::Format& data) override;
};

#endif /* ERAGPSIM_ARCH_INSTRUCTION_SET_HPP */
