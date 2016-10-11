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

#include "parser/symbol-table.hpp"
#include "gtest/gtest.h"

TEST(SymbolTable, initSimple) {
  CompileState state;
  SymbolTable st;
  st.insertEntry(std::string("Hi"), std::string("Bye"), state);
  ASSERT_EQ(state.errorList.size(), 0);
}

TEST(SymbolTable, replaceSimple) {
  CompileState state;
  SymbolTable st;
  st.insertEntry("A", "B", state);
  st.insertEntry("B", "C", state);
  st.insertEntry("C", "D", state);
  st.insertEntry("E", "C", state);
  auto result = st.replaceSymbols("A+B+C+c=E-D+X-ABCDEFG-124*12B32#E", state);
  ASSERT_EQ("D+D+D+c=D-D+X-ABCDEFG-124*12B32#D", result);
  ASSERT_EQ(state.errorList.size(), 0);
}

TEST(SymbolTable, infiniteRecursion) {
  CompileState state;
  SymbolTable st;
  st.insertEntry("A", "B", state);
  st.insertEntry("B", "A", state);
  auto result = st.replaceSymbols("Lorem ipsum sit amet A", state);
  ASSERT_EQ(state.errorList.size(), 1);
  ASSERT_EQ(state.errorList[0].severity(), CompileErrorSeverity::ERROR);
}

TEST(SymbolTable, doubleInsertion) {
  CompileState state;
  SymbolTable st;
  st.insertEntry("A", "B", state);
  st.insertEntry("A", "C", state);
  ASSERT_EQ(state.errorList.size(), 1);
  ASSERT_EQ(state.errorList[0].severity(), CompileErrorSeverity::ERROR);
}

TEST(SymbolTable, correctName) {
  CompileState state;
  SymbolTable st;
  st.insertEntry("a", "", state);
  st.insertEntry("_", "", state);
  st.insertEntry("hai", "", state);
  st.insertEntry("_someVariable", "", state);
  st.insertEntry("LONG", "", state);
  st.insertEntry("VERY_LONG", "", state);
  st.insertEntry("capslock", "", state);
  ASSERT_EQ(state.errorList.size(), 0);
}

TEST(SymbolTable, invalidName) {
  CompileState state;
  SymbolTable st;
  st.insertEntry("1A", "", state);
  st.insertEntry("1_2", "", state);
  st.insertEntry("#", "", state);
  st.insertEntry("_---_", "", state);
  st.insertEntry("-.-", "", state);
  st.insertEntry("🅱🅻🅾🆇🆇", "", state);
  st.insertEntry("", "", state);
  ASSERT_EQ(state.errorList.size(), 7);
  for (auto i : state.errorList) {
    ASSERT_EQ(i.severity(), CompileErrorSeverity::ERROR);
  }
}
