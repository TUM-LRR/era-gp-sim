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

#ifndef ERAGPSIM_PARSER_MACRO_DIRECTIVE_HPP
#define ERAGPSIM_PARSER_MACRO_DIRECTIVE_HPP

#include "parser/intermediate-directive.hpp"

class MacroDirective : public IntermediateDirective {
public:
    using Super = IntermediateDirective;

    MacroDirective(const LineInterval& lines,
                   const std::vector<std::string>& labels,
                   const std::string& name,
                   const std::string& macroName,
                   const std::vector<std::string>& macroParameters)
        : Super(lines, labels, macroName), _macroName(macroName), _macroParameters(macroParameters)
    {

    }

    virtual void execute(FinalRepresentation& finalRepresentator,
                       const SymbolTable& table,
                       CompileState& state);

    virtual bool targetOutput(OperationOutputFunction& target, const OperationOutputFunction& mainOutput, CompileState& state) const;

    const std::string& macroName()
    {
        return _macroName;
    }

private:
    std::string _macroName;
    std::vector<std::string> _macroParameters;
    std::vector<IntermediateOperationPointer> _operations;
    const OperationOutputFunction _ownOutputFunction = [this](IntermediateOperationPointer pointer) -> void {
        this->_operations.push_back(std::move(pointer));
    };
};

#endif /* ERAGPSIM_PARSER_MACRO_DIRECTIVE_HPP */
