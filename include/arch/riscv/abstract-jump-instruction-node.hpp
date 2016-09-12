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

#ifndef ERAGPSIM_ARCH_RISCV_ABSTRACT_JUMP_INSTRUCTION_NODE_HPP
#define ERAGPSIM_ARCH_RISCV_ABSTRACT_JUMP_INSTRUCTION_NODE_HPP

#include "arch/common/instruction-information.hpp"
#include "arch/riscv/instruction-node.hpp"

namespace riscv {
template <typename UnsignedWord, typename SignedWord>
class AbstractJumpAndLinkInstructionNode : public InstructionNode {
 public:
  using super = InstructionNode;

  explicit AbstractJumpAndLinkInstructionNode(
      const InstructionInformation& information)
  : super(information) {
  }

  /**
   * Make the constructor pure virtual so that the class is abstract.
   * This is necessary since the class, in itself, has no pure virtual methods.
   * Note the implementation outside (below) the class definition.
   */
  virtual ~AbstractJumpAndLinkInstructionNode() = 0;

  MemoryValue getValue(MemoryAccess& memoryAccess) const override {
    // JAL rd, imm

    auto destination    = _children[0]->getIdentifier();
    auto programCounter = _loadRegister<UnsignedWord>("pc", memoryAccess);

    // Store the return address (pc + 4) in the destination register
    memoryAccess.setRegisterValue(destination, programCounter + 4);

    auto result = _jump(programCounter, memoryAccess);

    memoryAccess.setRegisterValue("pc", result);

    return {};
  }

  ValidationResult validate() const override {
    auto result = _validateNumberOfChildren();
    if (!result.isSuccess()) return result;

    result = _validateChildren();
    if (!result.isSuccess()) return result;

    result = _validateOperandTypes();
    if (!result.isSuccess()) return result;

    result = _validateOffset();
    if (!result.isSuccess()) return result;

    result = _validateResultingProgramCounter();
    if (!result.isSuccess()) return result;

    return ValidationResult::success();
  }

 protected:
  virtual UnsignedWord
  _jump(UnsignedWord programCounter, MemoryAccess& memoryAccess) const = 0;
  virtual ValidationResult _validateNumberOfChildren() const        = 0;
  virtual ValidationResult _validateOperandTypes() const            = 0;
  virtual ValidationResult _validateOffset() const                  = 0;
  virtual ValidationResult _validateResultingProgramCounter() const = 0;
};

template <typename UnsignedWord, typename SignedWord>
AbstractJumpAndLinkInstructionNode<UnsignedWord, SignedWord>::
    ~AbstractJumpAndLinkInstructionNode() = default;
}

#endif /* ERAGPSIM_ARCH_RISCV_ABSTRACT_JUMP_INSTRUCTION_NODE_HPP */
