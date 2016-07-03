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
#include <vector>

#include "arch/common/information-interface.hpp"
#include "common/builder-interface.hpp"
#include "common/container-adapter.hpp"
#include "common/utility.hpp"

class InstructionKey
    : public ContainerAdapter<std::unordered_map<std::string, std::size_t>>,
      public InformationInterface,
      public BuilderInterface {
 public:
  using super = ContainerAdapter<std::unordered_map<std::string, std::size_t>>;
  using CONTAINER_ADAPTER_MEMBERS;
  using Key             = std::string;
  using Value           = std::size_t;
  using KeyCollection   = std::vector<Key>;
  using ValueCollection = std::vector<Value>;

  explicit InstructionKey(InformationInterface::Format& data);

  template <typename Range>
  explicit InstructionKey(const Range& range) : super(range) {
  }

  InstructionKey(InitializerList list = InitializerList());

  bool operator==(const InstructionKey& other) const noexcept;
  bool operator!=(const InstructionKey& other) const noexcept;

  InstructionKey& deserialize(InformationInterface::Format& data);

  InstructionKey& addPair(const Key& key, const Value& value);

  template <typename Range>
  InstructionKey& addPairs(const Range& range) {
    for (auto& pair : range) {
      addPair(pair.first, pair.second);
    }

    return *this;
  }

  InstructionKey& addPairs(InitializerList list);

  const Value& opcode() const;

  const Value& get(const Key& key) const noexcept;
  const Value& operator[](const Key& key) const;

  bool hasKey(const Key& key) const noexcept;

  KeyCollection getKeys() const noexcept;
  ValueCollection getValues() const noexcept;

  /** @copydoc BuilderInterface::isValid() */
  bool isValid() const noexcept override;

 private:
  void _deserialize(InformationInterface::Format& data) override;
};

#endif /* ERAGPSIM_ARCH_COMMON_INSTRUCTION_KEY_HPP */
