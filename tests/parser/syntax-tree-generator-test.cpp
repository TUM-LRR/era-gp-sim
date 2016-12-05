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

#include <memory>
#include <vector>

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/immediate-node.hpp"
#include "arch/common/node-factory-collection-maker.hpp"
#include "arch/common/register-node.hpp"
#include "arch/riscv/instruction-node.hpp"
#include "core/project-module.hpp"
#include "gtest/gtest.h"
#include "parser/compile-state.hpp"
#include "parser/riscv-parser.hpp"
#include "parser/syntax-tree-generator.hpp"

static SyntaxTreeGenerator buildGenerator() {
  Architecture testArch =
      Architecture::Brew(ArchitectureFormula{"riscv", {"rv32i"}});
  auto factoryCollection = NodeFactoryCollectionMaker::CreateFor(testArch);
  // For now: code function duplication!
  SyntaxTreeGenerator generator(factoryCollection,
                                RiscvParser::argumentGeneratorFunction);
  return generator;
}

template <typename SubType, typename BaseType>
bool isInstance(const std::unique_ptr<BaseType>& ptr) {
  return static_cast<SubType*>(ptr.get()) != nullptr;
}

TEST(SyntaxTreeGenerator, init) {
  buildGenerator();
}

TEST(SyntaxTreeGenerator, instantiateArgumentNumberNode) {
  auto generator = buildGenerator();
  CompileState state;
  auto output = generator.transformOperand("1234", state);
  ASSERT_EQ(state.errorList.size(), 0);
  ASSERT_TRUE((isInstance<ImmediateNode>(output)));
}

TEST(SyntaxTreeGenerator, instantiateArgumentRegisterNode) {
  auto generator = buildGenerator();
  CompileState state;
  auto output = generator.transformOperand("r18", state);
  ASSERT_EQ(state.errorList.size(), 0);
  ASSERT_TRUE((isInstance<RegisterNode>(output)));
}

TEST(SyntaxTreeGenerator, instantiateCommandNode) {
  auto generator = buildGenerator();
  CompileState state;

  auto arg1 = generator.transformOperand("r1", state);
  ASSERT_EQ(state.errorList.size(), 0);
  ASSERT_TRUE((isInstance<RegisterNode>(arg1)));

  auto arg2 = generator.transformOperand("r1", state);
  ASSERT_EQ(state.errorList.size(), 0);
  ASSERT_TRUE((isInstance<RegisterNode>(arg1)));

  auto arg3 = generator.transformOperand("r2", state);
  ASSERT_EQ(state.errorList.size(), 0);
  ASSERT_TRUE((isInstance<RegisterNode>(arg2)));

  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>> sources;
  sources.push_back(std::move(arg1));
  sources.push_back(std::move(arg2));

  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>> targets;
  targets.push_back(std::move(arg3));

  ProjectModule projectModule(
      ArchitectureFormula{"riscv", {"rv32i"}}, 4096, "riscv");
  auto memoryAccess = projectModule.getMemoryAccess();

  auto output =
      generator.transformCommand("add", sources, targets, state, memoryAccess);

  ASSERT_EQ(state.errorList.size(), 0);
  ASSERT_TRUE((isInstance<riscv::InstructionNode>(output)));
}
