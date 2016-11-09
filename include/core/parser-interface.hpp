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

#ifndef ERAGPSIM_CORE_PARSER_INTERFACE_HPP
#define ERAGPSIM_CORE_PARSER_INTERFACE_HPP

#include "core/parsing-and-execution-unit.hpp"
#include "core/proxy.hpp"

/**
 * Proxy to access information of the parser and set relevant callbacks.
 *
 */
class ParserInterface : public Proxy<ParsingAndExecutionUnit> {
 public:
  ParserInterface(const Proxy<ParsingAndExecutionUnit>& proxy) : Proxy(proxy) {
  }

  /** Access to the SyntaxInformation interface of the parser.
   *
   * \param token The token to search for.
   * \return Iterable object which can be used in a for loop.
   *
   * \see SyntaxInformation
   */
  POST_FUTURE_CONST(getSyntaxRegex);

  /**
   * Set the callback which is used to signal the gui that context information
   * for a memory cell was changed/added
   *
   * \param callback std::function<void(const std::vector<ContextInformation>
   * &)>
   *
   */
  POST(setSetContextInformationCallback)

  /**
   * Set the callback which is used to set the error list in the gui(editor)
   *
   * \param callback std::function<void(const std::vector<CompileError> &)>
   *
   */
  POST(setSetErrorListCallback)

  /**
   * Set the callback which is used to notify the gui of a runtime error.
   *
   * \param callback std::function<void(const std::string&)> callback
   *
   */
  POST(setThrowRuntimeErrorCallback)

  /**
   * Set the callback which is used to inform the gui about the execution point
   * to highlight it in the editor
   *
   * \param callback std::function<void(int)>
   *
   */
  POST(setSetCurrentLineCallback)
};

#endif /* ERAGPSIM_CORE_PARSER_INTERFACE_HPP */
