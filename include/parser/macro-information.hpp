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

#ifndef ERAGPSIM_PARSER_MACRO_INFORMATION_HPP
#define ERAGPSIM_PARSER_MACRO_INFORMATION_HPP

#include "parser/code-position.hpp"

/**
 * This is a dummy class, parser goup, please complete this.
 */
class MacroInformation {
 public:
  /**
   * \brief Instantiates a new compile error with the given arguments.
   * \param message The error message.
   * \param position The position interval of the error in the code.
   * \param severity The severity of the error.
   */
  MacroInformation(const std::string& code,
                   const CodePositionInterval& position)
  : _code(code), _position(position) {
  }

  /**
   * Returns the expanded code for this macro.
   */
  const std::string& macroCode() const {
    return _code;
  }

  /**
   * \brief Returns the position of this macro.
   * \return The position of the macro.
   */
  const CodePositionInterval& position() const {
    return _position;
  }


 private:
  /**
   * \brief The internal code attribute.
   */
  std::string _code;

  /**
   * \brief The internal position attribute.
   */
  CodePositionInterval _position;
};

#endif /* ERAGPSIM_PARSER_MACRO_INFORMATION_HPP */
