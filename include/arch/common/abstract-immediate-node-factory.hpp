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

#ifndef ERAGPSIM_ARCH_ABSTRACT_IMMEDIATE_NODE_FACTORY_HPP
#define ERAGPSIM_ARCH_ABSTRACT_IMMEDIATE_NODE_FACTORY_HPP

#include <memory>

class AbstractSyntaxTreeNode;
class MemoryValue;

/**
 * \brief The AbstractImmediateNodeFactory class
 * Abstract factory type for creating SyntaxTreeNodes of type immediate
 */
class AbstractImmediateNodeFactory {
 public:
  using Node = std::shared_ptr<AbstractSyntaxTreeNode>;

  virtual ~AbstractImmediateNodeFactory() = default;

  /**
   * \brief createImmediateNode
   * Creates and returns an architecture-specific implementation of a
   * SyntaxTreeNode of type immediate with the given numericalValue. If the
   * architecture-specific implementation cannot create an immediate from the
   * given numericalValue, a nullptr is returned
   *
   * \param numericalValue representation of a number to create the immediate
   * \return std::shared_ptr pointing to the newly created SyntaxTreeNode, or
   * nullptr if the architecture-specific implementation cannot create an
   * immediate from the given numericalValue
   */
  virtual Node createImmediateNode(const MemoryValue &numericalValue) const = 0;

   /**
   * Returns a immediate value for the given label value.
   * Due to the possibility that different instructions handle label values differently, the instruction mnemonic is required.
   * \param labelValue The absolute address where the label is defined
   * \param instructionMnemonic The mnemonic of the instruction whose operand is the current label
   * \param instructionAddress The absolute address of the current instruction
   * \return
   */
  virtual MemoryValue labelToImmediate(const MemoryValue& labelValue, const std::string& instructionMnemonic, const MemoryValue& instructionAddress) const = 0;
};

#endif /* ERAGPSIM_ARCH_ABSTRACT_IMMEDIATE_NODE_FACTORY_HPP */
