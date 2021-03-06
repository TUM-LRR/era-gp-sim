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

#include "core/architecture-access.hpp"
#include "core/command-interface.hpp"
#include "core/memory-access.hpp"
#include "core/memory-manager.hpp"
#include "core/parser-interface.hpp"
#include "core/parsing-and-execution-unit.hpp"
#include "core/project.hpp"
#include "core/proxy.hpp"
#include "core/scheduler.hpp"

class ArchitectureFormula;

/**
 * This class encapsulates all proxy components to the project servant, as well
 * as the proxy for the parsing and execution unit. It is supposed to live in
 * the ui-thread, as projects are created there.
 *
 */
class ProjectModule {
 public:
  using SharedCondition = MemoryAccess::SharedCondition;
  template <typename... T>
  using Callback = ParsingAndExecutionUnit::Callback<T...>;

  ProjectModule(const ArchitectureFormula& architectureFormula,
                std::size_t memorySize,
                const std::string& parserName);

  /**
   * Returns the MemoryAccess proxy.
   *
   */
  MemoryAccess& getMemoryAccess();

  /**
   * Returns the MemoryManager proxy.
   *
   */
  MemoryManager& getMemoryManager();

  /**
   * Returns the ArchitectureAccess proxy.
   *
   */
  ArchitectureAccess& getArchitectureAccess();

  /**
   * Returns the CommandInterface proxy.
   *
   */
  CommandInterface& getCommandInterface();

  /**
   * Returns the ParserInterface proxy.
   *
   */
  ParserInterface& getParserInterface();

  /**
   * Resets the state of registers, memory and the execution point
   *
   */
  void reset();

  /**
   * Stops the execution after the current instruction.
   *
   */
  void stopExecution();

  /**
   * Notify the core that the ui has displayed the current state of the
   * execution.
   */
  void guiReady();

 private:
  /** This object encapsulates a condition variable with a stop flag and a
   * mutex. It is used to stop the execution on a user request.*/
  SharedCondition _stopCondition;

  /** This object is used to synchronize the core with the ui, because the ui is
   * often slower. */
  SharedCondition _syncCondition;

  /** Scheduler for the project servant (active-object). */
  std::shared_ptr<Scheduler> _schedulerProject;

  /** Scheduler for the parsing and execution servant, will be used later. */
  std::shared_ptr<Scheduler> _schedulerParsingAndExecution;

  /** Proxy object used to create project servant and initialize the other
   * proxies. */
  Proxy<Project> _proxyProject;

  /** Proxy to access the memory and registers. */
  MemoryAccess _memoryAccess;

  /** Proxy to manage the memory and registers. */
  MemoryManager _memoryManager;

  /** Proxy to access the architecture. */
  ArchitectureAccess _architectureAccess;

  /** Proxy object used to create the parser-and-execution servant and
   * initialize its proxies. */
  Proxy<ParsingAndExecutionUnit> _proxyParsingAndExecution;

  /** ParseAndExecution proxy, will be added later. */
  CommandInterface _commandInterface;

  /** Proxy to access information of the parser. */
  ParserInterface _parserInterface;
};

#endif /* ERAGPSIM_CORE_PROJECT_MODULE_HPP */
