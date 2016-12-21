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
#include "parser/compile-error-annotator.hpp"
#include "parser/compile-error-list.hpp"
#include "parser/intermediate-instruction.hpp"
#include "parser/macro-directive.hpp"
#include "parser/macro-end-directive.hpp"
#include "parser/positioned-string.hpp"
#define DEFINE_ANNOTATOR      \
  CompileErrorList errorList; \
  CompileErrorAnnotator annotator(errorList, CodePosition(0), CodePosition(0));
#define ZP(x) PositionedString(x, CodePositionInterval())

TEST(IntermediateRepresentator, insertSimple) {
  DEFINE_ANNOTATOR;
  IntermediateRepresentator ir;
  ir.insertCommand(
      IntermediateInstruction(LineInterval(0, 1),
                              {ZP("label1"), ZP("label2"), ZP("label3")},
                              ZP("mov"),
                              {ZP("eax")},
                              {ZP("eax")}),
      annotator);
}

TEST(IntermediateRepresentator, transformSimple) {
  DEFINE_ANNOTATOR;
  IntermediateRepresentator ir;
  ir.insertCommand(
      IntermediateInstruction(LineInterval(0, 1),
                              {ZP("label1"), ZP("label2"), ZP("label3")},
                              ZP("mov"),
                              {ZP("eax")},
                              {ZP("eax")}),
      annotator);
  ir.insertCommand(IntermediateInstruction(LineInterval(2, 5),
                                           {ZP("label4")},
                                           ZP("add"),
                                           {ZP("eax")},
                                           {ZP("ebx")}),
                   annotator);
  // Test disabled for now.
  // FinalRepresentation fr = ir.transform(state);
  // ASSERT_EQ(fr.commandList.size(), 2);
}

TEST(IntermediateRepresentator, macroDefinition) {
  DEFINE_ANNOTATOR;
  IntermediateRepresentator ir;
  ir.insertCommand(
      IntermediateInstruction(LineInterval(0, 1),
                              {ZP("label1"), ZP("label2"), ZP("label3")},
                              ZP("mov"),
                              {ZP("eax")},
                              {ZP("eax")}),
      annotator);
  ir.insertCommand(
      MacroDirective(LineInterval(2, 2), {}, ZP(".macro"), ZP("test"), {}),
      annotator);
  ir.insertCommand(IntermediateInstruction(LineInterval(3, 3),
                                           {ZP("label1")},
                                           ZP("add"),
                                           {ZP("eax")},
                                           {ZP("eax")}),
                   annotator);
  ir.insertCommand(MacroEndDirective(LineInterval(4, 4), {}, ZP(".endmacro")),
                   annotator);
  // To be extended in a later branch.
}
