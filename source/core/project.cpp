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

#include "arch/common/register-information.hpp"
#include "arch/common/unit-information.hpp"
#include "common/assert.hpp"
#include "core/deserialization-error.hpp"

Project::Project(std::weak_ptr<Scheduler> &&scheduler,
                 const ArchitectureFormula &architectureFormula,
                 size_t memorySize)
: Servant(std::move(scheduler))
, _architecture(Architecture::Brew(architectureFormula))
, _memory(memorySize, _architecture.getByteSize())
, _registerSet()
, _architectureFormula(architectureFormula)
, _errorCallback(
      [](const std::string &s, const std::vector<std::string> &v) {}) {
  _architecture.validate();

  for (UnitInformation unitInfo : _architecture.getUnits()) {
    for (const auto &registerPair : unitInfo) {
      // create all top level registers
      _createRegister(registerPair.second, unitInfo);
    }
    // create special registers
    for (const auto &registerPair : unitInfo.getSpecialRegisters()) {
      _createRegister(registerPair.second, unitInfo);
    }
  }
}

void Project::_createRegister(RegisterInformation registerInfo,
                              UnitInformation unitInfo) {
  if (!registerInfo.hasEnclosing()) {
    MemoryValue startValue{registerInfo.getSize()};
    // if this register is hardwired to some constant
    if (registerInfo.isConstant()) {
      startValue = registerInfo.getConstant();
    }
    _registerSet.createRegister(
        registerInfo.getName(), startValue, registerInfo.isConstant());
    _registerSet.aliasRegister(
        registerInfo.getAliases(), registerInfo.getName(), 0, true);
    // create all constituents and their constituents
    _createConstituents(registerInfo, unitInfo);
  }
}

void Project::_createConstituents(RegisterInformation enclosingRegister,
                                  UnitInformation unitInfo) {
  for (const auto &constituentInformation :
       enclosingRegister.getConstituents()) {
    // find RegisterInformation of the constituent
    RegisterInformation constituentRegisterInfo =
        unitInfo.getRegister(constituentInformation.getID());

    // createRegister/alias
    // clang-format off
    auto startOffset = constituentInformation.getEnclosingOffset();
    auto endOffset = startOffset + constituentRegisterInfo.getSize();
    _registerSet.aliasRegister(
      constituentRegisterInfo.getName(),
      enclosingRegister.getName(),
      startOffset,
      endOffset,
      false
    );

    _registerSet.aliasRegister(
      constituentRegisterInfo.getAliases(),
      constituentRegisterInfo.getName(),
      0,
      true
    );
    // clang-format on

    // recursive call to create constituents of this constituent
    if (constituentRegisterInfo.hasConstituents()) {
      _createConstituents(constituentRegisterInfo, unitInfo);
    }
  }
}

MemoryValue Project::getMemoryValueAt(size_t address, size_t amount) const {
  return _memory.get(address, amount);
}

void Project::putMemoryValueAt(size_t address, const MemoryValue &value) {
  _memory.put(address, value);
}

MemoryValue
Project::setMemoryValueAt(size_t address, const MemoryValue &value) {
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

Architecture::byte_size_t Project::getByteSize() const {
  return _architecture.getByteSize();
}

size_t Project::getMemorySize() const {
  return _memory.getByteCount();
}

void Project::setMemorySize(size_t size) {
}

InstructionSet Project::getInstructionSet() const {
  return _architecture.getInstructions();
}

void Project::resetMemory() {
  MemoryValue zero(_memory.getByteSize() * _memory.getByteCount());
  putMemoryValueAt(0, zero);
}

void Project::resetRegisters() {
  for (UnitInformation unitInfo : _architecture.getUnits()) {
    // set the normal registers to zero
    for (const auto &registerPair : unitInfo) {
      _setRegisterToZero(registerPair.second);
    }
    // set the special registers to zero
    for (const auto &registerPair : unitInfo.getSpecialRegisters()) {
      _setRegisterToZero(registerPair.second);
    }
  }
}

void Project::loadSnapshot(Json snapshot) {
  bool valid = true;
  if (!(snapshot.count("architecture-name") == 1)) valid = false;
  if (!(snapshot.count("extensions") == 1)) valid = false;
  if (!(snapshot.count("memory") == 1)) valid = false;
  if (!(snapshot.count("registers") == 1)) valid = false;
  if (snapshot["extensions"].empty()) valid = false;
  if (!valid) {
    _errorCallback("Snapshot format is not valid", std::vector<std::string>());
    return;
  }
  try {
    std::string architectureName = snapshot["architecture-name"];
    std::vector<std::string> extensions = snapshot["extensions"];
    ArchitectureFormula formula(architectureName);
    for (const std::string &extension : extensions) {
      formula.addExtension(extension);
    }
    if (formula != _architectureFormula) {
      _errorCallback("This snapshot was created with a different architecture!",
                     std::vector<std::string>());
    }
    _memory.deserializeJSON(snapshot["memory"]);
    _registerSet.deserializeJSON(snapshot["registers"]);
  } catch (const DeserializationError &exception) {
    _errorCallback(exception.what(), std::vector<std::string>());
  }
}

Project::Json Project::generateSnapshot() const {
  Json snapshot;
  snapshot["architecture-name"] = _architectureFormula.getArchitectureName();
  snapshot["extensions"] = _architectureFormula.getUnderlying();
  snapshot["memory"] = _memory.serializeJSON();
  snapshot["registers"] = _registerSet.serializeJSON();
  return snapshot;
}

void Project::_setRegisterToZero(RegisterInformation registerInfo) {
  if (!registerInfo.isConstant() && !registerInfo.hasEnclosing()) {
    // create a empty MemoryValue as long as the register
    MemoryValue zero(registerInfo.getSize());
    putRegisterValue(registerInfo.getName(), zero);
  }
}

Project::MemoryValueToString Project::getSignedDecimalConversion() const {
  return MemoryValueToString();
}

Project::MemoryValueToString Project::getUnsignedDecimalConversion() const {
  return MemoryValueToString();
}

Project::MemoryValueToString Project::getFloatConversion() const {
  return MemoryValueToString();
}

Project::StringToMemoryValue Project::getSignedToMemoryValue() const {
  return StringToMemoryValue();
}

Project::StringToMemoryValue Project::getUnsignedToMemoryValue() const {
  return StringToMemoryValue();
}

Project::StringToMemoryValue Project::getFloatToMemoryValue() const {
  return StringToMemoryValue();
}

void Project::setUpdateRegisterCallback(
    Callback<const std::string &> callback) {
  _registerSet.setCallback(callback);
}

void Project::setUpdateMemoryCallback(Callback<size_t, size_t> callback) {
  _memory.setCallback(callback);
}

void Project::setErrorCallback(
    Callback<const std::string &, const std::vector<std::string> &> callback) {
  _errorCallback = callback;
}

Architecture Project::getArchitecture() const {
  return _architecture;
}
