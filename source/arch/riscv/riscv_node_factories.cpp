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

#include <cctype>

#include "include/arch/riscv/riscv_node_factories.hpp"

void RISCVInstructionNodeFactory::initializeInstructionMap() {
  // example
  _instructionMap["ADD"] = []() {
//      return std::make_shared<AbstractSyntaxTreeNode>();
    return std::unique_ptr<AbstractSyntaxTreeNode>(
        new AbstractSyntaxTreeNode());
  };
  // example
  _instructionMap["SUB"] = []() {
    return std::unique_ptr<AbstractSyntaxTreeNode>(
        new AbstractSyntaxTreeNode());
  };
}

std::unique_ptr<AbstractSyntaxTreeNode>
RISCVInstructionNodeFactory::createInstructionNode(std::string& token) {
  std::string upper = token;
  for (auto& c : upper) {
    c = toupper(c);
  }

  auto it = _instructionMap.find(upper);  // lookup the uppercase token
  if (it != _instructionMap.end()) {
    return it->second();  // dereference iterator to the key-value pair and call
                          // the function
  } else {
    return nullptr;  // return nullptr as the uppercase token could not be found
  }
}

std::unique_ptr<AbstractSyntaxTreeNode>
RISCVImmediateNodeFactory::createImmediateNode(MemoryValue v) {
  return nullptr;  // TODO construct Immediate Node
}

std::unique_ptr<AbstractSyntaxTreeNode>
RISCVRegisterAccessNodeFactory::createRegisterAccessNode(
    const std::string& id) {
  /*
   * TODO Waiting for Register-Access Node to be implemented
   */
  return nullptr;
}
