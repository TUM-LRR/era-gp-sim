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

#include <iostream>

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/extension-information.hpp"
#include "arch/common/information.hpp"
#include "arch/common/instruction-information.hpp"
#include "arch/common/instruction-set.hpp"
#include "arch/common/register-information.hpp"
#include "arch/common/unit-information.hpp"

auto main() -> int {
  // clang-format off
  auto eax =
    RegisterInformation("eax")
    .id(0)
    .size(32)
    .type(RegisterInformation::Type::INTEGER)
    .hardwiredTo(5)
    .addAlias("zero")
    .enclosing(1)
    .addConstituents({1, 2, 3});
  auto unit = UnitInformation("cpu").addRegister(eax);
  auto add = InstructionInformation("add").key(5);
  auto set = InstructionSet({add});
  auto base = ExtensionInformation("rvi32")
    .alignmentBehavior(Information::AlignmentBehavior::STRICT)
    .endianness(Information::Endianness::LITTLE)
    .wordSize(32)
    .addUnit(unit)
    .addInstructions(set);
  auto arch = Architecture("risc-v", base).validate();
  // clang-format on

  std::cout << arch.getWordSize() << std::endl;
}
