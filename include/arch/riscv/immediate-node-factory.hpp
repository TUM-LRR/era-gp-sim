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

#ifndef ERAGPSIM_ARCH_RISCV_IMMEDIATE_NODE_FACTORY_HPP
#define ERAGPSIM_ARCH_RISCV_IMMEDIATE_NODE_FACTORY_HPP

#include <unordered_map>

#include "arch/common/abstract-immediate-node-factory.hpp"

namespace riscv {

/**
 * \brief The ImmediateNodeFactory class is a concrete implementation of
 * AbstractImmediateNodeFactory for the RISC-V architecture
 */
class ImmediateNodeFactory : public AbstractImmediateNodeFactory {
 public:
  /**
   * \brief ImmediateNodeFactory
   * Creates a Immediate Node Factory for the RISC-V architecture
   */
  ImmediateNodeFactory() = default;

  /*! Default constructed copy constructor */
  ImmediateNodeFactory(ImmediateNodeFactory &copy) = default;

  /*! Default constructed move constructor */
  ImmediateNodeFactory(ImmediateNodeFactory &&move) = default;

  ~ImmediateNodeFactory() = default;

  /**
   * \brief createImmediateNode
   * Creates and returns an immediate node from the given MemoryValue. If the
   * given input is invalid, a nullptr is returned
   * \param v MemoryValue, which contains the numerical value of the immediate
   * to be created
   * \return std::unique_ptr pointing to the newly created immediate node, or
   * nullptr if the given MemoryValue is invalid
   */
  virtual Node createImmediateNode(const MemoryValue &value) const override;
};
}
#endif /* ERAGPSIM_ARCH_RISCV_IMMEDIATE_NODE_FACTORY_HPP */
