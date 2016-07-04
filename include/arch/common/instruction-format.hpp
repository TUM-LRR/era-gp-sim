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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*/


/*
 * Format is an abstract class, from which all the specific formats inherit.
 * It should be a friend class of the AbstractSyntaxNode class in order to have
 * access to the child nodes of the instruction node.
 */
struct Format {
  /*
   * @param key Has the information about the bit representation of the opcode
   * function bits.
   * @param node This gives us access to the arguments in the instruction.
   * @return Bit representation of the instruction.
   */
  virtual std::vector<bool>
  operator()(const InstructionInformation::InstructionKey& key,
             const InstructionNode& node) = 0;
};
