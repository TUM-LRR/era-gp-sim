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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_CONSTANT_DIRECTIVE_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_CONSTANT_DIRECTIVE_HPP

#include "parser/independent/enhance-symbol-table-immutable-arguments.hpp"
#include "parser/independent/execute-immutable-arguments.hpp"
#include "parser/independent/intermediate-directive.hpp"

/**
 * Represents a constant directive in code.
 */
class ConstantDirective : public IntermediateDirective {
 public:
  /**
   * Creates a new constant directive with the given parameters.
   *
   * \param positionInterval The position in code where the constant directive
   * is located.
   * \param labels The labels associated with this directive.
   * \param name The name of the directive.
   * \param arguments The given arguments of this constant directive.
   */
  ConstantDirective(const CodePositionInterval& positionInterval,
                    const PositionedStringVector& labels,
                    const PositionedString& name,
                    const PositionedStringVector& arguments);

  /**
   * Executes the constant directive, i.e. checks its validity.
   *
   * \param immutable Some constant arguments which might be helpful.
   * \param errors The compile error list to note down any errors.
   * \param commandOutput The final command output vector to record all
   * finalized commands.
   * \param memoryAccess The memory access used to reserve memory and validate
   * instructions.
   */
  virtual void execute(const ExecuteImmutableArguments& immutable,
                       CompileErrorList& errors,
                       FinalCommandVector& commandOutput,
                       MemoryAccess& memoryAccess);

  /**
* Inserts the constant into the symbol table.
*
* \param immutable Some constant arguments which might be helpful.
* \param errors The compile error list to note down any errors.
* \param macroTable A table to record occured macros.
*/
  virtual void precompile(const PrecompileImmutableArguments& immutable,
                          CompileErrorList& errors,
                          SymbolGraph& graph,
                          MacroDirectiveTable& macroTable);

  /**
   * \return The arguments of the constant directive.
   */
  const PositionedStringVector& arguments() const noexcept;

  /**
   * \return The expression which this constant directive represents.
   */
  const PositionedString& expression() const noexcept;

 private:
  /**
   * The argument list received by this constant directive.
   */
  PositionedStringVector _arguments;

  /**
   * A positioned string representing the expression the constant will be
   * replaced with.
   */
  PositionedString _expression;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_CONSTANT_DIRECTIVE_HPP */
