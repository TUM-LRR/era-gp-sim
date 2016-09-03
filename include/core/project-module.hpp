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

#ifndef ERAGPSIM_CORE_PROJECT_MODULE_HPP
#define ERAGPSIM_CORE_PROJECT_MODULE_HPP

#include <memory>

#include "arch/common/architecture-formula.hpp"
#include "core/architecture-access.hpp"
#include "core/callback-manager.hpp"
#include "core/memory-access.hpp"
#include "core/project.hpp"
#include "core/proxy.hpp"
#include "core/register-access.hpp"
#include "core/scheduler.hpp"

/**
 * This class encapsulates all proxy components to the project servant, as well
 * as the proxy for the parsing and execution unit. It is supposed to live in
 * the ui-thread, as projects are created there.
 *
 */
class ProjectModule {
 public:
  ProjectModule(ArchitectureFormula &&architectureFormula, int memorySize);

  /**
   * Returns the MemoryAccess proxy.
   *
   */
  MemoryAccess getMemoryAccess();

  /**
   * Returns the RegisterAccess proxy.
   *
   */
  RegisterAccess getRegisterAccess();

  /**
   * Returns the CallbackManager proxy.
   *
   */
  CallbackManager getCallbackManager();

  /**
   * Returns the ArchitectureAccess proxy.
   *
   */
  ArchitectureAccess getArchitectureAccess();

  /**
   * Resets the state of registers, memory and the execution point
   *
   */
  void reset();


 private:
  /** Scheduler for the project servant (active-object). */
  std::shared_ptr<Scheduler> _schedulerProject;

  /** Scheduler for the parsing and execution servant, will be used later. */
  std::shared_ptr<Scheduler> _schedulerParsingAndExecution;

  /** Proxy object used to create project servant and initialize the other
   * proxies. */
  Proxy<Project> _proxy;

  /** Proxy to access the memory component. */
  MemoryAccess _memoryAccess;

  /** Proxy to access the register component. */
  RegisterAccess _registerAccess;

  /** Proxy to set the ui callbacks. */
  CallbackManager _callbackManager;

  /** Proxy to access the architecture. */
  ArchitectureAccess _architectureAccess;

  /** ParseAndExecution proxy, will be added later. */
};

#endif /* ERAGPSIM_CORE_PROJECT_MODULE_HPP */
