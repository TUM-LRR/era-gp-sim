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
#include "parser/expression-compiler-clike.hpp"
#include "core/conversions.hpp"

#include <regex>
#include <cctype>

std::unique_ptr<AbstractSyntaxTreeNode>
SyntaxTreeGenerator::transformOperand(const std::string& operand,
                                      CompileState& state) const {
  // These checks are performed:
  // * Empty argument? Shouldn't happen, kill the compilation with fire.
  // * First character is a letter? We have replace all constants by now, so it must be a register - or an undefined constant!
  // * If not? Try to compile the expression!
  std::unique_ptr<AbstractSyntaxTreeNode> outputNode;
  if (operand.empty())
  {
    state.addError("Invalid argument: It's empty!", state.position);
  }
  else if (std::isalpha(operand[0])) {
    outputNode = _nodeFactories.createRegisterAccessNode(operand);
  } else {
    //using i32
    int32_t result = CLikeExpressionCompilers::CLikeCompilerI32.compile(operand, state);
    outputNode = _nodeFactories.createImmediateNode(conversions::convert(result, conversions::standardConversions::helper::twosComplement::toMemoryValueFunction, 32));
  }

  // according to the architecture group, we get a nullptr if the creation
  // failed.
  if (!outputNode) {
    state.addError("Invalid argument: '" + operand + "'", state.position);
  }

  // we already try to find flaws early in creation of the operation.
  auto validationResult = outputNode->validate();
  if (!validationResult) {
    state.addError("Invalid argument: '" + operand + "'", state.position);
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
    state.addError("Unknown operation: " + command_name, state.position);
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

  // Validate node.
  auto validationResult = outputNode->validate();
  if (!validationResult) {
    state.addError("Invalid operation: " + command_name, state.position);
  }

  // Return.
  return std::move(outputNode);
}
