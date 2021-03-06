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

#include <memory>
#include <string>

#include "arch/riscv/immediate-node-factory.hpp"
#include "arch/common/immediate-node.hpp"
#include "arch/riscv/utility.hpp"

namespace riscv {

ImmediateNodeFactory::Node
ImmediateNodeFactory::createImmediateNode(const MemoryValue &value) const {
  return std::make_unique<ImmediateNode>(value);
}

const ImmediateNodeFactory::MnemonicSet ImmediateNodeFactory::_addressRelativeInstructions = {"jal", "beq", "bne", "blt", "bltu", "bge", "bgeu"};

MemoryValue ImmediateNodeFactory::labelToImmediate(const MemoryValue &labelValue, const std::string &instructionMnemonic, const MemoryValue &instructionAddress) const {
    if(_addressRelativeInstructions.count(instructionMnemonic) > 0) {
        auto abs = riscv::convert<riscv::signed64_t>(labelValue);
        auto pc = riscv::convert<riscv::signed64_t>(instructionAddress);
        return riscv::convert((abs-pc)/2);
    }
    return labelValue;
}
}
