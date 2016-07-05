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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.*/

#include "include/parser/IntermediateInstruction.hpp"

void IntermediateInstruction::execute(FinalRepresentation& finalRepresentator,
																			const SymbolTable& table,
																			CompileState& state) {
	// For a machine instruction, it is easy to "execute" it: just insert it into
	// the final form.
	finalRepresentator.push_back(compileInstruction(table, state));
}

std::unique_ptr<AbstractSyntaxTreeNode>
IntermediateInstruction::compileInstruction(const SymbolTable& table,
																						CompileState& state) {
	// We replace all occurenced in target in source (using a copy of them).
	std::vector<std::string> src(_sources);
	std::vector<std::string> trg(_targets);

	table.replaceSymbols(src, state);
	table.replaceSymbols(trg, state);

	// More to do here (soon^TM).
	return std::make_unique<AbstractSyntaxTreeNode>();
}

void IntermediateInstruction::determineMemoryPosition() {
	// To be expanded soon^TM.
}
