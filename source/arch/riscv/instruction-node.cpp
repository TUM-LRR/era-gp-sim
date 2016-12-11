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


#include <string>
#include <vector>

#include "arch/common/instruction-assembler.hpp"
#include "arch/common/instruction-key.hpp"
#include "arch/riscv/formats.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "common/assert.hpp"

namespace riscv {

InstructionNode::InstructionNode(const InstructionInformation& information)
: super(information) {
}

bool InstructionNode::_requireChildren(Type type,
                                       size_t startIndex,
                                       size_t amount) const {
  auto first = _children.begin();
  std::advance(first, startIndex);

  auto last = first;
  std::advance(last, amount);

  // clang-format off
  return std::all_of(first, last, [type] (const auto& child) {
    return child->getType() == type;
  });
  // clang-format on
}

bool InstructionNode::_compareChildTypes(TypeList list,
                                         size_t startIndex) const {
  auto view = Utility::viewFrom(_children, startIndex);

  // clang-format off
  return Utility::isEqual(list, view, [](auto type, const auto& child) {
    return type == child->getType();
  });
  // clang-format on
}

MemoryValue InstructionNode::assemble() const {
  Format::Arguments arguments;
  for (const auto& child : _children) {
    arguments.emplace_back(child.assemble());
  }

  return Format::assemble(_information, arguments);
}

const Translateable& InstructionNode::getInstructionDocumentation() const {
  assert::that(_documentation &&
               _documentation->isContextInformationAvailable(
                   _information.getMnemonic()));
  return _documentation->getContextInformation(getInstructionInformation());
}

void InstructionNode::setDocumentation(
    const InstructionContextInformationPointer& documentation) {
  _documentation = documentation;
}
}
