#include "parser\syntax-tree-generator.hpp"
#include <regex>

//TODO: Error handling.

std::unique_ptr<AbstractSyntaxTreeNode> SyntaxTreeGenerator::transformOperand(const std::string& operand, CompileState& state) const
{
    //For now. A very simple generator. We just check: do we have a number? Yes? Else we must have a register... If it does not exist? Well, we failed.
    if (std::regex_search(operand, std::regex("^[0-9]+$")))
    {
        return std::move(_nodeFactories.createImmediateNode(MemoryValue{}));//Temporary. (std::stoi(operand));
    }
    else
    {
        return std::move(_nodeFactories.createRegisterAccessNode(operand));
    }
}

std::unique_ptr<AbstractSyntaxTreeNode> SyntaxTreeGenerator::transformCommand(const std::string& command_name, const std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>& sources, const std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>& targets, CompileState& state) const
{
    //Just create an instruction node and add all output and input nodes (operands).
    auto outputNode = _nodeFactories.createInstructionNode(command_name);
    for (const auto& i : targets)
    {
        outputNode.addChild(std::move(i));
    }
    for (const auto& i : sources)
    {
        outputNode.addChild(std::move(i));
    }
    return std::move(outputNode);
}