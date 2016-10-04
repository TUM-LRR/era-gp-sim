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

#ifndef ERAGPSIM_PARSER_PARSER_FACTORY_HPP_
#define ERAGPSIM_PARSER_PARSER_FACTORY_HPP_

#include <memory>
#include <string>
#include <unordered_map>
#include "parser/parser.hpp"

class Architecture;

using ParserPtr = std::unique_ptr<Parser>;

/**
 * Provides methods for the instantiation of differernt Parser classes.
 */
namespace ParserFactory {

/**
 * Instantiates a Parser for an Architecture by name.
 *
 * \param arch The architecture this parser is created for
 * \param name The name of the parser (see #mapping)
 * \return A unique pointer to the new parser.
 */
ParserPtr createParser(const Architecture &arch, const std::string &name);

/**
 * Maps names to Parser classes.
 *
 * See parser-factory.cpp for the definition including all available parser
 * names.
 */
extern const std::unordered_map<std::string,
                                ParserPtr (*)(const Architecture &)>
    mapping;
}

#endif /* ERAGPSIM_PARSER_PARSER_FACTORY_HPP */
