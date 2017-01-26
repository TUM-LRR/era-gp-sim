/* ERASIM Assembler Interpreter
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

#include <string>

#include "gtest/gtest.h"

#include "arch/riscv/instruction-context-information.hpp"
#include "tests/arch/riscv/base-fixture.hpp"

struct ContextInformationTest : public riscv::BaseFixture {
  using super = riscv::BaseFixture;
  ContextInformationTest() : super({"rv32i", "rv32m", "rv64i", "rv64m"}) {
  }
};

TEST_F(ContextInformationTest, allExist) {
  auto arch = project->getArchitectureAccess().getArchitecture().get();
  for (auto& pair : arch.getInstructions()) {
    const std::string& mnemonic = pair.second.getMnemonic();
    auto node = factories.createInstructionNode(mnemonic);
    // If a InstructionDocumentation exists, no assertion is thrown
    node->getInstructionDocumentation();
  }
}
