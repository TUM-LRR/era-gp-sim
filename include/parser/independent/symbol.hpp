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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_SYMBOL_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_SYMBOL_HPP

#include <regex>
#include <string>

#include "parser/independent/positioned-string.hpp"

/**
 * Denotes the behavior of a symbol.
 */
enum class SymbolBehavior {
  /**
   * The symbol value never changes, i.e. we can optimize with it.
   */
  STATIC,

  /**
   * The symbol value might change, so we got to dynamically replace it
   * each time we call.
   */
  DYNAMIC
};

/**
 * Basically this means some name which points to some value (i.e. it will be
 * replaced with it). It is a very basic class which is used to represent
 * labels, constants etc.
 */
class Symbol {
 public:
  /**
   * Constructs a new symbol with the given parameters.
   *
   * \param name The name of the symbol.
   * \param value The value of the symbol, i.e. the string with which the name
   * will be replaced in given expressions.
   * \param behavior The behavior of the symbol, i.e. if it might change some
   * time. Needed for optimizing the symbols.
   */
  Symbol(const PositionedString& name,
         const PositionedString& value,
         SymbolBehavior behavior = SymbolBehavior::STATIC);

  /**
   * \return The name of the symbol.
   */
  const PositionedString& name() const noexcept;

  /**
   * \return The value of the symbol, i.e. the string with which the name will
   * be replaced in given expressions.
   */
  const PositionedString& value() const noexcept;

  /**
   * \return The behavior of the symbol, i.e. if it might change some time.
   * Needed for optimizing the symbols.
   */
  SymbolBehavior behavior() const noexcept;

  /**
   * \return A regex which matched all occurences of the symbol name (when
   * surrounded by non-word characters (a word character is part of
   * [_0-9A-Za-z])).
   */
  const std::regex& regex() const noexcept;

  /**
   * Checks if the name of this symbol is valid.
   * \return True, if the name is valid, else false.
   */
  bool nameValid() const;

 private:
  /**
   * The name of the symbol.
   */
  PositionedString _name;

  /**
   * The value of the symbol, i.e. the string with which the name will be
   * replaced in given expressions.
   */
  PositionedString _value;

  /**
   * The behavior of the symbol, i.e. if it might change some time.
   * Needed for optimizing the symbols.
   */
  SymbolBehavior _behavior;

  /**
   * A regex which matched all occurences of the symbol name (when
   * surrounded by non-word characters (a word character is part of
   * [_0-9A-Za-z])).
   */
  std::regex _regex;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_SYMBOL_HPP */
