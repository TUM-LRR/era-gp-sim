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


Project::Project(ArchitectureFormula &&architectureFormula, int memorySize)
: _architecture(Architecture::Brew(std::move(architectureFormula)))
, _registerSet() {
  _architecture.validate();
}

MemoryValue Project::getMemory(int address, int length) const {
  return MemoryValue();
}

void Project::putMemoryCell(int address, const MemoryValue &value) {
}

MemoryValue Project::setMemoryCell(int address, const MemoryValue &value) {
  return MemoryValue();
}


MemoryValue Project::getRegisterValue(const std::string &name) const {
  return _registerSet.get(name);
}

MemoryValue Project::getRegisterValue(const std::string &name,
                                      const std::size_t byteSize) const {
  return _registerSet.get(name, byteSize);
}

MemoryValue
Project::getRegisterValue(const std::string &name, MemoryValue &&out) const {
  return _registerSet.get(name, std::move(out));
}

void Project::putRegisterValue(const std::string &name,
                               const MemoryValue &value) {
  _registerSet.put(name, value);
}

MemoryValue
Project::setRegisterValue(const std::string &name, const MemoryValue &value) {
  return _registerSet.set(name, value);
}

MemoryValue
Project::setRegisterValue(const std::string &name, MemoryValue &&value) {
  return _registerSet.set(name, std::move(value));
}

RegisterContainer Project::getRegisterSet() const {
  return RegisterContainer();
}

int Project::getByteSize() const {
  return 8;
}

int Project::getMemorySize() const {
  return 0;
}

void Project::setMemorySize(int size) {
}

InstructionSet Project::getInstructionSet() const {
  return _architecture.getInstructions();
}

void Project::setBreakpoint(int line) {
}

void Project::parse(std::string code) {
}

void Project::update(std::string code) {
}

void Project::createSnapshot(std::string name) {
}

/*Snapshot Project::loadSnapshot(std::string name) {
}*/

void Project::deleteSnapshot(std::string name) {
}

void Project::execute() {
}

void Project::executeNextLine() {
}

void Project::executeToBreakpoint() {
}

void Project::stop() {
}

void Project::setExecutionPoint(int line) {
}

void Project::reset() {
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
    std::function<void(std::string)> callback) {
  _updateRegister = callback;
}

void Project::setUpdateRegistersCallback(
    std::function<void(std::vector<std::string> &&)> callback) {
  _updateRegisters = callback;
}

void Project::setUpdateMemoryCellCallback(std::function<void(int)> callback) {
  _updateMemoryCell = callback;
}

void Project::setSetContextInformationCallback(
    std::function<void(int, int, int, std::string)> callback) {
  _setContextInformation = callback;
}

void Project::setSetErrorListCallback(
    std::function<void(std::vector<CompileError> &&)> callback) {
  _setErrorList = callback;
}

/*void Project::setSetMacroListCallback(
    std::function<void(std::vector)> callback) {
    _setMacroList = callback;
}*/

void Project::setSetCurrentLineCallback(std::function<void(int)> callback) {
  _setCurrentLine = callback;
}
