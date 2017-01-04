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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_TRANSFORMATION_PARAMETERS_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_TRANSFORMATION_PARAMETERS_HPP

#include "arch/common/architecture.hpp"
#include "parser/independent/memory-allocator.hpp"
#include "parser/independent/syntax-tree-generator.hpp"

/**
 * A collection of some constant parameters which are used during
 * transforming the command sequence to a FinalRepresentation with an
 * ImmediateRepresentator.
 */
class TransformationParameters {
 public:
  /**
   * Creates a new ExecuteImmutableArguments with the given parameters.
   *
   * \param architecture The architecture with which the operation was called.
   * \param allocator The memory allocator passed for section layout information
   * (must be empty).
   * \param generator The generator for syntax tree nodes.
   */
  TransformationParameters(const Architecture& architecture,
                           const MemoryAllocator& allocator,
                           const SyntaxTreeGenerator& generator);

  /**
   * \return The architecture with which the operation was called.
   */
  const Architecture& architecture() const noexcept;

  /**
   * \return The memory allocator passed for section layout information.
   */
  const MemoryAllocator& allocator() const noexcept;

  /**
   * \return The generator for syntax tree nodes.
   */
  const SyntaxTreeGenerator& generator() const noexcept;

 private:
  /**
   * The architecture with which the operation was called.
   */
  Architecture _architecture;

  /**
   * The memory allocator passed for section layout information.
   */
  MemoryAllocator _allocator;

  /**
   * The generator for syntax tree nodes.
   */
  SyntaxTreeGenerator _generator;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_TRANSFORMATION_PARAMETERS_HPP */
