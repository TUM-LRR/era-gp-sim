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

#include "core/project.hpp"

#include "common/assert.hpp"
#include "core/conversions.hpp"


Project::Project(std::weak_ptr<Scheduler> &&scheduler,
                 ArchitectureFormula &&architectureFormula,
                 std::size_t memorySize)
: Servant(std::move(scheduler))
, _architecture(Architecture::Brew(architectureFormula))
, _memory(memorySize)// TODO https://github.com/TUM-LRR/era-gp-sim/issues/83
, _registerSet() {
  _architecture.validate();

  for (UnitInformation unitInfo : _architecture.getUnits()) {
    for (auto &&registerPair : unitInfo) {
      // create all top level registers
      createRegister(registerPair.second, unitInfo);
    }
    // create special registers
    for (auto &&registerPair : unitInfo.getSpecialRegisters()) {
      createRegister(registerPair.second, unitInfo);
    }
  }
}

void Project::createRegister(RegisterInformation registerInfo,
                             UnitInformation unitInfo) {
  if (!registerInfo.hasEnclosing()) {
    _registerSet.createRegister(registerInfo.getName(), registerInfo.getSize());
    // create all constituents and their constituents
    createConstituents(registerInfo, unitInfo);
  }
}

void Project::createConstituents(RegisterInformation enclosingRegister,
                                 UnitInformation unitInfo) {
  for (auto &&constituentInformation : enclosingRegister.getConstituents()) {
    // find RegisterInformation of the constituent
    RegisterInformation constituentRegisterInfo =
        unitInfo.getRegister(constituentInformation.getID());

    // createRegister/alias
    _registerSet.aliasRegister(constituentRegisterInfo.getName(),
                               enclosingRegister.getName(),
                               constituentInformation.getEnclosingOffset(),
                               constituentInformation.getEnclosingOffset() +
                                   constituentRegisterInfo.getSize());

    // recursive call to create constituents of this constituent
    if (constituentRegisterInfo.hasConstituents()) {
      createConstituents(constituentRegisterInfo, unitInfo);
    }
  }
}

MemoryValue Project::getMemoryValue(std::size_t address, std::size_t amount) {
  return _memory.get(address, amount);
}

void Project::putMemoryValue(std::size_t address, const MemoryValue &value) {
  _memory.put(address, value);
}

MemoryValue
Project::setMemoryValue(std::size_t address, const MemoryValue &value) {
  return _memory.set(address, value);
}

MemoryValue Project::getRegisterValue(const std::string &name) const {
  return _registerSet.get(name);
}

void Project::putRegisterValue(const std::string &name,
                               const MemoryValue &value) {
  _registerSet.put(name, value);
}

MemoryValue
Project::setRegisterValue(const std::string &name, const MemoryValue &value) {
  return _registerSet.set(name, value);
}

UnitContainer Project::getRegisterUnits() const {
  return _architecture.getUnits();
}

/*https://github.com/TUM-LRR/era-gp-sim/issues/83
Architecture::byte_size_t Project::getByteSize() const {
  return _architecture.getByteSize();
}*/

std::size_t Project::getMemorySize() const {
  return _memory.getByteCount();
}

void Project::setMemorySize(std::size_t size) {
}

InstructionSet Project::getInstructionSet() const {
  return _architecture.getInstructions();
}

void Project::resetMemory() {
  MemoryValue zero = conversions::convert(0, _memory.getByteSize());
  for (std::size_t i = 0; i < getMemorySize(); i++) {
    putMemoryValue(i, zero);
  }
}

void Project::resetRegisters() {
  for (UnitInformation unitInfo : _architecture.getUnits()) {
    for (auto &&registerPair : unitInfo) {
      RegisterInformation registerInfo = registerPair.second;
      if (!registerInfo.isConstant() && !registerInfo.hasEnclosing()) {
        MemoryValue zero = conversions::convert(0, registerInfo.getSize());
        putRegisterValue(registerPair.second.getName(), zero);
      }
    }
  }
}

std::function<std::string(MemoryValue)>
Project::getSignedDecimalConversion() const {
  return std::function<std::string(MemoryValue)>();
}

std::function<std::string(MemoryValue)>
Project::getUnsignedDecimalConversion() const {
  return std::function<std::string(MemoryValue)>();
}

std::function<std::string(MemoryValue)> Project::getFloatConversion() const {
  return std::function<std::string(MemoryValue)>();
}

std::function<MemoryValue(std::string)>
Project::getSignedToMemoryValue() const {
  return std::function<MemoryValue(std::string)>();
}

std::function<MemoryValue(std::string)>
Project::getUnsignedToMemoryValue() const {
  return std::function<MemoryValue(std::string)>();
}

std::function<MemoryValue(std::string)> Project::getFloatToMemoryValue() const {
  return std::function<MemoryValue(std::string)>();
}

void Project::setUpdateRegisterCallback(
    std::function<void(const std::string &)> callback) {
  _registerSet.setCallback(callback);
}

void Project::setUpdateMemoryCallback(
    std::function<void(std::size_t, std::size_t)> callback) {
  _memory.setCallback(callback);
}

Architecture Project::getArchitecture() {
  return _architecture;
}
