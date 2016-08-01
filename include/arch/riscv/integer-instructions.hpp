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

#ifndef ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTIONS_HPP_
#define ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTIONS_HPP_

#include <string>

#include "arch/riscv/instruction-node.hpp"

/*
 * TODO Instructions: add sub lt sltu and or xor sll srl sra
 *                    + their respective immediate equivalents
 */

namespace riscv {

class IntegerInstructionNode : public InstructionNode {
  /* Different types of integer instructions within RISC V.
     See RISC V specification for reference. */
  enum struct Type { ADD, SLT, SLTU, AND, OR, XOR, SLI, SRL, SRA };

 public:
  IntegerInstruction(InstructionInformation& instructionInformation,
                     Type type,
                     bool immediate)
  : InstructionNode(instructionInformation)
  , _type(type)
  , _immediate(immediate) {
  }

  virtual MemoryValue getValue(DummyMemoryAccess& memoryAccess) const;

  virtual bool validate() const;

  virtual MemoryValue assemble() const {
    return MemoryValue{};// TODO
  }

 private:
  Type _type;
  bool _immediate;
};
}
#endif /* ERAGPSIM_ARCH_RISCV_INTEGER_INSTRUCTIONS_HPP_ */
