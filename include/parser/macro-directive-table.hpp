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

#include <unordered_map>
#include <string>
#include "common/optional.hpp"

class MacroDirective;

class MacroDirectiveTable {
 private:
  // Need to declare these types before the rest of the class

  /**
   * Helper class to hash the pair.
   */
  struct hash_pair {
    // Partially from
    // http://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
    size_t operator()(const std::pair<std::string, size_t> &pair) const {
      size_t hash = 0;
      hash ^= std::hash<std::string>{}(pair.first) + 0x9e3779b9 + (hash << 6) +
              (hash >> 2);
      hash ^= std::hash<size_t>{}(pair.second) + 0x9e3779b9 + (hash << 6) +
              (hash >> 2);
      return hash;
    }
  };

  using macro_map = std::unordered_map<std::pair<const std::string &, size_t>,
                                       MacroDirective &,
                                       hash_pair>;

  class MacroWrapper;

 public:
  /**
   * Inserts a macro into the table.
   * \param macro Macro to insert.
   * \return True if successful.
   */
  bool insert(MacroDirective &macro);

  /**
   * Tries to find a macro with the given name that accepts `argCount`
   * arguments.
   * \param name Macro name
   * \param argCount Number of arguments
   * \return Iterator to the macro if found. Otherwise `end()`.
   */
  MacroWrapper find(const std::string &name, size_t argCount);

  /**
   * Returns begin of the underlying map.
   */
  macro_map::const_iterator begin();

  /**
   * Returns end of the underlying map.
   */
  macro_map::const_iterator end();

  /**
   * Clears the list of registered macros.
   */
  void clear();

 private:
  /**
   * Helper class to automatically set and unset the _isCompiling flag of the
   * macro directive.
   */
  class MacroWrapper {
   public:
    MacroWrapper(macro_map::iterator i, const macro_map::iterator &end);
    ~MacroWrapper();

    MacroWrapper(const MacroWrapper &) = delete;
    MacroWrapper &operator=(const MacroWrapper &) = delete;

    MacroWrapper(MacroWrapper &&other)
    : _i{std::move(other._i)}, _end{other._end}, _cyclic{other._cyclic} {
      other._i = other._end;// Invalidate other by setting _i to _end.
    }

    MacroWrapper &operator=(MacroWrapper &&other) = delete;

    /**
     * These delegate the call to the underlying iterator. Assert that
     * isCyclic() is false.
     */
    const macro_map::value_type &operator*();
    macro_map::const_iterator operator->();

    friend bool operator==(const MacroWrapper &first,
                           const macro_map::const_iterator &second) {
      return first._i == second;
    }

    friend bool operator!=(const MacroWrapper &first,
                           const macro_map::const_iterator &second) {
      return first._i != second;
    }

    bool isCyclic() {
      return _cyclic;
    }

    bool found() {
      return _i != _end;
    }

   private:
    macro_map::iterator _i;
    const macro_map::iterator _end;
    bool _cyclic = false;
  };

  macro_map _macros;
};

#endif /* ERAGPSIM_PARSER_MACRO_DIRECTIVE_TABLE_HPP */
