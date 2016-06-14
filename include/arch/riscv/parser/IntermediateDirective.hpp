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

#ifndef ERAGPSIM_ARCH_RISCV_PARSER_INTERMEDIATE_DIRECTIVE_H
#define ERAGPSIM_ARCH_RISCV_PARSER_INTERMEDIATE_DIRECTIVE_H

#include "IntermediateOperation.hpp"

//This is a dummy class for a future implementation.
class IntermediateDirective : public IntermediateOperation {
public:
    IntermediateDirective(const LineInterval& lines, const std::vector<std::string>& labels, const std::string& name) : base(lines, labels, name) {}
    IntermediateDirective() = default;
    IntermediateDirective(const IntermediateDirective& other) = default;
    IntermediateDirective(IntermediateDirective&& other) = default;
    IntermediateDirective& operator =(const IntermediateDirective& other) = default;
    IntermediateDirective& operator =(IntermediateDirective&& other) = default;
};

#endif
