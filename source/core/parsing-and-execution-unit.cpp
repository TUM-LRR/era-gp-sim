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
, _setCurrentLine([](size_t x) {}) {
  // find the RegisterInformation object of the program counter
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
  size_t nextNode = _findNextNode();
  while (_stopFlag.test_and_set() &&
         nextNode < _finalRepresentation.commandList.size()) {
    nextNode = executeNextLine();
  }
}

size_t ParsingAndExecutionUnit::executeNextLine() {
  // check for parser errors
  if (_finalRepresentation.errorList.size() > 0) {
    return _findNextNode();
  }
  // reference to avoid copying a unique_ptr
  FinalCommand &currentCommand =
      _finalRepresentation.commandList.at(_findNextNode());
  // check for runtime errors
  ValidationResult validationResult =
      currentCommand.node->validateRuntime(_memoryAccess);
  if (!validationResult.isSuccess()) {
    // notify the ui of a runtime error
    _throwRuntimeError(validationResult.getMessage());
    return _findNextNode();
  }
  // update the current line in the ui (pre-execution)
  _setCurrentLine(currentCommand.position.lineStart);

  currentCommand.node->getValue(_memoryAccess);

  // find the next instruction and update the current line in the
  // ui (post-execution)
  size_t nextNode = _findNextNode();
  _setCurrentLine(
      _finalRepresentation.commandList.at(nextNode).position.lineStart);
  return nextNode;
}

void ParsingAndExecutionUnit::executeToBreakpoint() {
  // check if there are parser errors
  if (_finalRepresentation.errorList.size() > 0) {
    return;
  }
  // reset stop flag
  _stopFlag.test_and_set();
  // find the index of the next node and loop through the instructions
  size_t nextNode = _findNextNode();
  while (_stopFlag.test_and_set() &&
         nextNode < _finalRepresentation.commandList.size()) {
    nextNode = executeNextLine();
    // check if there is a brekpoint on the next line
    size_t nextLine =
        _finalRepresentation.commandList.at(nextNode).position.lineStart;
    if (_breakpoints.count(nextLine) > 0) {
      // we reached a breakpoint
      break;
    }
  }
}

void ParsingAndExecutionUnit::setExecutionPoint(size_t line) {
  // loop through all commands and see if any matches the given line
  for (auto &&command : _finalRepresentation.commandList) {
    if (command.position.lineStart == line) {
      // this command is on the given line, set the program counter to execute
      // this command next
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
  for (auto &&command : _finalRepresentation.commandList) {
    // create a empty MemoryValue as long as the command
    MemoryValue zero(command.node->assemble().getSize());
    _memoryAccess.putMemoryValueAt(command.address, zero);
  }
  // parse the new code and save the final representation
  _finalRepresentation = _parser->parse(code, ParserMode::COMPILE);
  _addressCommandMap = _finalRepresentation.createMapping();
  // update the error list of the ui
  _setErrorList(_finalRepresentation.errorList);
  // assemble commands into memory
  for (auto &&command : _finalRepresentation.commandList) {
    _memoryAccess.putMemoryValueAt(command.address, command.node->assemble());
  }
}

bool ParsingAndExecutionUnit::setBreakpoint(size_t line) {
  return _breakpoints.insert(line).second;
}

void ParsingAndExecutionUnit::deleteBreakpoint(size_t line) {
  _breakpoints.erase(line);
}

SyntaxInformation::TokenIterable
ParsingAndExecutionUnit::getSyntaxRegex(SyntaxInformation::Token token) const {
  return _syntaxInformation.getSyntaxRegex(token);
}

void ParsingAndExecutionUnit::setSetContextInformationCallback(
    ListCallback<ContextInformation> callback) {
  _setContextInformation = callback;
}

void ParsingAndExecutionUnit::setSetErrorListCallback(
    ListCallback<CompileError> callback) {
  _setErrorList = callback;
}

void ParsingAndExecutionUnit::setThrowRuntimeErrorCallback(
    Callback<const std::string &> callback) {
  _throwRuntimeError = callback;
}

void ParsingAndExecutionUnit::setSetMacroListCallback(
    ListCallback<MacroInformation> callback) {
  _setMacroList = callback;
}

void ParsingAndExecutionUnit::setSetCurrentLineCallback(
    Callback<size_t> callback) {
  _setCurrentLine = callback;
}

size_t ParsingAndExecutionUnit::_findNextNode() {
  // get the value of the program counter and convert it to a std::size_t
  std::string programCounterName = _programCounter.getName();
  MemoryValue programCounterValue =
      _memoryAccess.getRegisterValue(programCounterName).get();
  size_t nextInstructionAddress =
      conversions::convert<size_t>(programCounterValue);
  // find the instruction address from the program counter value
  auto iterator = _addressCommandMap.find(nextInstructionAddress);
  if (iterator == _addressCommandMap.end()) {
    // if there is no instruction (end of program reached), return a value that
    // stops the execution loop
    return _finalRepresentation.commandList.size();
  }
  return iterator->second;
}
