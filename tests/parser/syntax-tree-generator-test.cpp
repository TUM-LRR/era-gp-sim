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

#include "parser/syntax-tree-generator.hpp"
#include <memory>
#include <vector>
#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/immediate-node.hpp"
#include "arch/common/node-factory-collection-maker.hpp"
#include "arch/common/register-node.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "gtest/gtest.h"

static SyntaxTreeGenerator buildGenerator() {
  Architecture testArch =
      Architecture::Brew(ArchitectureFormula{"riscv", {"rv32i"}});
  auto factoryCollection = NodeFactoryCollectionMaker::CreateFor(testArch);
  SyntaxTreeGenerator generator(factoryCollection);
  return generator;
}

// based off
// http://stackoverflow.com/questions/500493/c-equivalent-of-instanceof , sorry
// I think I am still a c++ rookie...
template <typename Base, typename Type>
static bool instanceOf(const Type* ptr) {
  return dynamic_cast<const Base*>(ptr) != nullptr;
}

TEST(SyntaxTreeGenerator, init) {
  buildGenerator();
}

// Does not work right now... :(

/*TEST(SyntaxTreeGenerator, instantiateArgumentNumberNode) {
  auto generator = buildGenerator();
  CompileState state;
  auto output = generator.transformOperand("1234", state);
  ASSERT_EQ(state.errorList.size(), 0);
  ASSERT_TRUE((instanceOf<ImmediateNode,
AbstractSyntaxTreeNode>(output.get())));
}

TEST(SyntaxTreeGenerator, instantiateArgumentRegisterNode) {
  auto generator = buildGenerator();
  CompileState state;
  auto output = generator.transformOperand("r18", state);
  ASSERT_EQ(state.errorList.size(), 0);
  ASSERT_TRUE((instanceOf<RegisterNode, AbstractSyntaxTreeNode>(output.get())));
}*/

TEST(SyntaxTreeGenerator, instantiateCommandNode) {
  auto generator = buildGenerator();
  CompileState state;

  /*auto arg1 = generator.transformOperand("r1", state);
  ASSERT_EQ(state.errorList.size(), 0);
  ASSERT_TRUE((instanceOf<RegisterNode, AbstractSyntaxTreeNode>(arg1.get())));

  auto arg2 = generator.transformOperand("r2", state);
  ASSERT_EQ(state.errorList.size(), 0);
  ASSERT_TRUE((instanceOf<RegisterNode, AbstractSyntaxTreeNode>(arg2.get())));*/

  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>> sources;
  // sources.push_back(std::move(arg1));

  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>> targets;
  // targets.push_back(std::move(arg2));

  auto output = generator.transformCommand("add", sources, targets, state);

  // ASSERT_EQ(state.errorList.size(), 0);
  ASSERT_TRUE((instanceOf<riscv::InstructionNode, AbstractSyntaxTreeNode>(
      output.get())));
}
