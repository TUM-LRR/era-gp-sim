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

#ifndef ERAGPSIM_PARSER_SYMBOL_TABLE_HPP_
#define ERAGPSIM_PARSER_SYMBOL_TABLE_HPP_

#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>
#include "compile-state.hpp"

/**
 * \class SymbolTable
 * \brief Contains methods to manage symbols and replace them in strings. It
 * also checks for errors.
 */
class SymbolTable {
 public:
  /**
   * \fn SymbolTable::SymbolTable(int maximumRecursionDepth = 64)
   * \brief Instantiates an empty symbol table with the given recursion depth.
   * \param maximumRecursionDepth The given recursion depth, defaults to 64.
   */
  SymbolTable(int maximumRecursionDepth = 64)
  : _maximumRecursionDepth(maximumRecursionDepth) {
  }

  /**
   * \fn SymbolTable::insertEntry(const std::string& name, const std::string&
   * replacement, CompileState& state)
   * \brief Inserts an entry into the SymbolTable and checks for any errors.
   * \param name The symbol name.
   * \param replacement The symbol replacement.
   * \param state The compile state.
   */
  void insertEntry(const std::string& name,
                   const std::string& replacement,
                   CompileState& state);

  /**
   * \fn SymbolTable::clearTable()
   * \brief Clears the table.
   */
  void clearTable();

  /**
   * \fn SymbolTable::table() const
   * \brief Returns the internal symbol table.
   * \return The internal symbol table.
   */
  const std::map<std::string, std::string>& table() const {
    return _table;
  }

  /**
   * \fn SymbolTable::replaceSymbols(const std::string& source, CompileState&
   * state) const
   * \brief Replaces any symbols in the given string and records all occuring
   * errors.
   * \param source The source string.
   * \param state The compile state to log the errors.
   * \return The string with all symbols replaced (if the maximum recursion
   * depth has not been exceeded).
   */
  std::string
  replaceSymbols(const std::string& source, CompileState& state) const;

  /**
   * \fn SymbolTable::replaceSymbols(std::vector<std::string>& source,
   * CompileState& state) const
   * \brief Replaces any symbols in the given vector of strings and records all
   * occuring errors.
   * \param source The source vector.
   * \param state The compile state to log the errors.
   */
  void
  replaceSymbols(std::vector<std::string>& source, CompileState& state) const;

 private:
  /**
   * \fn SymbolTable::makeRegex(const std::string name) const
   * \brief Turns the name into a search regex for the distinct word.
   * \param name The name to transform.
   * \return The resulting regex.
   */
  std::regex makeRegex(const std::string name) const;

  /**
   * \fn SymbolTable::_table
   * \brief The internal map to store the symbol table.
   */
  std::map<std::string, std::string> _table;

  /**
   * \var SymbolTable::_maximumRecursionDepth
   * \brief Denotes the maximum passes through the symbol table before an
   * infinite recursion error.
   */
  int _maximumRecursionDepth;
};

#endif
