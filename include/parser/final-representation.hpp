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

#ifndef ERAGPSIM_PARSER_FINAL_REPRESENTATION_HPP
#define ERAGPSIM_PARSER_FINAL_REPRESENTATION_HPP

#include <memory>
#include <unordered_map>
#include <vector>
#include "arch/common/abstract-syntax-tree-node.hpp"
#include "parser/compile-error.hpp"
#include "parser/line-interval.hpp"

using MemoryAddress = std::size_t;

/**
 * \brief Denotes the temporary output of an IntermediateRepresentator ready to
 * be used by the architecture.
 */
struct FinalCommand {
  /**
   * \brief A pointer to the syntax tree node which carries the data of this
   * instruction.
   */
  std::unique_ptr<AbstractSyntaxTreeNode> node;

  /**
   * \brief Describes the interval of lines where this command occurs in the
   * plaintext.
   */
  LineInterval position;

  /**
   * \brief Describes the address of the command in memory.
   */
  MemoryAddress address;
};


/**
 * \brief Denotes the temporary output of an IntermediateRepresentator ready to
 * be used by the architecture.
 */
struct FinalRepresentation {
  using AddressMapping = std::unordered_map<MemoryAddress, std::size_t>;

  /**
   * @return True if the error list is non-empty, else false.
   */
  bool hasErrors() const noexcept;

  /**
   * \brief The list of commands which have been assembled.
   */
  std::vector<FinalCommand> commandList;

  /**
   * \brief The list of errors which occurred during the assemblation process.
   */
  std::vector<CompileError> errorList;

  /**
   * \brief Creates a mapping from memory address to instruction index.
   * \return The mapping.
   */
  AddressMapping createMapping() const;
};

#endif
