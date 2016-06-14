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

#ifndef ERAGPSIM_ARCH_RISCV_PARSER_INTERMEDIATE_REPRESENTATOR_H
#define ERAGPSIM_ARCH_RISCV_PARSER_INTERMEDIATE_REPRESENTATOR_H

#include<vector>
#include<string>
#include<memory>
#include"IntermediateOperation.hpp"
#include"../../AbstractNodeFactories.hpp"
#include"SymbolTable.hpp"
#include"FinalRepresentation.hpp"

class IntermediateRepresentator {
public:
    IntermediateRepresentator() = default;
    IntermediateRepresentator(const IntermediateRepresentator& other) = default;
    IntermediateRepresentator(IntermediateRepresentator&& other) = default;
    IntermediateRepresentator& operator =(const IntermediateRepresentator& other) = default;
    IntermediateRepresentator& operator =(IntermediateRepresentator&& other) = default;

    FinalRepresentation transform(const SymbolTable& table);

private:
    std::vector<std::unique_ptr<IntermediateOperation>> _commandList;
};

#endif
