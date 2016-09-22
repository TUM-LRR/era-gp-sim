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

std::unique_ptr<AbstractSyntaxTreeNode>
SyntaxTreeGenerator::transformOperand(const std::string& operand,
                                      CompileState& state) const {
  // For now. A very simple generator. We just check: do we have a base-10
  // number? Yes? If not, we must have a register... If it does not exist? Well,
  // we failed.
  // For the future, we got to: support several number types (simple), maybe
  // parse full-grown arithmetic expressions (done, not so easy..., lots of
  // work).
  std::unique_ptr<AbstractSyntaxTreeNode> outputNode;
  if (std::regex_search(operand, std::regex("^[0-9]+$"))) {
    outputNode = _nodeFactories.createImmediateNode(
        MemoryValue{});// std::stoi(operand) Temporary.
  } else {
    outputNode = _nodeFactories.createRegisterAccessNode(operand);
  }

  // according to the architecture group, we get a nullptr if the creation
  // failed.
  if (!outputNode) {
    state.errorList.push_back(CompileError("Invalid argument: " + operand,
                                           state.position,
                                           CompileErrorSeverity::ERROR));
  }

  return std::move(outputNode);
}

std::unique_ptr<AbstractSyntaxTreeNode> SyntaxTreeGenerator::transformCommand(
    const std::string& command_name,
    std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>& sources,
    std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>& targets,
    CompileState& state) const {
  // Just create an instruction node and add all output and input nodes
  // (operands).
  auto outputNode = _nodeFactories.createInstructionNode(command_name);

  if (!outputNode) {
    // The node creation failed!
    state.errorList.push_back(CompileError("Unknown operation: " + command_name,
                                           state.position,
                                           CompileErrorSeverity::ERROR));
    return std::move(outputNode);
  }

  // Targets.
  for (auto& i : targets) {
    outputNode->addChild(std::move(i));
  }

  // Sources.
  for (auto& i : sources) {
    outputNode->addChild(std::move(i));
  }

  // Validate node
  if (!outputNode->validate()) {
    state.errorList.push_back(CompileError("Invalid operation: " + command_name,
                                           state.position,
                                           CompileErrorSeverity::ERROR));
  }

  // Return.
  return std::move(outputNode);
}
