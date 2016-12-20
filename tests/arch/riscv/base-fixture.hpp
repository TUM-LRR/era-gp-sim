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
* along with this program. If not, see <http://www.gnu.org/licenses/>.*/

#include <cstddef>
#include <memory>

#include "gtest/gtest.h"

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/node-factory-collection.hpp"
#include "core/memory-access.hpp"
#include "core/project-module.hpp"

namespace riscv {

struct BaseFixture : public ::testing::Test {
  using InitializerList = ArchitectureFormula::InitializerList;
  using size_t = std::size_t;

  BaseFixture(const InitializerList& modules = {"rv32i"},
              size_t memorySize = 64) {
    loadArchitecture(modules, memorySize);
  }

  void
  loadArchitecture(const InitializerList& modules, size_t memorySize = 64) {
    ArchitectureFormula formula("riscv", modules);
    project = std::make_unique<ProjectModule>(formula, memorySize, "riscv");
    factories = project->getArchitectureAccess()
                    .getArchitecture()
                    .get()
                    .getNodeFactories();
  }

  MemoryAccess& getMemoryAccess() {
    return project->getMemoryAccess();
  }

  Architecture getArchitecture() {
    return project->getArchitectureAccess().getArchitecture().get();
  }

  std::unique_ptr<ProjectModule> project;
  NodeFactoryCollection factories;
};
}
