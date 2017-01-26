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

#ifndef ERASIM_PARSER_INDEPENDENT_MEMORY_RESERVATION_DIRECTIVE_HPP
#define ERASIM_PARSER_INDEPENDENT_MEMORY_RESERVATION_DIRECTIVE_HPP

#include <cstddef>
#include <functional>

#include "parser/independent/intermediate-directive.hpp"
#include "parser/independent/relative-memory-position.hpp"

class MemoryAllocator;
class CompileErrorList;
class CodePositionInterval;
class SyntaxTreeGenerator;
class Architecture;
class SymbolReplacer;
class FinalRepresentation;
class AllocateMemoryImmutableArguments;

class MemoryReservationDirective : public IntermediateDirective {
 public:
  using ArgumentCompileFunction = std::function<std::size_t(
      const PositionedString&, const SymbolReplacer&, CompileErrorList&)>;
  /**
 * Instantiates a new IntermediateDirective with the given arguments.
 * (only for subclass use!)
 *
 * \param positionInterval The line interval the operation occupies.
 * \param labels The vector of labels assigned to the operation.
 * \param name The name of the operation.
 */
  MemoryReservationDirective(const CodePositionInterval& positionInterval,
                             const PositionedStringVector& labels,
                             const PositionedString& name,
                             const PositionedStringVector& values,
                             const ArgumentCompileFunction& argumentCompile,
                             std::size_t cellSize);

  /**
   * Executes the reservation directive by inserting the data into
   * memory.
   *
   * \param immutable Some constant arguments which might be helpful.
   * \param errors The compile error list to note down any errors.
   * \param commandOutput The final command output vector to record all
   * finalized commands.
   * \param memoryAccess The memory access used to reserve memory and validate
   * instructions.
   */
  virtual void execute(const ExecuteImmutableArguments& immutable,
                       CompileErrorList& errors,
                       FinalCommandVector& commandOutput,
                       MemoryAccess& memoryAccess);

  /**
   * Reserves memory for this operation (but does not write it yet!) (if
   * needed).
   *
   * \param immutable Some constant arguments which might be helpful.
   * \param errors The compile error list to note down any errors.
   * \param allocator The allocator to reserve memory.
   * \param tracker The section tracker so we know in which section to reserve
   * our data.
   */
  virtual void allocateMemory(const AllocateMemoryImmutableArguments& immutable,
                              CompileErrorList& errors,
                              MemoryAllocator& allocator,
                              SectionTracker& tracker);

  /**
   * Reserves entries for this operation in the symbol table, inserts
   * labels.
   *
   * \param immutable Some constant arguments which might be helpful.
   * \param errors The compile error list to note down any errors.
   * \param graph The symbol graph for taking care of symbols (to check their
   * dependencies).
   */
  virtual void
  enhanceSymbolTable(const EnhanceSymbolTableImmutableArguments& immutable,
                     CompileErrorList& errors,
                     SymbolGraph& graph);

  /**
  * \return The absolute memory position where the memory will be reserved.
  */
  MemoryAddress absolutePosition() const noexcept;

  /**
   * \return The raw string values to reserve.
   */
  const PositionedStringVector& values() const noexcept;

  /**
     * Finalizes a memory reservation directive.
     */
  virtual ~MemoryReservationDirective() = default;

 private:
  /**
 * The absolute memory position where the memory will be reserved.
 */
  MemoryAddress _absolutePosition;

  /**
* The section-relative memory position where the memory will be reserved.
*/
  RelativeMemoryPosition _relativePosition;

  /**
   * The requested amount which shall be reserved by this instruction in bits.
   */
  std::size_t _size;

  /**
   * The cell size for this memory reservation in bytes.
   */
  std::size_t _cellSize;

  /**
   * The raw string values to reserve.
   */
  PositionedStringVector _values;

  /**
   * A function to compile the arguments for this instruction (string values).
   */
  ArgumentCompileFunction _argumentCompile;
};

#endif /* ERASIM_PARSER_INDEPENDENT_MEMORY_RESERVATION_DIRECTIVE_HPP */
