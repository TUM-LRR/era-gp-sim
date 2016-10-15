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

#include "core/parsing-and-execution-unit.hpp"

#include "arch/common/architecture.hpp"
#include "arch/common/unit-information.hpp"
#include "arch/common/validation-result.hpp"
#include "common/assert.hpp"
#include "core/conversions.hpp"
#include "parser/parser-factory.hpp"
#include "parser/parser-mode.hpp"

ParsingAndExecutionUnit::ParsingAndExecutionUnit(
    std::weak_ptr<Scheduler> &&scheduler,
    MemoryAccess memoryAccess,
    Architecture architecture,
    std::atomic_flag &stopFlag,
    std::string parserName)
: Servant(std::move(scheduler))
, _parser(ParserFactory::createParser(architecture, parserName))
, _stopFlag(stopFlag)
, _finalRepresentation()
, _addressCommandMap()
, _memoryAccess(memoryAccess)
, _breakpoints()
, _syntaxInformation(_parser->getSyntaxInformation())
, _setContextInformation([](const std::vector<ContextInformation> &x) {})
, _setErrorList([](const std::vector<CompileError> &x) {})
, _throwRuntimeError(([](const std::string &x) {}))
, _setMacroList(([](const std::vector<MacroInformation> &x) {}))
, _setCurrentLine([](int x) {}) {
  for (UnitInformation unitInfo : architecture.getUnits()) {
    if (unitInfo.hasSpecialRegister(
            RegisterInformation::Type::PROGRAM_COUNTER)) {
      _programCounter = unitInfo.getSpecialRegister(
          RegisterInformation::Type::PROGRAM_COUNTER);
    }
  }
}

void ParsingAndExecutionUnit::execute() {
  if (_finalRepresentation.errorList.size() > 0) {
    return;
  }
  _stopFlag.test_and_set();
  std::size_t nextNode = _findNextNode();
  while (_stopFlag.test_and_set() &&
         nextNode < _finalRepresentation.commandList.size()) {
    nextNode = executeNextLine();
  }
}

std::size_t ParsingAndExecutionUnit::executeNextLine() {
  // reference to avoid copying a unique_ptr
  FinalCommand &currentCommand =
      _finalRepresentation.commandList.at(_findNextNode());
  if (_finalRepresentation.errorList.size() > 0) {
    return _findNextNode();
  }
  ValidationResult validationResult =
      currentCommand.node->validateRuntime(_memoryAccess);
  if (!validationResult.isSuccess()) {
    _throwRuntimeError(validationResult.getMessage());
    return _findNextNode();
  }

  _setCurrentLine(currentCommand.position.lineStart);

  currentCommand.node->getValue(_memoryAccess);

  std::size_t nextNode = _findNextNode();
  _setCurrentLine(
      _finalRepresentation.commandList.at(nextNode).position.lineStart);
  return nextNode;
}

void ParsingAndExecutionUnit::executeToBreakpoint() {
  if (_finalRepresentation.errorList.size() > 0) {
    return;
  }
  _stopFlag.test_and_set();
  int nextNode = _findNextNode();
  while (_stopFlag.test_and_set() &&
         nextNode < _finalRepresentation.commandList.size()) {
    nextNode = executeNextLine();
    int currentLine =
        _finalRepresentation.commandList.at(nextNode).position.lineStart;
    if (_breakpoints.count(currentLine) > 0) {
      // we reached a breakpoint
      break;
    }
  }
}

void ParsingAndExecutionUnit::setExecutionPoint(int line) {
  for (auto &&command : _finalRepresentation.commandList) {
    if (command.position.lineStart == line) {
      _memoryAccess.putRegisterValue(
          _programCounter.getName(),
          conversions::convert(command.address, _programCounter.getSize()));
      _setCurrentLine(line);
      break;
    }
  }
}

void ParsingAndExecutionUnit::parse(std::string code) {
  // delete old assembled program in memory
  // Disabled because AbstractSyntaxTreeNode::assemble() causes a segfault.
  // TODO AbstractSyntaxTreeNode::length() would be nice, otherwise every
  // command has to be assembled twice.
  /*for (auto&& command : _finalRepresentation.commandList) {
    MemoryValue zero = conversions::convert(0, command.assemble.getSize());
    _memoryAccess.putMemoryValue(command.address, zero;
  }*/
  _finalRepresentation = _parser->parse(code, ParserMode::COMPILE);
  _addressCommandMap = _finalRepresentation.createMapping();
  _setErrorList(_finalRepresentation.errorList);
  // assemble commands into memory
  /*for (auto&& command : _finalRepresentation.commandList) {
    _memoryAccess.putMemoryValue(command.address, command.node->assemble());
  }*/
}

bool ParsingAndExecutionUnit::setBreakpoint(int line) {
  return _breakpoints.insert(line).second;
}

void ParsingAndExecutionUnit::deleteBreakpoint(int line) {
  _breakpoints.erase(line);
}

SyntaxInformation::TokenIterable
ParsingAndExecutionUnit::getSyntaxRegex(SyntaxInformation::Token token) const {
  return _syntaxInformation.getSyntaxRegex(token);
}

void ParsingAndExecutionUnit::setSetContextInformationCallback(
    std::function<void(const std::vector<ContextInformation> &)> callback) {
  _setContextInformation = callback;
}

void ParsingAndExecutionUnit::setSetErrorListCallback(
    std::function<void(const std::vector<CompileError> &)> callback) {
  _setErrorList = callback;
}

void ParsingAndExecutionUnit::setThrowRuntimeErrorCallback(
    std::function<void(const std::string &)> callback) {
  _throwRuntimeError = callback;
}

void ParsingAndExecutionUnit::setSetMacroListCallback(
    std::function<void(const std::vector<MacroInformation> &)> callback) {
  _setMacroList = callback;
}

void ParsingAndExecutionUnit::setSetCurrentLineCallback(
    std::function<void(int)> callback) {
  _setCurrentLine = callback;
}

std::size_t ParsingAndExecutionUnit::_findNextNode() {
  std::string programCounterName = _programCounter.getName();
  MemoryValue programCounterValue =
      _memoryAccess.getRegisterValue(programCounterName).get();
  std::size_t nextInstructionAddress =
      conversions::convert<std::size_t>(programCounterValue);
  auto iterator = _addressCommandMap.find(nextInstructionAddress);
  if (iterator == _addressCommandMap.end()) {
    return _finalRepresentation.commandList.size();
  }
  return iterator->second;
}
