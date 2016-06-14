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

#ifndef ERAGPSIM_ARCH_RISCV_PARSER_INTERMEDIATE_OPERATION_H
#define ERAGPSIM_ARCH_RISCV_PARSER_INTERMEDIATE_OPERATION_H

#include <vector>
#include <string>
#include "SymbolTable.hpp"
#include"FinalRepresentation.hpp"

using LineInterval = std::pair<unsigned int, unsigned int>;

class IntermediateOperation
{
public:
    IntermediateOperation(const LineInterval& lines, const std::vector<std::string>& labels, const std::string& name)
        : _lines(lines), _labels(labels), _name(name)
        {}
    IntermediateOperation() = default;
    IntermediateOperation(const IntermediateOperation& other) = default;
    IntermediateOperation(IntermediateOperation&& other) = default;
    IntermediateOperation& operator =(const IntermediateOperation& other) = default;
    IntermediateOperation& operator =(IntermediateOperation&& other) = default;

    virtual void execute(FinalRepresentation& finalRepresentator, const SymbolTable& table) = 0;

    virtual void enhanceSymbolTable(SymbolTable& table);

protected:
    LineInterval _lines;
    std::vector<std::string> _labels;
    std::string _name;
};

#endif
