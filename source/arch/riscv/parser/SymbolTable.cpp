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
* along with this program. If not, see <http://www.gnu.org/licenses/>.*/

#include "include/arch/riscv/parser/SymbolTable.hpp"

#include<stdexcept>

std::regex SymbolTable::symbolNameFormat("^[A-Za-z]\\w*$");

inline std::regex SymbolTable::makeRegex(const std::string name) const
{
    //Just searches for the word.
    return std::regex("\\b" + name + "\\b");
}


void SymbolTable::insert(const std::string& name, const std::string& replacement)
{
    if (!std::regex_search(name, symbolNameFormat))
    {
        throw std::runtime_error("Symbol has not a qualified name.");
    }    

    if (_values.find(name) != _values.end())
    {
        //Compile error, no symbols can be defined for than once.
        throw std::runtime_error("Symbol defined twice.");
    }

    //No recursive constants? Or should we implement them?
    const std::regex search = makeRegex(name);

    if (std::regex_search(replacement, search))
    {
        //Compile error, if we do not allow recursive macros.
        throw std::runtime_error("Recursive constants/aliases not allowed.");
    }

    //Maybe, we should remove the brackets until we have got a better parser...

    //Now we can loop through our map of existing symbols and insert our variable in each of them.
    for (auto& i : _values)
    {
        i.second = std::regex_replace(i.second, search, replacement);
    }

    //Then, insert our new one.
    _values[name] = replacement;
}


inline const std::map<std::string, std::string>& SymbolTable::values() const
{
    //Just returns the value list.
    return _values;
}


std::string SymbolTable::insertSymbols(const std::string& source) const {
    std::string result = source;

    //Just make a copy and iterate over all defined symbols. If they match, replace them.
    for (const auto& i : _values)
    {
        result = std::regex_replace(result, makeRegex(i.first), i.second);
    }
    return result;
}


void SymbolTable::insertSymbols(std::vector<std::string>& source) const
{
    //We just replace the symbols for each of the vector elements.

    for (int i = 0; i < source.size(); i++)
    {
        source[i] = insertSymbols(source[i]);
    }
}
