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

#ifndef ERAGPSIM_ARCH_RISCV_PARSER_SYMBOL_TABLE_H
#define ERAGPSIM_ARCH_RISCV_PARSER_SYMBOL_TABLE_H

#include<map>
#include<string>
#include<regex>

class SymbolTable
{
public:
    //Standard constructor for an empty symbol table.
    SymbolTable() = default;

    //Copy constructor for a symbol table.
    SymbolTable(const SymbolTable& other) = default;

    //Move constructor for a symbol table.
    SymbolTable(SymbolTable&& other) = default;

    //Copy assignment operator.
    SymbolTable& operator =(const SymbolTable& other) = default;

    //Move assignment operator.
    SymbolTable& operator =(SymbolTable&& other) = default;

    //This method inserts a 'replacement' for 'name' into the symbol table.
    //It updates all other entries of the symbol table and replaces the 'name'
    //in them, if they contain it. It also checks, if 'name' is valid and if
    //it has been already inserted into the table.
    void insert(const std::string& name, const std::string& replacement);

    //Returns the internal symbol table.
    const std::map<std::string, std::string>& values() const;

    //Replaces any symbol occurence in 'source'.
    std::string insertSymbols(const std::string& source) const;

    //Replaces any symbol occurences in the vector of strings 'source'.
    void insertSymbols(std::vector<std::string>& source) const;
    
private:
    //Turns 'name' into the regex '\\bname\\b'.
    std::regex makeRegex(const std::string name) const;

    //The internal map for storage of the table.
    std::map<std::string, std::string> _values;

    //The required format of the symbols.
    static /*const*/ std::regex symbolNameFormat; //I want this const, but it doesn't let me :(
};

#endif
