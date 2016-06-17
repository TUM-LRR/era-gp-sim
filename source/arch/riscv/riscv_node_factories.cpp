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

#include "include/arch/riscv/riscv_node_factories.hpp"

void RISCVInstructionNodeFactory::initializeInstructionMap() {

  //example
  instructionMap["ADD"] = []() {
    return std::unique_ptr<AbstractSyntaxTreeNode>(
        new AbstractSyntaxTreeNode());
  };
  instructionMap["SUB"] = []() {
    return std::unique_ptr<AbstractSyntaxTreeNode>(
        new AbstractSyntaxTreeNode());
  };
}

std::unique_ptr<AbstractSyntaxTreeNode>
RISCVInstructionNodeFactory::createInstructionNode(std::string& token) {
  std::string upper = token;

  auto it = instructionMap.find(upper);
  if (it != instructionMap.end()) {
    return instructionMap[upper]();
  } else {
    return nullptr;
  }
}

std::unique_ptr<AbstractSyntaxTreeNode>
RISCVImmediateNodeFactory::createImmediateNode(MemoryValue v) {
  return nullptr;  // TODO hier immediatenode instanziiern
}

std::unique_ptr<AbstractSyntaxTreeNode> RISCVRegisterAccessNodeFactory::createRegisterAccessNode(const std::string& id) {
  /*
   * TODO Waiting for Register-Access Node to be implemented
   */
return nullptr;
}
