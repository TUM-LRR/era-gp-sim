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

#ifndef ERAGPSIM_PARSER_INTERMEDIATE_REPRESENTATOR_HPP_
#define ERAGPSIM_PARSER_INTERMEDIATE_REPRESENTATOR_HPP_

#include <memory>
#include <string>
#include <vector>
#include "arch/common/abstract-syntax-tree-node.hpp"
#include "parser/compile-state.hpp"
#include "parser/final-representation.hpp"
#include "parser/intermediate-operation.hpp"

class SyntaxTreeGenerator;

/**
 * \brief Provides methods for collecting and compiling a command list.
 */
class IntermediateRepresentator {
 public:
  /**
   * \brief Instantiates an IntermediateRepresentator with the default values.
   */
  IntermediateRepresentator() = default;

  /**
   * \brief Inserts the given command into the command list.
   * \param command The given command.
   * \tparam T The command type.
   */
  template <typename T>
  void insertCommand(const T& command) {
    _commandList.push_back(std::make_unique<T>(command));
  }

  /**
   * \brief Transforms the commands to a syntax tree list.
   * \param generator A syntax tree generator to transform the instructions into
   * a readable format for the architecture module.
   * \param state The compile state to report errors.
   * \return The list of syntax trees to be interpreted by the architecture.
   */
  FinalRepresentation
  transform(const SyntaxTreeGenerator& generator, CompileState& state);

 private:
  /**
   * \brief The internal command list.
   */
  std::vector<std::unique_ptr<IntermediateOperation>> _commandList;
};

#endif
