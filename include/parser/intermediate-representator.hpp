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

#ifndef ERAGPSIM_PARSER_INTERMEDIATE_REPRESENTATOR_HPP
#define ERAGPSIM_PARSER_INTERMEDIATE_REPRESENTATOR_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "arch/common/abstract-syntax-tree-node.hpp"
#include "parser/compile-state.hpp"
#include "parser/final-representation.hpp"
#include "parser/intermediate-operation.hpp"

class SyntaxTreeGenerator;
class Architecture;

/**
 * \brief Provides methods for collecting and compiling a command list.
 */
class IntermediateRepresentator {
 public:
  /**
   * \brief Instantiates an IntermediateRepresentator with the default values.
   */
  IntermediateRepresentator() : _commandList(), _currentOutput(nullptr) {
  }

  /**
   * \brief Inserts the given command into the command list.
   * \param command The given command.
   * \param state The compile state to save any possible errors.
   * \tparam T The command type.
   */
  template <typename T>
  void insertCommand(T&& command, CompileState& state) {
    // First of all, we create our dear pointer.
    IntermediateOperationPointer pointer =
        std::make_unique<T>(std::move(command));

    // We got to handle the three target selector cases right here.
    if (command.newTarget() == TargetSelector::THIS) {
      // If we want the current command as new target, we set it like so.
      if (_currentOutput) {
        // Nested macros are not supported.
        state.addError("Error, nested macros are not supported.");
      }
      _currentOutput = std::move(pointer);
    } else {
      if (command.newTarget() == TargetSelector::MAIN) {
        // For the main selector, we may also insert the old command (otherwise
        // it and its sub commands might be lost).
        if (!_currentOutput) {
          // Classic bracket forgot to close problem.
          state.addError("The start directive of the macro is missing.");
        }
        internalInsertCommand(std::move(_currentOutput));
      }

      // Finally, we may insert our handed-over command.
      internalInsertCommand(std::move(pointer));
    }
  }

  /**
   * \brief Transforms the commands to a syntax tree list.
   * \param architecture The architecture for any specific memory/instruction
   * information.
   * \param generator A syntax tree generator to transform the instructions into
   * a readable format for the architecture module.
   * \param allocator A memory allocator for possible memory reservations.
   * \param state The compile state to report errors.
   * \param memoryAccess The access to write into the memory.
   * \return The list of syntax trees to be interpreted by the architecture.
   */
  FinalRepresentation transform(const Architecture& architecture,
                                const SyntaxTreeGenerator& generator,
                                MemoryAllocator& allocator,
                                CompileState& state,
                                MemoryAccess& memoryAccess);

 private:
  /**
   * \brief Inserts an operation into the list.
   * \param pointer The operation to insert, as pointer.
   */
  void internalInsertCommand(IntermediateOperationPointer pointer) {
    // Of course, it should be valid and is allowed to be inserted.
    if (pointer && pointer->shouldInsert()) {
      // We got to decide if there is an alternative output.
      if (_currentOutput) {
        _currentOutput->insert(std::move(pointer));
      } else {
        _commandList.push_back(std::move(pointer));
      }
    }
  }

  /**
   * \brief The internal command list.
   */
  std::vector<IntermediateOperationPointer> _commandList;

  /**
   * \brief The current target for operations.
   */
  IntermediateOperationPointer _currentOutput;
};

#endif
