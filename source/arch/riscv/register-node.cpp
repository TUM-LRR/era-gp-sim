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

#include "arch/riscv/register-node.hpp"
#include "arch/riscv/utility.hpp"

RegisterNode::RegisterNode(const std::string& identifier)
: AbstractRegisterNode(identifier) {
}

MemoryValue RegisterNode::assemble() const {
  // all register identifiers of riscv start with x
  auto number = getIdentifier().substr(1);
  if (!number.empty()) {
    try {
      unsigned int registerId = std::stoi(number);
      return conversions::convert(registerId,
                                  5,
                                  riscv::BITS_PER_BYTE,
                                  riscv::ENDIANNESS,
                                  riscv::SIGNED_REPRESENTATION);
    } catch (std::invalid_argument& ignore) {
    } catch (std::out_of_range& ignore) {
    }
  }
  return MemoryValue{5};
}
