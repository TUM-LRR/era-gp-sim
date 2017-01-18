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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_ALLOCATE_MEMORY_IMMUTABLE_ARGUMENTS_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_ALLOCATE_MEMORY_IMMUTABLE_ARGUMENTS_HPP

#include "arch/common/architecture.hpp"
#include "parser/independent/symbol-replacer.hpp"
#include "parser/independent/syntax-tree-generator.hpp"

class PrecompileImmutableArguments;

/**
 * A collection of some constant parameters which can be used during the
 * 'allocateMemory' operation of the intermediate operation.
 */
class AllocateMemoryImmutableArguments {
 public:
  /**
   * Creates a new AllocateMemoryImmutableArguments with the given
   * parameters.
   *
   * \param architecture The architecture with which the operation was called.
   * \param generator The generator for syntax tree nodes.
   * \param preliminaryReplacer The preliminaryReplacer to replace constants,
   * not labels.
   */
  AllocateMemoryImmutableArguments(const Architecture& architecture,
                                   const SyntaxTreeGenerator& generator,
                                   const SymbolReplacer& preliminaryReplacer);

  /**
   * Creates a new AllocateMemoryImmutableArguments with the given parameters
   * out of some PrecompileImmutableArguments.
   *
   * \param beforeBeforePass The PrecompileImmutableArguments these arguments
   * are based on.
   * \param preliminaryReplacer The preliminaryReplacer to replace constants,
   * not labels.
   */
  AllocateMemoryImmutableArguments(
      const PrecompileImmutableArguments& beforePass,
      const SymbolReplacer& preliminaryReplacer);

  /**
   * \return The architecture with which the operation was called.
   */
  const Architecture& architecture() const noexcept;

  /**
   * \return The generator for syntax tree nodes.
   */
  const SyntaxTreeGenerator& generator() const noexcept;

  /**
   * \return The preliminary replacer for constants, not labels.
   */
  const SymbolReplacer& preliminaryReplacer() const noexcept;

 private:
  /**
   * The architecture with which the operation was called.
   */
  Architecture _architecture;

  /**
   * The generator for syntax tree nodes.
   */
  SyntaxTreeGenerator _generator;

  /**
   * The preliminary replacer for constants, not labels.
   */
  SymbolReplacer _preliminaryReplacer;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_ALLOCATE_MEMORY_IMMUTABLE_ARGUMENTS_HPP \
        * \                                                                   \
          */
