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

#include "parser/intermediate-instruction.hpp"
#include "gtest/gtest.h"

// Hehe...
#define SAMPLE_COMMAND                                                  \
  LineInterval(0, 1), {"label1", "label2", "label3"}, "mov", {"eax"}, { \
    "eax"                                                               \
  }

TEST(IntermediateInstruction, initSimple) {
  // I don't know if this is useful, but it might be, so...
  IntermediateInstruction ii(SAMPLE_COMMAND);
}

TEST(IntermediateInstruction, enhance) {
  // We create an instruction and insert our labels into the symbol table.
  IntermediateInstruction ii(SAMPLE_COMMAND);
  SymbolTable st;
  CompileState state;
  ii.enhanceSymbolTable(st, state);
  ASSERT_EQ(state.errorList.size(), 0);
  ASSERT_EQ(st.table().size(), 3);
  ASSERT_EQ(st.table().at("label1"), std::to_string(ii.address()));
}

// For now, disable.
/*TEST(IntermediateInstruction, transformFinal) {
  IntermediateInstruction ii(SAMPLE_COMMAND);
  SymbolTable st;
  CompileState state;
  ii.enhanceSymbolTable(st, state);
  FinalRepresentation fr;
  ii.execute(fr, st, state);
  ASSERT_EQ(state.errorList.size(), 0);
  ASSERT_EQ(fr.commandList.size(), 1);
}*/
