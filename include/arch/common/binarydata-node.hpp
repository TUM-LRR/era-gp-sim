/* ERASIM Assembler Interpreter
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
#ifndef ERASIM_ARCH_COMMON_BINARYDATA_NODE_HPP
#define ERASIM_ARCH_COMMON_BINARYDATA_NODE_HPP

#include "arch/common/abstract-syntax-tree-node.hpp"
#include "arch/common/validation-result.hpp"
#include "core/memory-value.hpp"

/**
 * A binary data node stores data that is best stored in binary form, like a
 * text message, string literal etc.
 */
class BinaryDataNode : public AbstractSyntaxTreeNode {
 public:
  using Data = std::string;

  /**
   * Constructs an empty data node, containing no data.
   */
  BinaryDataNode();

  /**
   * Constructs a data node with the given data in string form.
   * \param data a string (or vector<char>) from where the data is copied
   */
  BinaryDataNode(const std::string& data);

  /**
   * \copydoc AbstractSyntaxTreeNode::validate()
   * \param memoryAccess
   * \return success, when this node does not have any child nodes, fail
   * otherwise
   */
  ValidationResult validate(MemoryAccess& memoryAccess) const override;

  /**
   * Transforms the stores data into a MemoryValue and returns the MemoryValue.
   * The size of this MemoryValue depends on the size of stored data and
   * therefore is not suited to be used in an actual assembly format.
   * \return A MemoryValue containing the data
   */
  MemoryValue assemble() const override;

  /**
   * Returns the same MemoryValue as assemble()
   * \param memoryAccess
   * \return
   */
  MemoryValue getValue(MemoryAccess& memoryAccess) const override;

  /**
   * Returns the stored data as string.
   * \return The stored data as string
   */
  const std::string& getIdentifier() const override;

 private:
  Data _rawData;
};

#endif  // ERASIM_ARCH_COMMON_BINARYDATA_NODE_HPP
