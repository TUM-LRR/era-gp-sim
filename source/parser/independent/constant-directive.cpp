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

#include <string>

#include "parser/common/compile-error-list.hpp"
#include "parser/independent/constant-directive.hpp"
#include "parser/independent/intermediate-parameters.hpp"
#include "parser/independent/symbol-graph.hpp"
#include "parser/independent/symbol-replacer.hpp"
#include "parser/independent/syntax-tree-generator.hpp"

ConstantDirective::ConstantDirective(
    const CodePositionInterval& positionInterval,
    const std::vector<PositionedString>& labels,
    const PositionedString& name,
    const std::vector<PositionedString>& arguments)
: IntermediateDirective(positionInterval, labels, name), _arguments{arguments} {
}

void ConstantDirective::execute(const ExecuteImmutableArguments& immutable,
                                CompileErrorList& errors,
                                FinalCommandVector& commandOutput,
                                MemoryAccess& memoryAccess) {
  // Try to parse argument to catch errors early.
  auto fullExpression = immutable.replacer().replace(_expression, errors);
  if (!fullExpression.string().empty()) {
    immutable.generator().transformOperand(
        _expression, immutable.replacer(), errors);
  } else {
    // better error messages:
    // 0 arguments -> this argument should be the name
    // 1 argument -> this argument should be the value
    //>1 arguments -> too many
    switch (_arguments.size()) {
      case 0:
        errors.pushError(name().positionInterval(), "Missing constant name.");
        break;
      case 1:
        errors.pushError(
            name().positionInterval().unite(_arguments[0].positionInterval()),
            "Missing constant value.");
        break;
      default:
        errors.pushError(
            name().positionInterval(),
            "Malformed constant directive, too many operands provided.");
        break;
    }
  }
}

void ConstantDirective::enhanceSymbolTable(
    const EnhanceSymbolTableImmutableArguments& immutable,
    CompileErrorList& errors,
    SymbolGraph& graph) {
  if (_arguments.size() != 2) {
    errors.pushError(name().positionInterval(), "Malformed constant directive");
    return;
  }
  _expression = PositionedString("(" + _arguments[1].string() + ")",
                                 _arguments[1].positionInterval());
  graph.addNode(Symbol(_arguments[0], _expression));
}
