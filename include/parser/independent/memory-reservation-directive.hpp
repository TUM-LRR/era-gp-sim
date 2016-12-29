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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_MEMORY_RESERVATION_DIRECTIVE_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_MEMORY_RESERVATION_DIRECTIVE_HPP


#include <functional>
#include "parser/independent/intermediate-directive.hpp"
#include "parser/independent/relative-memory-position.hpp"
class MemoryAllocator;
class CompileErrorList;
class CodePositionInterval;
class SymbolTable;
class SyntaxTreeGenerator;
class Architecture;
class SymbolReplacer;
struct FinalRepresentation;

class MemoryReservationDirective : public IntermediateDirective {
 public:
  using ArgumentCompileFunction = std::function<std::size_t(
      const PositionedString&, const SymbolReplacer&, CompileErrorList&)>;
  /**
 * \brief Instantiates a new IntermediateDirective with the given arguments.
 * (only for subclass use!)
 * \param positionInterval The line interval the operation occupies.
 * \param labels The vector of labels assigned to the operation.
 * \param name The name of the operation.
 */
  MemoryReservationDirective(const CodePositionInterval& positionInterval,
                             const std::vector<PositionedString>& labels,
                             const PositionedString& name,
                             const std::vector<PositionedString>& values,
                             const ArgumentCompileFunction& argumentCompile,
                             std::size_t cellSize);

  /**
   * \brief Executes the reservation directive by inserting the data into
   * memory.
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
                       MemoryAccess& memoryAccess) override;

  /**
   * \brief Reserves memory for this operation (but does not write it yet!) (if
   * needed).
   * \param immutable Some constant arguments which might be helpful.
   * \param errors The compile error list to note down any errors.
   * \param allocator The allocator to reserve memory.
   * \param tracker The section tracker so we know in which section to reserve
   * our data.
   */
  virtual void allocateMemory(const PreprocessingImmutableArguments& immutable,
                              CompileErrorList& errors,
                              MemoryAllocator& allocator,
                              SectionTracker& tracker) override;

  /**
   * \brief Reserves entries for this operation in the symbol table, inserts
   * labels.
   * \param immutable Some constant arguments which might be helpful.
   * \param errors The compile error list to note down any errors.
   * \param graph The symbol graph for taking care of symbols (to check their
   * dependencies).
   */
  virtual void
  enhanceSymbolTable(const EnhanceSymbolTableImmutableArguments& immutable,
                     CompileErrorList& errors,
                     SymbolGraph& graph) override;

  MemoryAddress absolutePosition() const noexcept;
  const std::vector<PositionedString>& values() const noexcept;

  /**
     * \brief Finalizes a memory reservation directive.
     */
  virtual ~MemoryReservationDirective() = default;

 private:
  MemoryAddress _absolutePosition;
  RelativeMemoryPosition _relativePosition;
  std::size_t _size;
  std::size_t _cellSize;
  std::vector<PositionedString> _values;
  ArgumentCompileFunction _argumentCompile;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_MEMORY_RESERVATION_DIRECTIVE_HPP */
