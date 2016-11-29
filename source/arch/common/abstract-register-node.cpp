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

#include "arch/common/abstract-register-node.hpp"
#include <sstream>

#include "arch/common/validation-result.hpp"
#include "core/memory-access.hpp"
#include "core/memory-value.hpp"

AbstractRegisterNode::AbstractRegisterNode(const std::string& identifier)
: AbstractSyntaxTreeNode(Type::REGISTER), _identifier(identifier) {
}

MemoryValue AbstractRegisterNode::getValue(MemoryAccess& memoryAccess) const {
  return memoryAccess.getRegisterValue(_identifier).get();
}

ValidationResult AbstractRegisterNode::validate(MemoryAccess& memoryAccess) const {
  // Registers can't have any children
  if (AbstractSyntaxTreeNode::_children.size() == 0) {
    return ValidationResult::success();
  }
  return ValidationResult::fail(
      QT_TRANSLATE_NOOP("Syntax-Tree-Validation",
                        "The register node must not have any arguments"));
}

const std::string& AbstractRegisterNode::getIdentifier() const {
  return _identifier;
}
