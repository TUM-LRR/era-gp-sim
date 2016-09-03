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

ProjectModule::ProjectModule(ArchitectureFormula &&architectureFormula,
                             int memorySize)
: _schedulerProject(std::make_shared<Scheduler>())
, _schedulerParsingAndExecution()
, _proxy(
      std::move(_schedulerProject), std::move(architectureFormula), memorySize)
, _memoryAccess(_proxy)
, _registerAccess(_proxy)
, _callbackManager(_proxy)
, _architectureAccess(_proxy) {
}


MemoryAccess ProjectModule::getMemoryAccess() {
  return _memoryAccess;
}

RegisterAccess ProjectModule::getRegisterAccess() {
  return _registerAccess;
}

CallbackManager ProjectModule::getCallbackManager() {
  return _callbackManager;
}

ArchitectureAccess ProjectModule::getArchitectureAccess() {
  return _architectureAccess;
}

void ProjectModule::reset() {
    _memoryAccess.resetMemory();
    _registerAccess.resetRegisters();
    //set execution point to line 0
}
