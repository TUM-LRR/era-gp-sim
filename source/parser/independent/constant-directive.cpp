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

#include "parser/independent/constant-directive.hpp"

#include <string>

#include "parser/common/compile-error-list.hpp"
#include "parser/independent/enhance-symbol-table-immutable-arguments.hpp"
#include "parser/independent/execute-immutable-arguments.hpp"
#include "parser/independent/symbol-graph.hpp"
#include "parser/independent/symbol-replacer.hpp"
#include "parser/independent/syntax-tree-generator.hpp"

ConstantDirective::ConstantDirective(
    const CodePositionInterval& positionInterval,
    const PositionedStringVector& labels,
    const PositionedString& name,
    const PositionedStringVector& arguments)
: IntermediateDirective(positionInterval, labels, name), _arguments{arguments} {
}

void ConstantDirective::execute(const ExecuteImmutableArguments& immutable,
                                CompileErrorList& errors,
                                FinalCommandVector& commandOutput,
                                MemoryAccess& memoryAccess) {
  // Try to parse argument to catch errors early.
  immutable.generator().transformOperand(
      _expression, immutable.replacer(), errors);
}

void ConstantDirective::precompile(
    const PrecompileImmutableArguments& immutable,
    CompileErrorList& errors,
    SymbolGraph& graph,
    MacroDirectiveTable& macroTable) {
  // better error messages:
  //  0 arguments -> this argument should be the name
  //  1 argument  -> this argument should be the value
  //  2 arguments -> ok
  // >2 arguments -> too many
  switch (_arguments.size()) {
    case 0:
      errors.pushError(name().positionInterval(), "Missing constant name.");
      return;
    case 1:
      errors.pushError(
          name().positionInterval().unite(_arguments[0].positionInterval()),
          "Missing constant value.");
      return;
    case 2:
      // Everything ok, break.
      break;
    default:
      errors.pushError(
          name().positionInterval(),
          "Malformed constant directive, too many operands provided.");
      return;
  }

  // We embrace the expression with brackets, so it causes no conflicts when we
  // insert it into the token stream/string.
  _expression = PositionedString("(" + _arguments[1].string() + ")",
                                 _arguments[1].positionInterval());
  graph.addNode(Symbol(_arguments[0], _expression));
}

const PositionedStringVector& ConstantDirective::arguments() const noexcept {
  return _arguments;
}
const PositionedString& ConstantDirective::expression() const noexcept {
  return _expression;
}
