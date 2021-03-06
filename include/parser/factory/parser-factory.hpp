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

#ifndef ERAGPSIM_PARSER_FACTORY_PARSER_FACTORY_HPP
#define ERAGPSIM_PARSER_FACTORY_PARSER_FACTORY_HPP

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class Parser;
class MemoryAccess;
class Architecture;
class MemoryAccess;

/**
 * Abbreviation for a pointer to a parsing unit.
 */
using ParserPtr = std::unique_ptr<Parser>;

/**
 * Provides methods for the instantiation of different Parser classes.
 */
namespace ParserFactory {
using ParserBuildFunction =
    std::function<ParserPtr(const Architecture &, const MemoryAccess &)>;

/**
 * Instantiates a Parser for an Architecture by name.
 *
 * \param architecture The architecture this parser is created for
 * \param memoryAccess The access to the memory to store data and validate
 * instructions.
 * \param name The name of the parser (see #mapping)
 * \return A unique pointer to the new parser.
 */
ParserPtr createParser(const Architecture &architecture,
                       const MemoryAccess &memoryAccess,
                       const std::string &name);

/**
 * Maps names to Parser classes.
 *
 * See parser-factory.cpp for the definition including all available parser
 * names.
 */
extern const std::unordered_map<std::string, ParserBuildFunction> mapping;
}  // namespace ParserFactory

#endif /* ERAGPSIM_PARSER_FACTORY_PARSER_FACTORY_HPP */
