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

#include "parser/constant-directive.hpp"

void ConstantDirective::execute(FinalRepresentation& finalRepresentator,
                                const SymbolTable& table,
                                const SyntaxTreeGenerator& generator,
                                CompileState& state, MemoryAccess &memoryAccess) {
  // Try to parse argument to catch errors early.
  std::string fullExpression = table.replaceSymbols(expression, state);
  generator.transformOperand(fullExpression, state);
}

void ConstantDirective::enhanceSymbolTable(SymbolTable& table, const MemoryAllocator &allocator,
                                           CompileState& state) {
  if (_arguments.size() != 2) {
    state.addError("Malformed constant directive", state.position);
    return;
  }
  expression = "(" + _arguments[1] + ")";
  table.insertEntry(_arguments[0], expression, state);
}
