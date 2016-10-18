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

#include <cctype>
#include <regex>

#include "arch/common/abstract-syntax-tree-node.hpp"
#include "arch/common/validation-result.hpp"
#include "core/memory-access.hpp"
#include "parser/compile-state.hpp"

std::unique_ptr<AbstractSyntaxTreeNode>
SyntaxTreeGenerator::transformOperand(const std::string& operand,
                                      CompileState& state) const {
  // We invoke our node generator to get a node!
  std::unique_ptr<AbstractSyntaxTreeNode> outputNode =
      _argumentGenerator(operand, _nodeFactories, state);

  // According to the architecture group, we get a nullptr if the creation
  // failed.
  if (!outputNode) {
    state.addError("Invalid argument: '" + operand + "'", state.position);
  }

  return std::move(outputNode);
}

std::unique_ptr<AbstractSyntaxTreeNode> SyntaxTreeGenerator::transformCommand(
    const std::string& command_name,
    std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>& sources,
    std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>& targets,
    CompileState& state,
    MemoryAccess& memoryAccess) const {
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
  auto validationResult = outputNode->validate(memoryAccess);
  if (!validationResult) {
    state.addError("Invalid operation (" + command_name + "): " +
                       validationResult.getMessage(),
                   state.position);
  }

  // Return.
  return std::move(outputNode);
}
