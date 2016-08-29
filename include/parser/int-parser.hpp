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

#ifndef ERAGPSIM_PARSER_INT_PARSER_HPP
#define ERAGPSIM_PARSER_INT_PARSER_HPP

#include <string>
#include "parser/compile-state.hpp"

/**
 * Provides methods for integer parsing.
 */
namespace IntParser {

///@{
/**
 * Parses an input string to a number.
 * \param input The input string.
 * \param state The compile state to record errors.
 * \param output The resulting number will be stored here.
 * \return Returns if the conversion was successful.
 */
bool parseInteger(const std::string &input, int &output, CompileState &state);
bool parseInteger(const std::string &input, long &output, CompileState &state);
bool parseInteger(const std::string &input,
                  long long &output,
                  CompileState &state);
bool parseInteger(const std::string &input,
                  unsigned long &output,
                  CompileState &state);
bool parseInteger(const std::string &input,
                  unsigned long long &output,
                  CompileState &state);
///@}
}
#endif /* ERAGPSIM_PARSER_INT_PARSER_HPP */
