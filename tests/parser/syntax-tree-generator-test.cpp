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
#include "arch/riscv/instruction-node.hpp"
#include "arch/riscv/register-node.hpp"
#include "common/utility.hpp"
#include "core/project-module.hpp"
#include "gtest/gtest.h"
#include "gtest/gtest.h"
#include "parser/common/compile-error-list.hpp"
#include "parser/common/compile-error-list.hpp"
#include "parser/independent/positioned-string.hpp"
#include "parser/independent/symbol-replacer.hpp"
#include "parser/independent/syntax-tree-generator.hpp"
#include "parser/riscv/riscv-parser.hpp"
#define ZP(x) PositionedString(x, CodePositionInterval())


static SyntaxTreeGenerator buildGenerator() {
  Architecture testArch =
      Architecture::Brew(ArchitectureFormula{"riscv", {"rv32i"}});
  auto factoryCollection = NodeFactoryCollectionMaker::CreateFor(testArch);
  // For now: code function duplication!
  SyntaxTreeGenerator generator(factoryCollection,
                                RiscvParser::argumentGeneratorFunction);
  return generator;
}

TEST(SyntaxTreeGenerator, init) {
  buildGenerator();
}

TEST(SyntaxTreeGenerator, instantiateArgumentNumberNode) {
  auto generator = buildGenerator();
  CompileErrorList errors;
  auto output =
      generator.transformOperand(ZP("1234"), SymbolReplacer(), errors);
  ASSERT_EQ(errors.size(), 0);
  ASSERT_TRUE((Utility::isInstance<ImmediateNode>(output)));
}

TEST(SyntaxTreeGenerator, instantiateArgumentRegisterNode) {
  auto generator = buildGenerator();
  CompileErrorList errors;
  auto output = generator.transformOperand(ZP("x18"), SymbolReplacer(), errors);
  ASSERT_EQ(errors.size(), 0);
  ASSERT_TRUE((Utility::isInstance<riscv::RegisterNode>(output)));
}

TEST(SyntaxTreeGenerator, instantiateCommandNode) {
  auto generator = buildGenerator();
  CompileErrorList errors;

  auto arg1 = generator.transformOperand(ZP("x1"), SymbolReplacer(), errors);
  ASSERT_EQ(errors.size(), 0);
  ASSERT_TRUE((Utility::isInstance<riscv::RegisterNode>(arg1)));

  auto arg2 = generator.transformOperand(ZP("x1"), SymbolReplacer(), errors);
  ASSERT_EQ(errors.size(), 0);
  ASSERT_TRUE((Utility::isInstance<riscv::RegisterNode>(arg1)));

  auto arg3 = generator.transformOperand(ZP("x2"), SymbolReplacer(), errors);
  ASSERT_EQ(errors.size(), 0);
  ASSERT_TRUE((Utility::isInstance<riscv::RegisterNode>(arg2)));

  std::vector<std::shared_ptr<AbstractSyntaxTreeNode>> sources;
  sources.emplace_back(arg1);
  sources.emplace_back(arg2);

  std::vector<std::shared_ptr<AbstractSyntaxTreeNode>> targets;
  targets.emplace_back(arg3);

  ProjectModule projectModule(
      ArchitectureFormula{"riscv", {"rv32i"}}, 4096, "riscv");
  auto memoryAccess = projectModule.getMemoryAccess();

  auto output = generator.transformCommand(
      ZP("add"), sources, targets, errors, memoryAccess);

  ASSERT_EQ(errors.size(), 0);
  ASSERT_TRUE((Utility::isInstance<riscv::InstructionNode>(output)));
}
