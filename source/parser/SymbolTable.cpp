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

#include "include/parser/SymbolTable.hpp"

#include <cassert>

const std::regex SymbolTable::symbolNameFormat("^[A-Za-z_][A-Za-z0-9_]*$",
																							 std::regex::ECMAScript);

inline std::regex SymbolTable::makeRegex(const std::string name) const {
	// Just searches for the word.
	return std::regex("\\b" + name + "\\b", std::regex::ECMAScript);
}

inline void SymbolTable::clearTable() {
	// Simply clear everything.
	_table.clear();
}

void SymbolTable::insertEntry(const std::string& name,
															const std::string& replacement,
															CompileState& state) {
	// Expects a trimmed string.
	// To explain the regex: We just look if there are spaces at the beginning OR
	// at the end of the string.

	assert(!std::regex_search(
			name, std::regex("(^\\s+|\\s+$)", std::regex::ECMAScript)));

	// First of all, we check for errors with our new symbol.

	if (!std::regex_search(name, symbolNameFormat)) {
		// Basically, everything with a leading number is not accepted.
		state.errorList.push_back(
				CompileError("Symbol '" + name + "' does not have a qualified name.",
										 state.position,
										 CompileErrorSeverity::kError));
		return;
	}

	if (_table.find(name) != _table.end()) {
		// We also fail, if we define the symbol twice in a commit (which would
		// count as double definition in a file).
		state.errorList.push_back(
				CompileError("Symbol '" + name + "' defined twice.",
										 state.position,
										 CompileErrorSeverity::kError));
		return;
	}

	// All conditions checked insert!
	_table[name] = replacement;
}

std::string SymbolTable::replaceSymbols(const std::string& source,
																				CompileState& state) const {
	std::string result = source;

	// Just make a copy and iterate over all defined symbols. If they match,
	// replace them.
	for (int i = 0; i < _maximumRecursionDepth; ++i) {
		bool cont = false;// Actually, we could store all last results and check, if
											// one has already occured. Or just for one round... We
											// just have do differ from infinite recursion where one
											// pass gets us the same string again...
		for (const auto& i : _table) {
			// Replace all symbol occurrences.
			std::string newResult =
					std::regex_replace(result, makeRegex(i.first), i.second);
			if (!cont && newResult != result) {
				cont = true;
			}
			result = newResult;
		}

		if (!cont) {
			// Abort, if we have found no more to replace.
			return result;
		}
	}

	// If we come here, we have replaced too often and abort, suspecting an
	// infinite loop.
	state.errorList.push_back(
			CompileError("Exceeded recursion replacement depth.",
									 state.position,
									 CompileErrorSeverity::kError));

	return result;
}


void SymbolTable::replaceSymbols(std::vector<std::string>& source,
																 CompileState& state) const {
	// We just replace the symbols for each of the vector elements.

	for (int i = 0; i < source.size(); i++) {
		source[i] = replaceSymbols(source[i], state);
	}
}
