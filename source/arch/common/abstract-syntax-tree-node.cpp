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

#include "arch/common/abstract-syntax-tree-node.hpp"

AbstractSyntaxTreeNode::AbstractSyntaxTreeNode(Type nodeType)
    : _nodeType(nodeType) {}

MemoryValue AbstractSyntaxTreeNode::operator()(
    MemoryAccess& memoryAccess) const {
  return getValue(memoryAccess);
}

AbstractSyntaxTreeNode::Type AbstractSyntaxTreeNode::getType() const noexcept {
  return _nodeType;
}

size_t AbstractSyntaxTreeNode::numberOfChildren() const noexcept {
  return _children.size();
}

bool AbstractSyntaxTreeNode::hasChildren() const noexcept {
  return !_children.empty();
}

void AbstractSyntaxTreeNode::addChild(Node&& node) {
  _children.emplace_back(std::move(node));
}

void AbstractSyntaxTreeNode::insertChild(size_t index, Node&& node) {
  assert(index < _children.size());

  auto iterator = _children.cbegin();
  std::advance(iterator, index);

  _children.emplace(iterator, std::move(node));
}

void AbstractSyntaxTreeNode::setChild(size_t index, Node&& node) {
  assert(index < _children.size());

  _children[index] = std::move(node);
}

ValidationResult AbstractSyntaxTreeNode::_validateChildren() const {
  for (auto& child : _children) {
    auto result = child->validate();
    if (!result.isSuccess()) {
      return result;
    }
  }

  return ValidationResult::success();
}

bool AbstractSyntaxTreeNode::_fitsIntoNBit(const MemoryValue& value, size_t n,
                                           bool isSigned) const {
  if (value.getSize() > n) {
    // Look for the sign bit to determine what bits to expect in the "upper"
    // region (i.e. n-1...size).
    // Index 0 <-> MSB in Memory Value
    bool isSignBitSet = isSigned && value.get(value.getSize() - 1);
    for (std::size_t index = n - 1; index < value.getSize(); ++index) {
      if ((isSignBitSet && !value.get(index)) ||
          (!isSignBitSet && value.get(index))) {
        return false;
      }
    }
  }
  return true;
}
