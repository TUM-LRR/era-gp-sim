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
using MemoryAddress = std::size_t;
using FinalCommandNodePointer = std::shared_ptr<AbstractInstructionNode>;

class FinalCommand {
 public:
  FinalCommand(const FinalCommandNodePointer& node,
               const CodePositionInterval& position,
               const MemoryAddress& address);
  const FinalCommandNodePointer& node() const noexcept;
  const CodePositionInterval& position() const noexcept;
  const MemoryAddress& address() const noexcept;

 private:
  FinalCommandNodePointer _node;
  CodePositionInterval _position;
  MemoryAddress _address;
};

using FinalCommandVector = std::vector<FinalCommand>;

#endif /* ERAGPSIM_PARSER_FINAL_COMMAND_HPP */
