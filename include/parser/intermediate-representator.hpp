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
#include <functional>
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
  IntermediateRepresentator()
  : _commandList(), _currentOutput(nullptr)
  {

  }

  /**
   * \brief Inserts the given command into the command list.
   * \param command The given command.
   * \param state The compile state to save any possible errors.
   * \tparam T The command type.
   */
  template <typename T>
  void insertCommand(T&& command, CompileState& state) {
    IntermediateOperationPointer pointer = std::make_unique<T>(std::move(command));
    if (command.newTarget() == TargetSelector::THIS)
    {
      if (_currentOutput)
      {
        state.addError("Error, macro not finished before another should begin.");
      }
      _currentOutput = std::move(pointer);
    }
    else
    {
      if (command.newTarget() == TargetSelector::MAIN)
      {
        if (!_currentOutput)
        {
          state.addError("Macro end without beginning.");
        }
        internalInsertCommand(std::move(_currentOutput));
      }
      internalInsertCommand(std::move(pointer));
    }
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
  void internalInsertCommand(IntermediateOperationPointer pointer)
  {
    if (pointer && pointer->shouldInsert())
    {
      if (_currentOutput)
      {
        _currentOutput->insert(std::move(pointer));
      }
      else
      {
        _commandList.push_back(std::move(pointer));
      }
    }
  }

  /**
   * \brief The internal command list.
   */
  std::vector<IntermediateOperationPointer> _commandList;

  IntermediateOperationPointer _currentOutput;
};

#endif
