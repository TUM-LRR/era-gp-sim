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
#include "parser/symbol-graph-evaluation.hpp"
#include "parser/symbol-graph.hpp"
#include "parser/symbol-replacer.hpp"

auto POSITION_ZERO = CodePositionInterval(CodePosition(0), CodePosition(0));

TEST(SymbolTable, empty) {
  SymbolGraph graph;
  auto eval = graph.evaluate();
  ASSERT_TRUE(eval.valid());
}

TEST(SymbolTable, evaluateSimple) {
  SymbolGraph graph;
  graph.addNode(Symbol(std::string("Hi"), std::string("Bye"), POSITION_ZERO));
  auto eval = graph.evaluate();
  ASSERT_TRUE(eval.valid());
}

TEST(SymbolTable, replaceSimple) {
  SymbolGraph graph;
  graph.addNode(Symbol("A", "B", POSITION_ZERO));
  graph.addNode(Symbol("B", "C", POSITION_ZERO));
  graph.addNode(Symbol("C", "D", POSITION_ZERO));
  graph.addNode(Symbol("E", "C", POSITION_ZERO));
  auto eval = graph.evaluate();
  ASSERT_TRUE(eval.valid());
  SymbolReplacer replacer(eval);
  auto result = replacer.replace("A+B+C+c=E-D+X-ABCDEFG-124*12B32#E");
  ASSERT_EQ(result, "D+D+D+c=D-D+X-ABCDEFG-124*12B32#D");
}

TEST(SymbolTable, simpleCycle) {
  SymbolGraph graph;
  graph.addNode(Symbol("A", "B", POSITION_ZERO));
  graph.addNode(Symbol("B", "C", POSITION_ZERO));
  graph.addNode(Symbol("C", "D", POSITION_ZERO));
  graph.addNode(Symbol("D", "A", POSITION_ZERO));
  auto eval = graph.evaluate();
  ASSERT_FALSE(eval.valid());
  ASSERT_FALSE(eval.sampleCycle().empty());
  ASSERT_EQ(eval.sampleCycle().size(), 5);
}

TEST(SymbolTable, doubleInsertion) {
  SymbolGraph graph;
  graph.addNode(Symbol("A", "B", POSITION_ZERO));
  graph.addNode(Symbol("A", "C", POSITION_ZERO));
  graph.addNode(Symbol("C", "D", POSITION_ZERO));
  graph.addNode(Symbol("C", "D", POSITION_ZERO));
  graph.addNode(Symbol("C", "D", POSITION_ZERO));
  graph.addNode(Symbol("D", "A", POSITION_ZERO));
  auto eval = graph.evaluate();
  ASSERT_FALSE(eval.valid());
  ASSERT_FALSE(eval.duplicates().empty());
  ASSERT_EQ(eval.duplicates().size(), 2);
}

TEST(SymbolTable, correctName) {
  SymbolGraph graph;
  graph.addNode(Symbol("a", "", POSITION_ZERO));
  graph.addNode(Symbol("_", "", POSITION_ZERO));
  graph.addNode(Symbol("hai", "", POSITION_ZERO));
  graph.addNode(Symbol("_someVariable", "", POSITION_ZERO));
  graph.addNode(Symbol("LONG", "", POSITION_ZERO));
  graph.addNode(Symbol("VERY_LONG", "", POSITION_ZERO));
  graph.addNode(Symbol("capslock", "", POSITION_ZERO));
  auto eval = graph.evaluate();
  ASSERT_TRUE(eval.valid());
}

TEST(SymbolTable, invalidName) {
  SymbolGraph graph;
  graph.addNode(Symbol("1A", "", POSITION_ZERO));
  graph.addNode(Symbol("1_2", "", POSITION_ZERO));
  graph.addNode(Symbol("#", "", POSITION_ZERO));
  graph.addNode(Symbol("_---_", "", POSITION_ZERO));
  graph.addNode(Symbol("-.-", "", POSITION_ZERO));
  graph.addNode(Symbol("🅱🅻🅾🆇🆇", "", POSITION_ZERO));
  graph.addNode(Symbol("", "", POSITION_ZERO));
  auto eval = graph.evaluate();
  ASSERT_FALSE(eval.valid());
  ASSERT_EQ(eval.invalidNames().size(), 7);
}

TEST(SymbolTable, recursiveReplace) {
  SymbolGraph graph;
  graph.addNode(Symbol("A", "(B C D 0)", POSITION_ZERO));
  graph.addNode(Symbol("B", "(C C 1)", POSITION_ZERO));
  graph.addNode(Symbol("C", "(D D D 2)", POSITION_ZERO));
  graph.addNode(Symbol("D", "(E 3)", POSITION_ZERO));
  graph.addNode(Symbol("E", "(X 4)", POSITION_ZERO));
  graph.addNode(Symbol("F", "(A B C D E X 5)", POSITION_ZERO));
  auto eval = graph.evaluate();
  ASSERT_TRUE(eval.valid());
  SymbolReplacer replacer(eval);
  auto result = replacer.replace("F ABCDEFG HIJKLMNOP A B F");
  ASSERT_EQ(result,
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
  SymbolGraph graph;
  graph.addNode(Symbol("A", "A B C D E F G H I J K", POSITION_ZERO));
  graph.addNode(Symbol("B", "B C D E F", POSITION_ZERO));
  graph.addNode(Symbol("C", "D", POSITION_ZERO));
  graph.addNode(Symbol("D", "E F G H J K", POSITION_ZERO));
  graph.addNode(Symbol("E", "F G", POSITION_ZERO));
  graph.addNode(Symbol("F", "G", POSITION_ZERO));
  graph.addNode(Symbol("G", "B H J K", POSITION_ZERO));
  graph.addNode(Symbol("H", "", POSITION_ZERO));
  graph.addNode(Symbol("I", "J K", POSITION_ZERO));
  graph.addNode(Symbol("J", "", POSITION_ZERO));
  graph.addNode(Symbol("K", "A", POSITION_ZERO));
  auto eval = graph.evaluate();
  ASSERT_FALSE(eval.valid());
  ASSERT_FALSE(eval.sampleCycle().empty());
}
