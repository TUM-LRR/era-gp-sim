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

#include "parser/symbol.hpp"
#include "gtest/gtest.h"
#include "parser/compile-error-list.hpp"
#include "parser/compile-error-list.hpp"
#include "parser/positioned-string.hpp"
#include "parser/symbol-graph-evaluation.hpp"
#include "parser/symbol-graph.hpp"
#include "parser/symbol-replacer.hpp"
#define ZP(x) PositionedString(x, CodePositionInterval())

TEST(SymbolTable, empty) {
  CompileErrorList errors;
  SymbolGraph graph;
  auto eval = graph.evaluate();
  ASSERT_TRUE(eval.valid());
}

TEST(SymbolTable, evaluateSimple) {
  CompileErrorList errors;
  SymbolGraph graph;
  graph.addNode(Symbol(ZP("Hi"), ZP("Bye")));
  auto eval = graph.evaluate();
  ASSERT_TRUE(eval.valid());
}

TEST(SymbolTable, replaceSimple) {
  CompileErrorList errors;
  SymbolGraph graph;
  graph.addNode(Symbol(ZP("A"), ZP("B")));
  graph.addNode(Symbol(ZP("B"), ZP("C")));
  graph.addNode(Symbol(ZP("C"), ZP("D")));
  graph.addNode(Symbol(ZP("E"), ZP("C")));
  auto eval = graph.evaluate();
  ASSERT_TRUE(eval.valid());
  SymbolReplacer replacer(eval);
  auto result =
      replacer.replace(ZP("A+B+C+c=E-D+X-ABCDEFG-124*12B32#E"), errors);
  ASSERT_EQ(result.string(), "D+D+D+c=D-D+X-ABCDEFG-124*12B32#D");
}

TEST(SymbolTable, simpleCycle) {
  CompileErrorList errors;
  SymbolGraph graph;
  graph.addNode(Symbol(ZP("A"), ZP("B")));
  graph.addNode(Symbol(ZP("B"), ZP("C")));
  graph.addNode(Symbol(ZP("C"), ZP("D")));
  graph.addNode(Symbol(ZP("D"), ZP("A")));
  auto eval = graph.evaluate();
  ASSERT_FALSE(eval.valid());
  ASSERT_FALSE(eval.sampleCycle().empty());
  ASSERT_EQ(eval.sampleCycle().size(), 5);
}

TEST(SymbolTable, doubleInsertion) {
  CompileErrorList errors;
  SymbolGraph graph;
  graph.addNode(Symbol(ZP("A"), ZP("B")));
  graph.addNode(Symbol(ZP("A"), ZP("C")));
  graph.addNode(Symbol(ZP("C"), ZP("D")));
  graph.addNode(Symbol(ZP("C"), ZP("D")));
  graph.addNode(Symbol(ZP("C"), ZP("D")));
  graph.addNode(Symbol(ZP("D"), ZP("A")));
  auto eval = graph.evaluate();
  ASSERT_FALSE(eval.valid());
  ASSERT_FALSE(eval.duplicates().empty());
  ASSERT_EQ(eval.duplicates().size(), 2);
}

TEST(SymbolTable, correctName) {
  CompileErrorList errors;
  SymbolGraph graph;
  graph.addNode(Symbol(ZP("a"), ZP("")));
  graph.addNode(Symbol(ZP("_"), ZP("")));
  graph.addNode(Symbol(ZP("hai"), ZP("")));
  graph.addNode(Symbol(ZP("_someVariable"), ZP("")));
  graph.addNode(Symbol(ZP("LONG"), ZP("")));
  graph.addNode(Symbol(ZP("VERY_LONG"), ZP("")));
  graph.addNode(Symbol(ZP("capslock"), ZP("")));
  auto eval = graph.evaluate();
  ASSERT_TRUE(eval.valid());
}

