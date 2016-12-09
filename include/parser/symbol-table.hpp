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

#ifndef ERAGPSIM_PARSER_SYMBOL_TABLE_HPP
#define ERAGPSIM_PARSER_SYMBOL_TABLE_HPP

#include <map>
#include <regex>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>
#include "common/multiregex.hpp"
#include "parser/code-position.hpp"
#include "parser/compile-state.hpp"

/**
 * \brief Contains methods to manage symbols and replace them in strings. It
 * also checks for errors.
 */
class SymbolTable {
 public:
  /**
 * \brief The SymbolType enum describes the type of symbol in this SymbolTable.
 * This distinction helps with a type-depended replacement.
 */
  enum class SymbolType { LABEL, CONSTANT, OTHER };
  enum class SymbolBehavior { STATIC, DYNAMIC };
  class TableEntry {
   public:
    friend SymbolTable;
    TableEntry(const std::string& name,
               const std::string& specifiedReplacement,
               CodePositionInterval position,
               SymbolBehavior behavior = SymbolBehavior::STATIC,
               SymbolType type = SymbolType::OTHER);
    void checkDependency(const TableEntry& entry);
    const std::string& name() const noexcept;
    const std::string& specifiedReplacement() const noexcept;
    const std::string& finalReplacement() const noexcept;
    SymbolType type() const noexcept;
    SymbolBehavior behavior() const noexcept;
    const CodePositionInterval& position() const noexcept;
    const std::regex& searchRegex() const noexcept;
    const std::set<std::string>& dependencies() const noexcept;

   private:
    void finalReplacement(const std::string& finalReplacement);
    std::string _name;
    std::string _specifiedReplacement;
    std::string _finalReplacement;
    SymbolType _type;
    SymbolBehavior _behavior;
    CodePositionInterval _position;
    std::regex _searchRegex;
    std::set<std::string> _dependencies;
  };
  using Table = std::map<std::string, TableEntry>;
  using ReplacementFunction =
      std::function<std::string(const std::string&, SymbolType)>;

  /**
   * \brief Instantiates an empty symbol table.
   */
  SymbolTable() = default;

  /**
   * \brief Inserts an entry into the SymbolTable and checks for any errors.
   * \param name The symbol name.
   * \param replacement The symbol replacement.
   * \param state The compile state.
   */
  void insertEntry(const std::string& name,
                   const std::string& replacement,
                   const CodePositionInterval& position,
                   CompileState& state,
                   SymbolBehavior behavior = SymbolBehavior::STATIC,
                   SymbolType type = SymbolType::OTHER);

  /**
   * \brief Clears the table.
   */
  void clearTable();

  bool finalizeEntries();

  /**
   * \brief Returns the internal symbol table.
   * \return The internal symbol table.
   */
  const Table& table() const noexcept;

  /**
   * \brief Replaces any symbols in the given string and records all occuring
   * errors.
   * \param source The source string.
   * \param state The compile state to log the errors.
   * \param replacer An optional ReplacementFunction to define if a certain
   * group of symbols (e.g. constants or labels) should be
   * replaced differently. The replacement function takes the raw replacement
   * string and the symbol type and returns a string that will replace the
   * symbol.
   * \return The string with all symbols replaced (if the maximum recursion
   * depth has not been exceeded).
   */
  std::string
  replaceSymbols(const std::string& source,
                 CompileState& state,
                 const ReplacementFunction& replacer = SIMPLE_REPLACE) const;

  /**
   * \brief Replaces any symbols in the given vector of strings and records all
   * occuring errors.
   * \param source The source vector.
   * \param state The compile state to log the errors
   * \param replacer An optional ReplacementFunction to define if a certain
   * group of symbols (e.g. constants or labels) should be
   * replaced differently. The replacement function takes the raw replacement
   * string and the symbol type and returns a string that will replace the
   * symbol
   */
  void
  replaceSymbols(std::vector<std::string>& source,
                 CompileState& state,
                 const ReplacementFunction& replacer = SIMPLE_REPLACE) const;

  static const ReplacementFunction SIMPLE_REPLACE;

 private:
  bool checkCyclic(std::vector<std::string>& order);
  bool doDfs(std::vector<std::string>& order,
             std::unordered_set<std::string>& visited,
             const std::string& startNode);
  void prepareSymbols(const std::vector<std::string>& order);

  /**
   * \brief The internal map to store the symbol table.
   */
  Table _table;
  std::vector<std::string> _replacementOrder;
};

#endif
