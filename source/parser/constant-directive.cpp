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

#include "parser/constant-directive.hpp"
#include "parser/symbol-table.hpp"
#include "parser/syntax-tree-generator.hpp"

void ConstantDirective::execute(FinalRepresentation& finalRepresentator,
                                const SymbolTable& table,
                                const SyntaxTreeGenerator& generator,
                                CompileState& state,
                                MemoryAccess& memoryAccess) {
  // Try to parse argument to catch errors early.
  std::string fullExpression = table.replaceSymbols(expression, state);
  if (!fullExpression.empty()) {
    generator.transformOperand(fullExpression, state);
  } else {
    // better error messages:
    // 0 arguments -> this argument should be the name
    // 1 argument -> this argument should be the value
    //>1 arguments -> too many
    switch (_arguments.size()) {
      case 0: state.addError("Missing constant name", state.position); break;
      case 1: state.addError("Missing constant value", state.position); break;
      default:
        state.addError(
            "Malformed constant directive, too many operands provided",
            state.position);
        break;
    }
  }
}

void ConstantDirective::enhanceSymbolTable(SymbolTable& table,
                                           const MemoryAllocator& allocator,
                                           CompileState& state) {
  if (_arguments.size() != 2) {
    state.addError("Malformed constant directive", state.position);
    return;
  }
  expression = "(" + _arguments[1] + ")";
  table.insertEntry(_arguments[0], expression, state);
}