TEST(SymbolTable, invalidName) {
  CompileErrorList errors;
  SymbolGraph graph;
  graph.addNode(Symbol(ZP("1A"), ZP("")));
  graph.addNode(Symbol(ZP("1_2"), ZP("")));
  graph.addNode(Symbol(ZP("#"), ZP("")));
  graph.addNode(Symbol(ZP("_---_"), ZP("")));
  graph.addNode(Symbol(ZP("-.-"), ZP("")));
  graph.addNode(Symbol(ZP("🅱🅻🅾🆇🆇"), ZP("")));
  graph.addNode(Symbol(ZP(""), ZP("")));
  auto eval = graph.evaluate();
  ASSERT_FALSE(eval.valid());
  ASSERT_EQ(eval.invalidNames().size(), 7);
}

TEST(SymbolTable, recursiveReplace) {
  CompileErrorList errors;
  SymbolGraph graph;
  graph.addNode(Symbol(ZP("A"), ZP("(B C D 0)")));
  graph.addNode(Symbol(ZP("B"), ZP("(C C 1)")));
  graph.addNode(Symbol(ZP("C"), ZP("(D D D 2)")));
  graph.addNode(Symbol(ZP("D"), ZP("(E 3)")));
  graph.addNode(Symbol(ZP("E"), ZP("(X 4)")));
  graph.addNode(Symbol(ZP("F"), ZP("(A B C D E X 5)")));
  auto eval = graph.evaluate();
  ASSERT_TRUE(eval.valid());
  SymbolReplacer replacer(eval);
  auto result = replacer.replace(ZP("F ABCDEFG HIJKLMNOP A B F"), errors);
  ASSERT_EQ(result.string(),
            "((((((X 4) 3) ((X 4) 3) ((X 4) 3) 2) (((X 4) 3) ((X 4) 3) ((X 4) "
            "3) 2) 1) (((X 4) 3) ((X 4) 3) ((X 4) 3) 2) ((X 4) 3) 0) ((((X 4) "
            "3) ((X 4) 3) ((X 4) 3) 2) (((X 4) 3) ((X 4) 3) ((X 4) 3) 2) 1) "
            "(((X 4) 3) ((X 4) 3) ((X 4) 3) 2) ((X 4) 3) (X 4) X 5) ABCDEFG "
            "HIJKLMNOP (((((X 4) 3) ((X 4) 3) ((X 4) 3) 2) (((X 4) 3) ((X 4) "
            "3) ((X 4) 3) 2) 1) (((X 4) 3) ((X 4) 3) ((X 4) 3) 2) ((X 4) 3) 0) "
            "((((X 4) 3) ((X 4) 3) ((X 4) 3) 2) (((X 4) 3) ((X 4) 3) ((X 4) 3) "
            "2) 1) ((((((X 4) 3) ((X 4) 3) ((X 4) 3) 2) (((X 4) 3) ((X 4) 3) "
            "((X 4) 3) 2) 1) (((X 4) 3) ((X 4) 3) ((X 4) 3) 2) ((X 4) 3) 0) "
            "((((X 4) 3) ((X 4) 3) ((X 4) 3) 2) (((X 4) 3) ((X 4) 3) ((X 4) 3) "
            "2) 1) (((X 4) 3) ((X 4) 3) ((X 4) 3) 2) ((X 4) 3) (X 4) X 5)");
}

TEST(SymbolTable, lessSimpleCycle) {
  CompileErrorList errors;
  SymbolGraph graph;
  graph.addNode(Symbol(ZP("A"), ZP("A B C D E F G H I J K")));
  graph.addNode(Symbol(ZP("B"), ZP("B C D E F")));
  graph.addNode(Symbol(ZP("C"), ZP("D")));
  graph.addNode(Symbol(ZP("D"), ZP("E F G H J K")));
  graph.addNode(Symbol(ZP("E"), ZP("F G")));
  graph.addNode(Symbol(ZP("F"), ZP("G")));
  graph.addNode(Symbol(ZP("G"), ZP("B H J K")));
  graph.addNode(Symbol(ZP("H"), ZP("")));
  graph.addNode(Symbol(ZP("I"), ZP("J K")));
  graph.addNode(Symbol(ZP("J"), ZP("")));
  graph.addNode(Symbol(ZP("K"), ZP("A")));
  auto eval = graph.evaluate();
  ASSERT_FALSE(eval.valid());
  ASSERT_FALSE(eval.sampleCycle().empty());
}
