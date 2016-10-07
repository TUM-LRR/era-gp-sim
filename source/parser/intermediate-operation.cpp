/* C++ Assembler Interpreter
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

#include "parser/intermediate-operation.hpp"

void IntermediateOperation::enhanceSymbolTable(SymbolTable& table,
                                               CompileState& state) {
  // We assign an address, if we do not have one yet (usually, we should not
  // have one).
  if (_address == NULL_ADDRESS) {
    determineMemoryPosition();
  }

  // Then, we insert all our labels.
  for (const auto& i : _labels) {
    table.insertEntry(i, std::to_string(_address), state);
  }
}
