#include "parser\syntax-tree-generator.hpp"
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
