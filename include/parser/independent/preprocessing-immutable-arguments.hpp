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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_PREPROCESSING_IMMUTABLE_ARGUMENTS_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_PREPROCESSING_IMMUTABLE_ARGUMENTS_HPP

#include "arch/common/architecture.hpp"
#include "parser/independent/syntax-tree-generator.hpp"

/**
 * A collection of some constant parameters which can be used during the
 * 'allocateMemory' and 'preprocess' operation of the intermediate operation.
 */
class PreprocessingImmutableArguments {
 public:
  /**
   * Creates a new PreprocessingImmutableArguments with the given
   * parameters.
   *
   * \param architecture The architecture with which the operation was called.
   * \param generator The generator for syntax tree nodes.
   */
  PreprocessingImmutableArguments(const Architecture& architecture,
                                  const SyntaxTreeGenerator& generator);

  /**
    * \return The architecture with which the operation was called.
    */
  const Architecture& architecture() const noexcept;

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
   * The memory allocator used to convert relative to absolute positions
   * and also for different section management.
   */
  SyntaxTreeGenerator _generator;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_PREPROCESSING_IMMUTABLE_ARGUMENTS_HPP */
