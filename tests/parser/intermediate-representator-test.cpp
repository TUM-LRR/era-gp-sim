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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "parser/intermediate-representator.hpp"
#include "gtest/gtest.h"
#include "parser/intermediate-instruction.hpp"

TEST(IntermediateRepresentator, insertSimple) {
  IntermediateRepresentator ir;
  ir.insertCommand(IntermediateInstruction(LineInterval(0, 1),
                                           {"label1", "label2", "label3"},
                                           "mov",
                                           {"eax"},
                                           {"eax"}));
}

TEST(IntermediateRepresentator, transformSimple) {
  CompileState state;
  IntermediateRepresentator ir;
  ir.insertCommand(IntermediateInstruction(LineInterval(0, 1),
                                           {"label1", "label2", "label3"},
                                           "mov",
                                           {"eax"},
                                           {"eax"}));
  ir.insertCommand(IntermediateInstruction(
      LineInterval(2, 5), {"label4"}, "add", {"eax"}, {"ebx"}));
  // Test disabled for now.
  // FinalRepresentation fr = ir.transform(state);
  // ASSERT_EQ(fr.commandList.size(), 2);
}
