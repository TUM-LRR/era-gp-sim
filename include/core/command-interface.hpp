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

#ifndef ERAGPSIM_CORE_COMMAND_INTERFACE_HPP
#define ERAGPSIM_CORE_COMMAND_INTERFACE_HPP

#include "core/parsing-and-execution-unit.hpp"
#include "core/proxy.hpp"

/**
 * Proxy to invoke commands of the ParsingAndExecutionUnit servant, such as
 * execute() and parse().
 *
 */
class CommandInterface : public Proxy<ParsingAndExecutionUnit> {
 public:
  CommandInterface(const Proxy<ParsingAndExecutionUnit>& proxy) : Proxy(proxy) {
  }

  /**
   * Execute the whole assembler program
   *
   */
  POST(execute)

  /**
   * Execute the next line of the assembler program
   *
   */
  POST_FUTURE(executeNextLine)

  /**
   * Execute the assembler program to the next breakpoint
   *
   */
  POST(executeToBreakpoint)

  /**
   * Set the line which should be executed with any execute...() method
   *
   * \param line int The line where the execution point is set
   *
   */
  POST(setExecutionPoint)

  /**
   * Parses the given code
   *
   * \param code the std::string to parse
   *
   */
  POST(parse)

  /**
   * Sets a breakpoint
   *
   * \param line int the line at which the breakpoint is set
   *
   * \returns true if the breakpoint could be set, false otherwise.
   */
  POST_FUTURE(setBreakpoint)

  /**
   * Deletes a breakpoint.
   *
   * \param line the line of the breakpoint which is deleted.
   *
   */
  POST(deleteBreakpoint)
};

#endif /* ERAGPSIM_CORE_COMMAND_INTERFACE_HPP */
