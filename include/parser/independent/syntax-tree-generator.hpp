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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_SYNTAX_TREE_GENERATOR_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_SYNTAX_TREE_GENERATOR_HPP

#include <functional>
#include <memory>
#include <vector>

#include "arch/common/node-factory-collection.hpp"
#include "parser/common/final-command.hpp"
#include "parser/independent/positioned-string.hpp"

class CompileErrorList;
class AbstractInstructionNode;
class MemoryAccess;
class SymbolReplacer;

/**
 * A pointer to an abstract syntax tree node.
 */
using AbstractSyntaxTreeNodePointer = std::shared_ptr<AbstractSyntaxTreeNode>;

/**
 * A vector of abstract syntax tree node pointers.
 */
using AbstractSyntaxTreeNodePointerVector =
    std::vector<AbstractSyntaxTreeNodePointer>;

/**
 * A connector class for turning arguments and commands into syntax tree
 * nodes.
 */
class SyntaxTreeGenerator {
 public:
  /**
   * A function type for transforming a string into a syntax tree node.
   */
  using ArgumentNodeGenerator =
      std::function<AbstractSyntaxTreeNodePointer(const PositionedString&,
                                                  const SymbolReplacer&,
                                                  const NodeFactoryCollection&,
                                                  CompileErrorList&)>;

  /**
   * Creates a new syntax tree generator with the given node factory
   * collection.
   *
   * \param nodeFactories The node factory collection to instantiate the nodes
   * from.
   * \param argumentGenerator The generator function for operands.
   */
  SyntaxTreeGenerator(const NodeFactoryCollection& nodeFactories,
                      const ArgumentNodeGenerator& argumentGenerator)
  : _nodeFactories(nodeFactories), _argumentGenerator(argumentGenerator) {
  }

  /**
   * Transforms the given operand as string into a syntax tree node and
   * writes down any occuring errors.
   *
   * \param operand The operand in pure-string form.
   * \param replacer The symbol replacer to replace any occuring symbols in the
   * operand
   * strings.
   * \param errors The compile error list to denote errors.
   * \return The transformed operand.
   */
  AbstractSyntaxTreeNodePointer
  transformOperand(const PositionedString& operand,
                   const SymbolReplacer& replacer,
                   CompileErrorList& errors) const;

  /**
   * Transforms the given instruction/command into a syntax tree, adds
   * any source and target nodes and denotes any occuring errors.
   *
   * \param commandName The opcode of the instruction.
   * \param sources The source arguments.
   * \param targets The target arguments.
   * \param errors The compile error list to denote errors.
   * \param memoryAccess The memory access to check the validity of the nodes.
   * \return The transformed command.
   */
  FinalCommandNodePointer
  transformCommand(const PositionedString& commandName,
                   const AbstractSyntaxTreeNodePointerVector& sources,
                   const AbstractSyntaxTreeNodePointerVector& targets,
                   CompileErrorList& errors,
                   MemoryAccess& memoryAccess) const;

  /**
   * \return a reference to the NodeFactoryCollection used by this
   * SyntaxTreeGenerator
   */
  const NodeFactoryCollection& getNodeFactories() const;

 private:
  /**
   * The internal storage of the node factory collection.
   */
  NodeFactoryCollection _nodeFactories;

  /**
   * The internal (architecture-dependent) function to generate the
   * argument nodes.
   *
   * (can be easily switched at generation of this class so that this class is
   * independent from the architecture)
   */
  ArgumentNodeGenerator _argumentGenerator;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_SYNTAX_TREE_GENERATOR_HPP */
