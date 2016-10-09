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
#include "arch/common/register-information.hpp"
#include "arch/common/unit-information.hpp"
#include "common/assert.hpp"
#include "parser/parser-factory.hpp"
#include "parser/parser-mode.hpp"

ParsingAndExecutionUnit::ParsingAndExecutionUnit(
    std::weak_ptr<Scheduler>&& scheduler,
    MemoryAccess memoryAccess,
    Architecture architecture,
    std::atomic_flag& stopFlag,
    std::string parserName)
: Servant(std::move(scheduler))
, _parser(ParserFactory::createParser(architecture, parserName))
, _stopFlag(stopFlag)
, _memoryAccess(memoryAccess)
, _breakpoints() {
  for (UnitInformation unitInfo : architecture.getUnits()) {
    if (unitInfo.hasSpecialRegister(
            RegisterInformation::Type::PROGRAM_COUNTER)) {
      _programCounterName =
          unitInfo
              .getSpecialRegister(RegisterInformation::Type::PROGRAM_COUNTER)
              .getName();
    }
  }
}

void ParsingAndExecutionUnit::execute() {
  if (_finalRepresentation.errorList.size() != 0) {
    // TODO
    assert::that(false);
  }
  _stopFlag.test_and_set();
  std::size_t nextNode = findNextNode();
  while (_stopFlag.test_and_set() &&
         nextNode < _finalRepresentation.commandList.size()) {
    nextNode = executeNextLine();
  }
}

std::size_t ParsingAndExecutionUnit::executeNextLine() {
  // reference to avoid copying a unique_ptr
  FinalCommand& currentCommand =
      _finalRepresentation.commandList.at(findNextNode());
  assert::that(currentCommand.node->validate());

  assert::that(_setCurrentLine);
  _setCurrentLine(currentCommand.position.lineStart);

  // currentCommand.node->getValue(_memoryAccess);

  std::size_t nextNode = findNextNode();
  _setCurrentLine(
      _finalRepresentation.commandList.at(nextNode).position.lineStart);
  return nextNode;
}

// current behaviour: only considers first breakpoint, if multiple breakpoints
// are in the same whitespace between two commands.
void ParsingAndExecutionUnit::executeToBreakpoint() {
  _stopFlag.test_and_set();
  int nextNode = findNextNode();
  int lastLine =
      _finalRepresentation.commandList.at(nextNode).position.lineStart;
  while (_stopFlag.test_and_set() &&
         nextNode < _finalRepresentation.commandList.size()) {
    nextNode                    = executeNextLine();
    auto nextBreakpointIterator = _breakpoints.lower_bound(lastLine);
    int currentLine =
        _finalRepresentation.commandList.at(nextNode).position.lineStart;
    if (nextBreakpointIterator != _breakpoints.end() &&
        *nextBreakpointIterator <= currentLine) {
      // we reached a breakpoint
      break;
    }
    lastLine = currentLine;
  }
}

void ParsingAndExecutionUnit::setExecutionPoint(int line) {
  // TODO handle macros
  for (auto&& command : _finalRepresentation.commandList) {
    if (command.position.lineStart >= line) {
      //_memoryAccess.putRegisterValue(_programCounterName,
      // convertToMemoryValue(command.address));
      break;
    }
  }
}

void ParsingAndExecutionUnit::parse(std::string code) {
  // delete old assembled program in memory
  for (auto&& command : _finalRepresentation.commandList) {
    _memoryAccess.putMemoryCell(command.address, MemoryValue());
  }
  _finalRepresentation = _parser->parse(code, ParserMode::COMPILE);
  _addressCommandMap   = _finalRepresentation.createMapping();
  // assemble commands into memory
  for (auto&& command : _finalRepresentation.commandList) {
    _memoryAccess.putMemoryCell(command.address, command.node->assemble());
  }
}

void ParsingAndExecutionUnit::setBreakpoint(int line) {
  _breakpoints.insert(line);
}

void ParsingAndExecutionUnit::deleteBreakpoint(int line) {
  _breakpoints.erase(line);
}

const SyntaxInformation::TokenIterable
ParsingAndExecutionUnit::getSyntaxRegex(SyntaxInformation::Token token) const {
  return _parser->getSyntaxInformation().getSyntaxRegex(token);
}

void ParsingAndExecutionUnit::setSetContextInformationCallback(
    std::function<void(int, int, int, std::string)> callback) {
  _setContextInformation = callback;
}

void ParsingAndExecutionUnit::setSetErrorListCallback(
    std::function<void(std::vector<CompileError>&&)> callback) {
  _setErrorList = callback;
}

/*void ParsingAndExecutionUnit::setSetMacroListCallback(
    std::function<void(std::vector)> callback) {
    _setMacroList = callback;
}*/

void ParsingAndExecutionUnit::setSetCurrentLineCallback(
    std::function<void(int)> callback) {
  _setCurrentLine = callback;
}

std::size_t ParsingAndExecutionUnit::findNextNode() const {
  // std::size_t nextInstructionAddress =
  // convert(_memoryAccess.getRegisterValue(_programCounterName));
  // return _addressCommandMap.find(nextInstructionAddress);
  return 0;
}
