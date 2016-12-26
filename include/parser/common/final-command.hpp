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

#ifndef ERAGPSIM_PARSER_FINAL_COMMAND_HPP
#define ERAGPSIM_PARSER_FINAL_COMMAND_HPP

#include <memory>
#include <vector>
#include "parser/common/code-position-interval.hpp"
class AbstractInstructionNode;

/**
 * \brief Helper definition for memory address integer sizes.
 */
using MemoryAddress = std::size_t;

/**
 * \brief A pointer on a final instruction node sent to the code and the gui.
 */
using FinalCommandNodePointer = std::shared_ptr<AbstractInstructionNode>;

/**
 * \brief A finalized command with display information ready to be executed.
 */
class FinalCommand {
 public:
  /**
   * \brief Creates a final command with the given parameters.
   * \param node The pointer to the syntax tree node containing the instruction
   * data.
   * \param position The position in code where the interval is located.
   * \param address The address of the command in memory.
   */
  FinalCommand(const FinalCommandNodePointer& node,
               const CodePositionInterval& position,
               const MemoryAddress& address);

  /**
   * \brief Returns the pointer to the syntax tree node containing the
   * instruction data.
   * \return The pointer to the syntax tree node containing the instruction
   * data.
   */
  const FinalCommandNodePointer& node() const noexcept;

  /**
   * \brief Returns the position in code where the interval is located.
   * \return The position in code where the interval is located.
   */
  const CodePositionInterval& position() const noexcept;

  /**
   * \brief Returns the address of the command in memory.
   * \return The address of the command in memory.
   */
  const MemoryAddress& address() const noexcept;

 private:
  /**
   * \brief Returns the pointer to the syntax tree node containing the
   * instruction data.
   */
  FinalCommandNodePointer _node;

  /**
   * \brief The position in code where the interval is located.
   */
  CodePositionInterval _position;

  /**
   * \brief The address of the command in memory.
   */
  MemoryAddress _address;
};

/**
 * \brief A vector of final commands.
 */
using FinalCommandVector = std::vector<FinalCommand>;

#endif /* ERAGPSIM_PARSER_FINAL_COMMAND_HPP */
