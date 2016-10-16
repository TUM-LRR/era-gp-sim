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

#ifndef ERAGPSIM_ARCH_COMMON_INSTRUCTION_KEY_HPP
#define ERAGPSIM_ARCH_COMMON_INSTRUCTION_KEY_HPP

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "arch/common/information-interface.hpp"
#include "common/builder-interface.hpp"
#include "common/container-adapter.hpp"
#include "common/utility.hpp"

/**
 * Identifies an instruction in an ISA.
 *
 * It is naive to think that an instruction is identified uniquely through its
 * opcode alone. In RISC-V, the opcode is a quite general *instruction group*
 * identifier. For example, all load operations and all store operations have
 * the same opcode. Next to the opcode, almost all instructions have *function
 * bits*, which further specialize an instruction. For example, for loads, the
 * instructions for load operations of different sizes (`LH`, `LD`) have the
 * same opcode, but different function bits (in this case referred to as the
 * `width`). These bits would tell the decoder the width and sign of the data to
 * load (and analogously, store, for `S[BHWD][U]`). It is thus necessary to
 * identify instructions through a tuple of keys. In practice, having different
 * fixed-size key arrays for different instruction types would be infeasible or
 * impossible (you would need to derive different subclasses with new members,
 * but then polymorphism wouldn't work anyway). Rather, the solution
 * is to use a hashmap, mapping key names to key values. A key name would be,
 * for example, the "opcode", "function bits", "format bits" for floating point
 * instructions. The values would be the respective key values, i.e. the numeric
 * values of the bit sequences for the given key. For example, this would mean
 * the value `0b000 = 0` for the width bits of load instructions in RISC-V when
 * the instruction is `LB`, or `0b001 = 1` for `LH` (16-bit halfword).
 *
 * Every instruction key must have at least one entry named "opcode".
 */
class InstructionKey
        : public ContainerAdapter<std::unordered_map<std::string, std::size_t>>,
          public InformationInterface {
public:
    using super = ContainerAdapter<std::unordered_map<std::string, std::size_t>>;
    using CONTAINER_ADAPTER_MEMBERS;
    using Key             = std::string;
    using Value           = std::size_t;
    using Entry           = std::pair<Key, Value>;
    using KeyCollection   = std::vector<Key>;
    using ValueCollection = std::vector<Value>;
    using InitializerList = std::initializer_list<Entry>;

    /**
     * Constructs an instruction key from the given serialized data.
     *
     * \param data The data to deserialize from.
     *
   */
    explicit InstructionKey(InformationInterface::Format& data);


    /**
     * Constructs the instruction key from the given range of entries.
     *
     * The range should contain `std::pair<std::string, std::size_t>` entries.
     *
     * \param range The range of entries to initialize the key with.
     */
    template <typename Range>
    explicit InstructionKey(const Range& range) : super(range) {
    }

    /**
     * Constructs the instruction key from the given initializer list.
     *
     * The initializer list must contain `std::pair<std::string, std::size_t`
     * entries.
     *
     * \param list The initializer list to construct the instruction key with.
     */
    InstructionKey(InitializerList list = InitializerList());

    /**
     * Compares two instruction keys for equality.
     *
     * Equality is performed entry-wise.
     *
     * \param other The other instruction key.
     *
     * \return True if the two keys are equal entry-for-entry, else false.
     */
    bool operator==(const InstructionKey& other) const noexcept;

    /**
     * \see operator==()
     */
    bool operator!=(const InstructionKey& other) const noexcept;

    /**
     * Deserializes an instruction key from the given serialized data.
     *
     * \param data The deserialized data.
     *
     * \return The instruction key instance.
     */
    InstructionKey& deserialize(InformationInterface::Format& data);

    /**
     * Adds an entry to the key.
     *
     * An entry is a name-value pair, with the name of the value in the
     * instruction key (e.g. "opcode") and the actual numeric value.
     *
     * \param key The key of the entry.
     * \param value The value of the entry.
     *
     * \return The instruction key instance.
     */
    InstructionKey& addEntry(const Key& key, const Value& value);

    /**
     * Adds a range of entries to the key.
     *
     * An entry is a name-value pair, with the name of the value in the
     * instruction key (e.g. "opcode") and the actual numeric value.
     *
     * \tparam A range-like type.
     *
     * \param range The range of entries to add.
     *
     * \return The instruction key instance.
     */
    template <typename Range>
    InstructionKey& addEntries(const Range& range) {
      for (const auto& pair : range) {
        addEntry(pair.first, pair.second);
      }

      return *this;
    }

    /**
     * Adds a list of entries to the key.
     *
     * An entry is a name-value pair, with the name of the value in the
     * instruction key (e.g. "opcode") and the actual numeric value.
     *
     * \param list The (initializer) list of entries to add.
     *
     * \return The instruction key instance.
     */
    InstructionKey& addEntries(InitializerList list);

    /**
     * Returns the "opcode" member of the instruction key.
     */
    const Value& opcode() const;

    /**
     * Returns the value associated with the given key.
     *
     * The value is const and can only be accessed as such.
     *
     * \param key The key to retrieve the value for.
     *
     * \return The numeric value associated with the given key.
     */
    const Value& get(const Key& key) const noexcept;

    /**
     * Returns the value associated with the given key.
     *
     * The value is const and can only be accessed as such.
     *
     * \param key The key to retrieve the value for.
     *
     * \return The numeric value associated with the given key.
     */
    const Value& operator[](const Key& key) const;

    /**
     * Tests if the instruction key contains the given key.
     *
     * \param key The key to test for.
     *
     * \return True if the instruction set contains the key, else false.
     */
    bool hasKey(const Key& key) const noexcept;

    /**
     * Returns the keys of the instruction key (i.e. the string labels).
     */
    KeyCollection getKeys() const noexcept;

    /**
     * Returns the values of the instruction key (i.e. the numeric values).
     */
    ValueCollection getValues() const noexcept;

    /** \copydoc BuilderInterface::isValid() */
    bool isValid() const noexcept override;

private:
    /** \copydoc InformationInterface::_deserialize() */
    void _deserialize(InformationInterface::Format& data) override;
};

#endif /* ERAGPSIM_ARCH_COMMON_INSTRUCTION_KEY_HPP */
