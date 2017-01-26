/*
* ERASIM Assembler Interpreter
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

#ifndef ERASIM_PARSER_INDEPENDENT_EXECUTE_IMMUTABLE_ARGUMENTS_HPP
#define ERASIM_PARSER_INDEPENDENT_EXECUTE_IMMUTABLE_ARGUMENTS_HPP

#include "arch/common/architecture.hpp"
#include "parser/independent/memory-allocator.hpp"
#include "parser/independent/symbol-replacer.hpp"
#include "parser/independent/syntax-tree-generator.hpp"

class PrecompileImmutableArguments;
class AllocateMemoryImmutableArguments;
class EnhanceSymbolTableImmutableArguments;

/**
 * A collection of some constant parameters which can be used during the
 * 'execute' operation of the intermediate operation.
 */
class ExecuteImmutableArguments {
 public:
  /**
   * Creates a new ExecuteImmutableArguments with the given parameters.
   *
   * \param architecture The architecture with which the operation was called.
   * \param generator The generator for syntax tree nodes.
   * \param allocator The memory allocator used to convert relative to absolute
   * positions and also for different section management.
   * \param replacer The replacer for constants, labels etc.
   */
  ExecuteImmutableArguments(const Architecture& architecture,
                            const SyntaxTreeGenerator& generator,
                            const MemoryAllocator& allocator,
                            const SymbolReplacer& replacer);

  /**
   * Creates a new ExecuteImmutableArguments with the given parameters
   * out of some PrecompileImmutableArguments.
   *
   * \param beforeBeforePass The PrecompileImmutableArguments these arguments
   * are based on.
   * \param allocator The memory allocator used to convert relative to absolute
   * positions and also for different section management.
   * \param replacer The replacer for constants, labels etc.
   */
  ExecuteImmutableArguments(
      const PrecompileImmutableArguments& beforeBeforeBeforePass,
      const MemoryAllocator& allocator,
      const SymbolReplacer& replacer);

  /**
   * Creates a new ExecuteImmutableArguments with the given parameters
   * out of some AllocateMemoryImmutableArguments.
   *
   * \param beforeBeforePass The AllocateMemoryImmutableArguments these
   * arguments
   * are based on.
   * \param allocator The memory allocator used to convert relative to absolute
   * positions and also for different section management.
   * \param replacer The replacer for constants, labels etc.
   */
  ExecuteImmutableArguments(
      const AllocateMemoryImmutableArguments& beforeBeforePass,
      const MemoryAllocator& allocator,
      const SymbolReplacer& replacer);

  /**
   * Creates a new ExecuteImmutableArguments with the given parameters
   * out of some EnhanceSymbolTableImmutableArguments.
   *
   * \param beforePass The EnhanceSymbolTableImmutableArguments these arguments
   * are based on.
   * \param replacer The replacer for constants, labels etc.
   */
  ExecuteImmutableArguments(
      const EnhanceSymbolTableImmutableArguments& beforePass,
      const SymbolReplacer& replacer);

  /**
   * \return The architecture with which the operation was called.
   */
  const Architecture& architecture() const noexcept;

  /**
   * \return The generator for syntax tree nodes.
   */
  const SyntaxTreeGenerator& generator() const noexcept;

  /**
   * \return The memory allocator used to convert relative to absolute positions
   * and also for different section management.
   */
  const MemoryAllocator& allocator() const noexcept;

  /**
   * \return The replacer for constants, labels etc.
   */
  const SymbolReplacer& replacer() const noexcept;

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
   * The memory allocator used to convert relative to absolute positions
   * and also for different section management.
   */
  MemoryAllocator _allocator;

  /**
   * The replacer for constants, labels etc.
   */
  SymbolReplacer _replacer;
};

#endif /* ERASIM_PARSER_INDEPENDENT_EXECUTE_IMMUTABLE_ARGUMENTS_HPP */
