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

#include "core/project-module.hpp"

ProjectModule::ProjectModule(ArchitectureFormula&& architectureFormula,
                             std::size_t memorySize,
                             const std::string& parserName)
: _schedulerProject(std::make_shared<Scheduler>())
, _schedulerParsingAndExecution(std::make_shared<Scheduler>())
, _proxyProject(
      std::move(_schedulerProject), std::move(architectureFormula), memorySize)
, _memoryAccess(_proxyProject)
, _memoryManager(_proxyProject)
, _architectureAccess(_proxyProject)
, _stopFlag(ATOMIC_FLAG_INIT)
, _proxyParsingAndExecution(std::move(_schedulerParsingAndExecution),
                            _memoryAccess,
                            _architectureAccess.getArchitecture().get(),
                            std::ref(_stopFlag),
                            parserName)
, _commandInterface(_proxyParsingAndExecution)
, _parserInterface(_proxyParsingAndExecution) {
}


const MemoryAccess& ProjectModule::getMemoryAccess() const {
  return _memoryAccess;
}

const MemoryManager& ProjectModule::getMemoryManager() const {
  return _memoryManager;
}

const ArchitectureAccess& ProjectModule::getArchitectureAccess() const {
  return _architectureAccess;
}

const CommandInterface& ProjectModule::getCommandInterface() const {
  return _commandInterface;
}

const ParserInterface& ProjectModule::getParserInterface() const {
  return _parserInterface;
}

void ProjectModule::reset() {
  _memoryManager.resetMemory();
  _memoryManager.resetRegisters();
  // set execution point to line 0
}

void ProjectModule::stopExecution() {
  _stopFlag.clear();
}
