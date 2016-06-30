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

#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/extension-information.hpp"
#include "arch/common/information.hpp"
#include "arch/common/instruction-information.hpp"
#include "arch/common/instruction-set.hpp"
#include "arch/common/register-information.hpp"
#include "arch/common/unit-information.hpp"

TEST(arch_common_test, test_register_information) {
  auto eax = RegisterInformation("r0")
                 .id(0)
                 .size(32)
                 .type(RegisterInformation::Type::INTEGER)
                 .constant(5)
                 .addAlias("zero")
                 .enclosing(1)
                 .addConstituents({2, 3, 4});

  EXPECT_EQ(eax.getID(), 0);
  EXPECT_TRUE(eax.hasSize());
  EXPECT_EQ(eax.getSize(), 32);
  EXPECT_EQ(eax.getType(), RegisterInformation::Type::INTEGER);
  EXPECT_TRUE(eax.isConstant());
  EXPECT_EQ(eax.getConstant<std::size_t>(), 5);
  EXPECT_EQ(eax.getAliases(), std::vector<std::string>({"zero"}));
  EXPECT_TRUE(eax.hasEnclosing());
  EXPECT_EQ(eax.getEnclosing(), 1);
  EXPECT_EQ(eax.getConstituents(), std::vector<std::size_t>({2, 3, 4}));
}

// auto unit = UnitInformation("cpu").addRegister(eax);
// auto add = InstructionInformation("add").key(5);
// auto set = InstructionSet({add});
// auto base = ExtensionInformation("rvi32")
//   .alignmentBehavior(Information::AlignmentBehavior::STRICT)
//   .endianness(Information::Endianness::LITTLE)
//   .wordSize(32)
//   .addUnit(unit)
//   .addInstructions(set);
// auto arch = Architecture("risc-v", base).validate();
