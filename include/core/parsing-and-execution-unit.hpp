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

#ifndef ERAGPSIM_CORE_PARSING_AND_EXECUTION_UNIT_HPP
#define ERAGPSIM_CORE_PARSING_AND_EXECUTION_UNIT_HPP

#include <atomic>

#include "arch/common/architecture.hpp"
#include "core/memory-access.hpp"
#include "core/servant.hpp"
#include "parser/final-representation.hpp"

/**
 * This servant parses the code and executes the program.
 *
 */
class ParsingAndExecutionUnit : public Servant {
 public:
  /**
   * Creates a new ParsingAndExecutionUnit.
   *
   */
  ParsingAndExecutionUnit(std::weak_ptr<Scheduler> &&scheduler,
                          MemoryAccess memoryAccess,
                          Architecture architecture,
                          std::atomic_flag &stopFlag);

  /**
   * Execute the whole assembler program
   *
   */
  void execute();

  /**
   * Execute the next line of the assembler program
   *
   */
  void executeNextLine();

  /**
   * Execute the assembler program to the next breakpoint
   *
   */
  void executeToBreakpoint();


  /**
   * Set the line which should be executed with any execute...() method
   *
   * \param line The line where the execution point is set
   *
   */
  void setExecutionPoint(int line);

  /**
   * Parses the given code
   *
   * \param code the std::string to parse
   *
   */
  void parse(std::string code);

  /**
   * Sets a breakpoint
   *
   * \param line the line at which the breakpoint is set
   *
   */
  void setBreakpoint(int line);


  /**
   * Creates dialect-specific Regex for syntax highlighting registers.
   *
   * \param name Register name
   * \return Dialect-specific Regex
   */
  std::string getSyntaxRegister(std::string name);

  /**
   * Creates dialect-specific Regex for syntax highlighting instructions.
   *
   * \param name Assembler instruction name
   * \return Dialect-specific Regex
   */
  std::string getSyntaxInstruction(std::string name);

  /**
   * Creates dialect-specific Regex for syntax highlighting immediates.
   *
   * \return Dialect-specific Regex
   */
  std::string getSyntaxImmediate();

  /**
   * Creates a dialect-specific Regex for syntax highlighting comments.
   *
   */
  std::string getSyntaxComment();

  /**
   * Creates a dialect-specific Regex for syntax highlighting labels.
   *
   */
  std::string getSyntaxLabel();

  /**
   * Set the callback which is used to signal the gui that context information
   * for a memory cell was changed/added
   *
   * \param callback
   *
   */
  void setSetContextInformationCallback(
      std::function<void(int, int, int, std::string)> callback);

  // editor

  /**
   * Set the callback which is used to set the error list in the gui(editor)
   *
   * \param callback
   *
   */
  void setSetErrorListCallback(
      std::function<void(std::vector<CompileError> &&)> callback);

  // void setSetMacroListCallback(std::function<void(std::vector)> callback);
  // TODO not yet know how macros are passed on by the parser

  /**
   * Set the callback which is used to inform the gui about the execution point
   * to highlight it in the editor
   *
   * \param callback
   *
   */
  void setSetCurrentLineCallback(std::function<void(int)> callback);


 private:
  /** The parser. TODO: How to decide which parser to use? */
  // Parser _parser

  /**  Reference to a std::atomic_flag to stop the execution. */
  std::atomic_flag &_stopFlag;

  /** The number of the current node of the execution. */
  int _currentNode;

  FinalRepresentation _finalRepresentation;

  /** MemoryAccess proxy for the syntax trees.*/
  MemoryAccess _memoryAccess;

  /** Callback to set memory context information in the ui. */
  std::function<void(int, int, int, std::string)> _setContextInformation;

  /** Callback to set the error list in the ui.*/
  std::function<void(std::vector<CompileError> &&)> _setErrorList;

  /** Callback to set the macro list in the ui.*/
  // std::function<void(std::vector)> _setMacroList;

  /** Callback to set the line which is executed in the ui.*/
  std::function<void(int)> _setCurrentLine;
};

#endif /* ERAGPSIM_CORE_PARSING_AND_EXECUTION_UNIT_HPP */
