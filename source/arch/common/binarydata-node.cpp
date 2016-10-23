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
#include "include/arch/common/binarydata-node.hpp"

static std::string identifier = "binary-data";

BinaryDataNode::BinaryDataNode() : AbstractSyntaxTreeNode(Type::OTHER) {}

BinaryDataNode::BinaryDataNode(const std::string &data)
    : AbstractSyntaxTreeNode(Type::OTHER), _rawData(data.begin(), data.end()) {}

ValidationResult BinaryDataNode::validate(MemoryAccess &memoryAccess) const {
  if (_children.size() != 0) {
    return ValidationResult::fail(
        "A binary data node may not have any children");
  }
  return ValidationResult::success();
}

MemoryValue BinaryDataNode::assemble() const {
  MemoryValue assembled = MemoryValue(_rawData, _rawData.size());
  return assembled;
}

MemoryValue BinaryDataNode::getValue(MemoryAccess &memoryAccess) const {
  return assemble();
}

const std::string &BinaryDataNode::getIdentifier() const {
  return identifier;
}
