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

ParsingAndExecutionUnit::ParsingAndExecutionUnit(std::weak_ptr<Scheduler> &&scheduler, MemoryAccess memoryAccess) : Servant(std::move(scheduler)), _memoryAccess(memoryAccess) {

}

void ParsingAndExecutionUnit::execute() {

}

void ParsingAndExecutionUnit::executeNextLine() {

}

void ParsingAndExecutionUnit::executeToBreakpoint() {

}

void ParsingAndExecutionUnit::setExecutionPoint(int line) {

}

void ParsingAndExecutionUnit::parse(std::string code) {

}

void ParsingAndExecutionUnit::setBreakpoint(int line) {

}

std::string ParsingAndExecutionUnit::getSyntaxRegister(std::string name) {
    return std::string();
}

std::string ParsingAndExecutionUnit::getSyntaxInstruction(std::string name) {
    return std::string();
}

std::string ParsingAndExecutionUnit::getSyntaxImmediate() {
    return std::string();
}

std::string ParsingAndExecutionUnit::getSyntaxComment() {
    return std::string();
}

std::string ParsingAndExecutionUnit::getSyntaxLabel() {
    return std::string();
}

void ParsingAndExecutionUnit::setSetContextInformationCallback(
    std::function<void(int, int, int, std::string)> callback) {
    _setContextInformation = callback;
}

void ParsingAndExecutionUnit::setSetErrorListCallback(
    std::function<void(std::vector<CompileError> &&)> callback) {
  _setErrorList = callback;
}

/*void ParsingAndExecutionUnit::setSetMacroListCallback(
    std::function<void(std::vector)> callback) {
    _setMacroList = callback;
}*/

void ParsingAndExecutionUnit::setSetCurrentLineCallback(std::function<void(int)> callback) {
  _setCurrentLine = callback;
}
