/* C++ Assembler Interpreter
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef ERAGPSIM_TESTS_ARCH_RISCV_DUMMIES_HPP_
#define ERAGPSIM_TESTS_ARCH_RISCV_DUMMIES_HPP_

#include <cstdint>
#include <iostream>

#include "arch/common/abstract-syntax-tree-node.hpp"
#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/register-node.hpp"
#include "arch/riscv/instruction-node-factory.hpp"
#include "arch/riscv/instruction-node.hpp"

#include "core/conversions.hpp"

using namespace riscv;

template <typename IntType>
MemoryValue convertToMem(IntType t) {
  return convert<IntType>(t,
                          InstructionNode::RISCV_BITS_PER_BYTE,
                          InstructionNode::RISCV_ENDIANNESS);
}

class FakeRegister {
 public:
  FakeRegister() : FakeRegister(0) {
  }
  FakeRegister(uint64_t value) : _value(convertToMem<uint64_t>(value)) {
  }
  FakeRegister(MemoryValue& v) : _value(v) {
  }

  void set(uint64_t newValue) {
    _value = convertToMem<uint64_t>(newValue);
  }
  void set(MemoryValue& v) {
    _value = v;
  }

  MemoryValue get() {
    return _value;
  }

 private:
  MemoryValue _value;
};


class FakeRegisterNode : public RegisterNode {
 public:
  explicit FakeRegisterNode(std::string& regId)
  : RegisterNode(regId), _id(regId) {
  }

  MemoryValue getValue(DummyMemoryAccess& access) const override {
    return access.getRegisterValue(_id);
  }

  MemoryValue assemble() const override {
    return MemoryValue{};
  }

  const std::string& getIdentifier() const override {
    return _id;
  }

 private:
  std::string& _id;
};

class DummyMemoryAccessImpl : public DummyMemoryAccess {
 public:
  void addRegister(std::string token, FakeRegister& reg) {
    _register.emplace(token, reg);
  }

  MemoryValue getRegisterValue(const std::string& token) override {
    return _register.at(token).get();
  }
  void setRegisterValue(const std::string& token, MemoryValue value) override {
    FakeRegister& desiredRegister = _register.at(token);
    desiredRegister.set(value);
  }

  MemoryValue
  getMemoryValueAt(std::size_t address, std::size_t amount) override {
    if (_memory.size() >= address + amount) {
      auto first = _memory.begin() + address;
      auto last  = _memory.begin() + (address + amount);
      return MemoryValue{std::vector<uint8_t>{first, last},
                         InstructionNode::RISCV_BITS_PER_BYTE};
    }
    // Address out of range
    std::cerr << "Address " << address << " with amount " << amount
              << " out of range" << std::endl;
    return MemoryValue{};
  }

  void setMemoryValueAt(std::size_t address, MemoryValue value) override {
    if (_memory.size() < address) {
      // Address out of range
      return;
    }
    size_t i = address;
    for (uint8_t byte : value.internal()) {
      if (i >= _memory.size()) {
        _memory.push_back(byte);
      } else {
        _memory.at(i) = byte;
      }
      ++i;
    }
  }

 private:
  std::unordered_map<std::string, FakeRegister&> _register;
  std::vector<uint8_t> _memory;
};

MemoryValue to32BitMemoryValue(uint32_t value);

MemoryValue to64BitMemoryValue(uint64_t value);

InstructionNodeFactory
setUpFactory(ArchitectureFormula::InitializerList modules);

#endif /* ERAGPSIM_TESTS_ARCH_RISCV_DUMMIES_HPP_*/
