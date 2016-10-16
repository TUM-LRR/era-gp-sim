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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef ERAGPSIM_TESTS_ARCH_RISCV_DUMMIES_HPP_
#define ERAGPSIM_TESTS_ARCH_RISCV_DUMMIES_HPP_

#include <cstdint>
#include <iostream>

#include "arch/common/abstract-syntax-tree-node.hpp"
#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/register-node.hpp"
#include "arch/riscv/instruction-node-factory.hpp"
#include "arch/riscv/instruction-node.hpp"

#include "core/conversions.hpp"

using namespace riscv;

MemoryValue to32BitMemoryValue(uint32_t value);

MemoryValue to64BitMemoryValue(uint64_t value);

InstructionNodeFactory
setUpFactory(ArchitectureFormula::InitializerList modules);

#endif /* ERAGPSIM_TESTS_ARCH_RISCV_DUMMIES_HPP_*/
