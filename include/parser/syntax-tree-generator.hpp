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

#ifndef ERAGPSIM_PARSER_SYNTAX_TREE_GENERATOR_HPP_
#define ERAGPSIM_PARSER_SYNTAX_TREE_GENERATOR_HPP_

#include <functional>
#include <memory>
#include <vector>

#include "arch/common/node-factory-collection.hpp"

class AbstractSyntaxTreeNode;
class CompileState;
class MemoryAccess;

/**
 * \brief A connector class for turning arguments and commands into syntax tree
 * nodes.
 */
class SyntaxTreeGenerator {
 public:
  using ArgumentNodeGenerator =
      std::function<std::unique_ptr<AbstractSyntaxTreeNode>(
          const std::string&, const NodeFactoryCollection&, CompileState&)>;

  /**
   * \brief Creates a new syntax tree generator with the given node factory
   * collection.
   * \param nodeFactories The node factory collection to instantiate the nodes
   * from.
   * \param argumentGenerator The generator function for operands.
   */
  SyntaxTreeGenerator(const NodeFactoryCollection& nodeFactories,
                      const ArgumentNodeGenerator& argumentGenerator)
  : _nodeFactories(nodeFactories), _argumentGenerator(argumentGenerator) {
  }

  /**
   * \brief Transforms the given operand as string into a syntax tree node and
   * writes down any occuring errors.
   * \param operand The operand in pure-string form.
   * \param state The compile state to denote errors and other stuff.
   * \return The transformed operand.
   */
  std::unique_ptr<AbstractSyntaxTreeNode>
  transformOperand(const std::string& operand, CompileState& state) const;

  /**
   * \brief Transforms the given instruction/command into a syntax tree, adds
   * any source and target nodes and denotes any occuring errors.
   * \param command_name The opcode of the instruction.
   * \param sources The source arguments.
   * \param targets The target arguments.
   * \param state The compile state to denote errors.
   * \return The transformed command.
   */
  std::unique_ptr<AbstractSyntaxTreeNode> transformCommand(
      const std::string& command_name,
      std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>& sources,
      std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>& targets,
      CompileState& state,
      MemoryAccess& memoryAccess) const;

 private:
  /**
   * \brief The internal storage of the node factory collection.
   */
  NodeFactoryCollection _nodeFactories;

  ArgumentNodeGenerator _argumentGenerator;
};

#endif /* ERAGPSIM_PARSER_SYNTAX_TREE_GENERATOR_HPP_ */
