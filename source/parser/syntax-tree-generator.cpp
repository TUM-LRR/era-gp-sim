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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "parser/syntax-tree-generator.hpp"
#include <regex>

//TODO: Error handling.

std::unique_ptr<AbstractSyntaxTreeNode> SyntaxTreeGenerator::transformOperand(const std::string& operand, CompileState& state) const
{
    //For now. A very simple generator. We just check: do we have a number? Yes? If not, we must have a register... If it does not exist? Well, we failed.
    if (std::regex_search(operand, std::regex("^[0-9]+$")))
    {
        return std::move(_nodeFactories.createImmediateNode(MemoryValue{}));//(std::stoi(operand))));//Temporary.
    }
    else
    {
        return std::move(_nodeFactories.createRegisterAccessNode(operand));
    }
}

std::unique_ptr<AbstractSyntaxTreeNode> SyntaxTreeGenerator::transformCommand(const std::string& command_name, std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>& sources, std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>& targets, CompileState& state) const
{
    //Just create an instruction node and add all output and input nodes (operands).
    auto outputNode = _nodeFactories.createInstructionNode(command_name);

    //Targets.
    for (auto& i : targets)
    {
        outputNode->addChild(std::move(i));
    }

    //Sources.
    for (auto& i : sources)
    {
        outputNode->addChild(std::move(i));
    }

    //Return.
    return std::move(outputNode);
}
