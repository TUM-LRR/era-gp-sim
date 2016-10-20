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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include "arch/common/immediate-node.hpp"

#include "arch/common/validation-result.hpp"
#include "core/memory-access.hpp"
#include "core/memory-value.hpp"

const std::string ImmediateNode::IMMEDIATE_IDENTIFIER = "Imm";

ImmediateNode::ImmediateNode(const MemoryValue& value)
: AbstractSyntaxTreeNode(Type::IMMEDIATE), _value(value) {
}

void ImmediateNode::setValue(const MemoryValue& value) {
  _value = value;
}

MemoryValue ImmediateNode::getValue(MemoryAccess& memoryAccess) const {
  return _value;
}

ValidationResult ImmediateNode::validate(MemoryAccess& memoryAccess) const {
  // Immediate values can't have any children
  return AbstractSyntaxTreeNode::_children.size() == 0
             ? ValidationResult::success()
             : ValidationResult::fail(QT_TRANSLATE_NOOP(
                   "Syntax-Tree-Validation",
                   "The immediate node must not have any arguments"));
}

MemoryValue ImmediateNode::assemble() const {
  return _value;
}

const std::string& ImmediateNode::getIdentifier() const {
  return IMMEDIATE_IDENTIFIER;
}
