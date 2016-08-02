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

#ifndef ERAGPSIM_PARSER_FINAL_REPRESENTATION_HPP_
#define ERAGPSIM_PARSER_FINAL_REPRESENTATION_HPP_

#include <memory>
#include <vector>
#include "arch/common/abstract-syntax-tree-node.hpp"
#include "parser/compile-error.hpp"

using LineInterval = std::pair<unsigned int, unsigned int>;

/**
 * \class FinalCommand
 * \brief Denotes the temporary output of an IntermediateRepresentator ready to
 * be used by the architecture.
 */
struct FinalCommand {
  std::unique_ptr<AbstractSyntaxTreeNode> node;
  LineInterval position;
};


/**
 * \brief Denotes the temporary output of an IntermediateRepresentator ready to
 * be used by the architecture.
 */
struct FinalRepresentation {
  std::vector<FinalCommand> commandList;
  std::vector<CompileError> errorList;
};

#endif