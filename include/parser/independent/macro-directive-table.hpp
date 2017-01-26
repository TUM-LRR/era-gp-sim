/*
* ERASIM Assembler Interpreter
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

#ifndef ERASIM_PARSER_INDEPENDENT_MACRO_DIRECTIVE_TABLE_HPP
#define ERASIM_PARSER_INDEPENDENT_MACRO_DIRECTIVE_TABLE_HPP

#include <cstddef>
#include <string>
#include <unordered_map>
#include <utility>

#include "common/hash.hpp"
#include "common/optional.hpp"
#include "parser/independent/macro-directive.hpp"
#include "parser/independent/positioned-string.hpp"

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
    /**
     * Creates a new macro wrapper out of a macro iterator.
     *
     * \param iterator The iterator to begin with.
     * \param end The iterator position to stop at.
     */
    MacroWrapper(MacroIterator iterator, MacroIterator end);

    /**
     * Creates a new macro wrapper out of a moved one.
     *
     * \param other The moved macro wrapper.
     */
    MacroWrapper(MacroWrapper &&other);

    /**
     * Destroys the given macro wrapper.
     */
    ~MacroWrapper();

    MacroWrapper(const MacroWrapper &) = delete;
    MacroWrapper &operator=(const MacroWrapper &) = delete;
    MacroWrapper &operator=(MacroWrapper &&other) = delete;

    /**
     * These delegate the call to the underlying iterator. Assert that
     * isCyclic() is false.
     *
     * \return The underlying iterator.
     */
    const MacroMap::value_type &operator*();

    /**
     * Provides access to the macro iterator (asserts that isCyclic() is false).
     *
     * \return The macro iterator.
     */
    MacroConstIterator operator->();

    /**
     * Compares for equality with another iterator.
     *
     * \param iterator The other iterator.
     * \return True, if and only if the iterator and this wrapper are equal.
     */
    bool operator==(MacroConstIterator iterator) const noexcept;

    /**
     * Compares for inequality with another iterator.
     *
     * \param iterator The other iterator.
     * \return True, if and only if the iterator and this wrapper are *not*
     * equal.
     */
    bool operator!=(MacroConstIterator iterator) const noexcept;

    /**
     * \return True, if the macro contains a cyclic call, else false.
     */
    bool isCyclic() const noexcept;

    /**
     * \return True, if the macro has been found in the directive table.
     */
    bool found() const noexcept;

    /**
     * \return The forwarded name of the macro.
     */
    const PositionedString &name() const;

   private:
    /**
     * The internal macro iterator position.
     */
    MacroIterator _iterator;

    /**
     * The final macro iterator position.
     */
    const MacroIterator _end;

    /**
     * True, if the macro contains a cyclic call, else false.
     */
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
   *
   * \param name Macro name
   * \param argumentCount Number of arguments
   * \return Iterator to the macro if found. Otherwise `end()`.
   */
  MacroWrapper find(const std::string &name, size_t argumentCount);

  /**
   * \return The begin of the underlying map.
   */
  MacroConstIterator begin();

  /**
   * \return The end of the underlying map.
   */
  MacroConstIterator end();

  /**
   * Clears the list of registered macros.
   */
  void clear();

 private:
  /**
   * The internal macro map.
   */
  MacroMap _macros;
};

#endif /* ERASIM_PARSER_INDEPENDENT_MACRO_DIRECTIVE_TABLE_HPP */
