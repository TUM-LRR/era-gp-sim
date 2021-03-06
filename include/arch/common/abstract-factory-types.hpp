/*
* C++ Assembler Interpreter
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

#ifndef ERAGPSIM_ARCH_COMMON_ABSTRACT_FACTORY_TYPES_HPP
#define ERAGPSIM_ARCH_COMMON_ABSTRACT_FACTORY_TYPES_HPP

#include <memory>

#include "arch/common/architecture.hpp"
#include "arch/common/instruction-set.hpp"

template <typename ImmediateNodeFactoryTemplate,
          typename ArithmeticNodeFactoryTemplate,
          typename MemoryAccessNodeFactoryTemplate,
          typename RegisterNodeFactoryTemplate,
          typename InstructionNodeFactoryTemplate,
          typename DataNodeFactoryTemplate>
struct AbstractFactoryTypes {
  using ImmediateNodeFactoryType    = ImmediateNodeFactoryTemplate;
  using ArithmeticNodeFactoryType   = ArithmeticNodeFactoryTemplate;
  using MemoryAccessNodeFactoryType = MemoryAccessNodeFactoryTemplate;
  using RegisterNodeFactoryType     = RegisterNodeFactoryTemplate;
  using InstructionNodeFactoryType  = InstructionNodeFactoryTemplate;
  using DataNodeFactoryType         = DataNodeFactoryTemplate;

  static auto immediateFactory() {
    return makeFactory<ImmediateNodeFactoryType>();
  }

  static auto arithmeticFactory() {
    return makeFactory<ArithmeticNodeFactoryType>();
  }

  static auto memoryAccessFactory() {
    return makeFactory<MemoryAccessNodeFactoryType>();
  }

  static auto registerFactory(const Architecture& architecture) {
    return makeFactory<RegisterNodeFactoryType>(architecture);
  }

  static auto instructionFactory(const InstructionSet& instructions,
                                 const Architecture& architecture) {
    return makeFactory<InstructionNodeFactoryType>(instructions, architecture);
  }

  static auto dataNodeFactory() {
      return makeFactory<DataNodeFactoryType>();
  }

  template <typename FactoryType,
            typename... Args,
            typename = std::enable_if_t<!std::is_void<FactoryType>::value>>
  static std::unique_ptr<FactoryType> makeFactory(Args&&... args) {
    return std::make_unique<FactoryType>(std::forward<Args>(args)...);
  }

  template <typename FactoryType,
            typename... Args,
            typename = std::enable_if_t<std::is_void<FactoryType>::value>>
  static auto makeFactory(Args&&... args) {
    return nullptr;
  }
};

using NoSuchFactory = void;

#endif /* ERAGPSIM_ARCH_COMMON_ABSTRACT_FACTORY_TYPES_HPP */
