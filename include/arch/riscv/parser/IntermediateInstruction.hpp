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

#ifndef ERAGPSIM_ARCH_RISCV_PARSER_INTERMEDIATE_INSTRUCTION_H
#define ERAGPSIM_ARCH_RISCV_PARSER_INTERMEDIATE_INSTRUCTION_H

#include <vector>
#include <string>
#include <map>
#include "IntermediateOperation.hpp"
#include "../../AbstractNodeFactories.hpp"
#include "SymbolTable.hpp"

class IntermediateInstruction : public IntermediateOperation {
public:
    IntermediateInstruction(const LineInterval& lines, const std::vector<std::string>& labels, const std::string& name, const std::vector<std::string>& sources, const std::vector<std::string>& targets)
        : IntermediateOperation(lines, labels, name), _sources(sources), _targets(targets) {}
    IntermediateInstruction() = default;
    IntermediateInstruction(const IntermediateInstruction& other) = default;
    IntermediateInstruction(IntermediateInstruction&& other) = default;
    IntermediateInstruction& operator =(const IntermediateInstruction& other) = default;
    IntermediateInstruction& operator =(IntermediateInstruction&& other) = default;

    virtual void execute(FinalRepresentation& finalRepresentator, const SymbolTable& table);

    AbstractSyntaxTreeNode compileInstruction(const SymbolTable& table, const SymbolTable& additional);

private:
    std::vector<std::string> _sources;
    std::vector<std::string> _targets;
};

#endif
