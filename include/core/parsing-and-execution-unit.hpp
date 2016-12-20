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

#include <functional>
#include <memory>
#include <unordered_set>

#include "arch/common/register-information.hpp"
#include "arch/common/validation-result.hpp"
#include "core/memory-access.hpp"
#include "core/servant.hpp"
#include "parser/final-representation.hpp"
#include "parser/parser.hpp"
#include "parser/syntax-information.hpp"

class Architecture;
class ContextInformation;
class MacroInformation;

/**
 * This servant parses the code and executes the program.
 *
 */
class ParsingAndExecutionUnit : public Servant {
 public:
  using size_t = std::size_t;
  using SharedCondition = MemoryAccess::SharedCondition;
  template <typename... T>
  using Callback = std::function<void(T...)>;
  template <typename T>
  using ListCallback = std::function<void(const std::vector<T> &)>;

  /**
   * Creates a new ParsingAndExecutionUnit.
   *
   */
  ParsingAndExecutionUnit(std::weak_ptr<Scheduler> &&scheduler,
                          MemoryAccess memoryAccess,
                          Architecture architecture,
                          SharedCondition stopCondition,
                          std::string parserName);

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
  void setExecutionPoint(size_t line);

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
   * \returns true if the breakpoint could be set, false otherwise.
   */
  bool setBreakpoint(size_t line);

  /**
   * Deletes a breakpoint.
   *
   * \param line the line of the breakpoint which is deleted.
   *
   */
  void deleteBreakpoint(size_t line);

  /** Access to the SyntaxInformation interface of the parser.
   *
   * \param token The token to search for.
   * \return Iterable object which can be used in a for loop.
   *
   * \see SyntaxInformation
   */
  SyntaxInformation::TokenIterable
  getSyntaxRegex(SyntaxInformation::Token token) const;

  /**
   * Set the callback which is used to signal the gui that context information
   * for a memory cell was changed/added
   *
   * \param callback
   *
   */
  void
  setSetContextInformationCallback(ListCallback<ContextInformation> callback);

  // editor

  /**
   * Set the callback which is used to set the error list in the gui(editor)
   *
   * \param callback
   *
   */
  void setSetErrorListCallback(ListCallback<CompileError> callback);

  /**
   * Set the callback which is used to notify the gui of an error.
   *
   * \param callback
   */
  void setThrowErrorCallback(
      Callback<const std::string &, const std::vector<std::string> &> callback);

  /**
   * Set the callback to set the macro list in the ui.
   *
   * \param callback
   */
  void setSetMacroListCallback(ListCallback<MacroInformation> callback);

  /**
   * Set the callback which is used to inform the gui about the execution point
   * to highlight it in the editor
   *
   * \param callback
   *
   */
  void setSetCurrentLineCallback(Callback<size_t> callback);

  /**
   * Set the callback which signals the ui that the execution was stopped.
   *
   * \param callback
   */
  void setExecutionStoppedCallback(Callback<> callback);


 private:
  /**
   * Calculates the index of the next node according to the program counter.
   *
   * \return index of the next node.
   */
  size_t _findNextNode();

  /**
   * Executes a syntax tree node.
   *
   * \param nodeIndex the index of the syntax tree node
   */
  bool _executeNode(size_t nodeIndex);

  /**
   * Finds the next instruction and updates the line number in the ui.
   *
   * \param currentNode The index of the last executed node.
   * \return the index of the next node
   */
  size_t _updateLineNumber(size_t currentNode);

  /** A unique_ptr to the parser. */
  std::unique_ptr<Parser> _parser;

  /** Shared pointer to a ConditionTimer to stop the execution. */
  SharedCondition _stopCondition;

  /** A FinalRepresentation created by the parser. */
  FinalRepresentation _finalRepresentation;

  /** A mapping of address to command, has to be recreated every time the
   * FinalRepresentation changes */
  std::unordered_map<MemoryAddress, std::size_t> _addressCommandMap;

  /** A map to cache a line -> address mapping. */
  std::unordered_map<size_t, MemoryValue> _lineCommandCache;

  /** MemoryAccess proxy for the syntax trees.*/
  MemoryAccess _memoryAccess;

  /** set which contains the breakpoints.*/
  std::unordered_set<size_t> _breakpoints;

  /** The name of the program counter register. */
  RegisterInformation _programCounter;

  /** The SyntaxInformation object of the parser. */
  SyntaxInformation _syntaxInformation;

  /** Callback to set memory context information in the ui. */
  ListCallback<ContextInformation> _setContextInformation;

  /** Callback to set the error list in the ui.*/
  ListCallback<CompileError> _setErrorList;

  /** Callback to throw a runtime error. */
  Callback<const std::string &, const std::vector<std::string> &> _throwError;

  /** Callback to set the macro list in the ui.*/
  ListCallback<MacroInformation> _setMacroList;

  /** Callback to set the line which is executed in the ui.*/
  Callback<size_t> _setCurrentLine;

  /** Callback to tell the gui that the execution stopped. */
  Callback<> _executionStopped;
};

#endif /* ERAGPSIM_CORE_PARSING_AND_EXECUTION_UNIT_HPP */
