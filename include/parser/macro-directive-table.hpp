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

#ifndef ERAGPSIM_PARSER_MACRO_DIRECTIVE_TABLE_HPP
#define ERAGPSIM_PARSER_MACRO_DIRECTIVE_TABLE_HPP

#include <string>
#include <unordered_map>
#include <utility>

#include "common/hash.hpp"
#include "common/optional.hpp"
#include "parser/macro-directive.hpp"
#include "parser/positioned-string.hpp"

class MacroDirectiveTable {
 public:
  using size_t = std::size_t;

 private:
  using MacroInstance = std::pair<std::string, size_t>;
  using MacroMap = std::unordered_map<MacroInstance, MacroDirective>;
  using MacroIterator = MacroMap::iterator;
  using MacroConstIterator = MacroMap::const_iterator;

  /**
   * Helper class to automatically set and unset the _isCompiling flag of the
   * macro directive.
   */
  class MacroWrapper {
   public:
    MacroWrapper(MacroIterator i, MacroIterator end);
    MacroWrapper(MacroWrapper &&);
    ~MacroWrapper();

    MacroWrapper(const MacroWrapper &) = delete;
    MacroWrapper &operator=(const MacroWrapper &) = delete;
    MacroWrapper &operator=(MacroWrapper &&other) = delete;

    /**
     * These delegate the call to the underlying iterator. Assert that
     * isCyclic() is false.
     */
    const MacroMap::value_type &operator*();
    MacroConstIterator operator->();

    bool operator==(MacroConstIterator iterator) const noexcept;
    bool operator!=(MacroConstIterator iterator) const noexcept;

    bool isCyclic() const noexcept;
    bool found() const noexcept;

    const PositionedString &name() const;

   private:
    MacroIterator _iterator;
    const MacroIterator _end;
    bool _cyclic = false;
  };

 public:
  /**
   * Inserts a macro into the table.
   * \param macro Macro to insert.
   * \return True if successful.
   */
  bool insert(const MacroDirective &macro);

  /**
   * Tries to find a macro with the given name that accepts `argumentCount`
   * arguments.
   * \param name Macro name
   * \param argumentCount Number of arguments
   * \return Iterator to the macro if found. Otherwise `end()`.
   */
  MacroWrapper find(const std::string &name, size_t argumentCount);

  /**
   * Returns begin of the underlying map.
   */
  MacroConstIterator begin();

  /**
   * Returns end of the underlying map.
   */
  MacroConstIterator end();

  /**
   * Clears the list of registered macros.
   */
  void clear();

 private:
  MacroMap _macros;
};

#endif /* ERAGPSIM_PARSER_MACRO_DIRECTIVE_TABLE_HPP */
