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

#ifndef ERAGPSIM_PARSER_MEMORY_RESERVATION_DIRECTIVE_HPP
#define ERAGPSIM_PARSER_MEMORY_RESERVATION_DIRECTIVE_HPP


#include <functional>
#include "parser/intermediate-directive.hpp"
#include "parser/relative-memory-position.hpp"
class MemoryAllocator;
class CompileErrorAnnotator;
struct LineInterval;
class SymbolTable;
class SyntaxTreeGenerator;
class Architecture;
struct FinalRepresentation;

class MemoryReservationDirective : public IntermediateDirective {
 public:
  using ArgumentCompileFunction = std::function<std::size_t(
      const std::string&, const CompileErrorAnnotator&)>;
  /**
 * \brief Instantiates a new IntermediateDirective with the given arguments.
 * (only for subclass use!)
 * \param lines The line interval the operation occupies.
 * \param labels The vector of labels assigned to the operation.
 * \param name The name of the operation.
 */
  MemoryReservationDirective(const LineInterval& lines,
                             const std::vector<std::string>& labels,
                             const std::string& name,
                             std::size_t cellSize,
                             const std::vector<std::string>& values,
                             const ArgumentCompileFunction& argumentCompile);

  /**
   * \brief Executes the given operation (somehow).
   * \param finalRepresentator The FinalRepresentation for possible output.
   * \param table The SymbolTable for possible replacements.
   * \param generator The generator to transform the instructions.
   * \param state The CompileState to log possible errors.
   * \param memoryAccess The MemoryAccess for verifying instructions or
   * reserving data.
   */
  virtual void execute(const ExecuteImmutableArguments& immutable,
                       const CompileErrorAnnotator& annotator,
                       FinalRepresentation& finalRepresentator,
                       MemoryAccess& memoryAccess);

  /**
   * \brief Reserves (not writes!) memory for the operation (if needed).
   * \param architecture The architecture for information about the memory
   * format.
   * \param allocator The allocator to reserve memory.
   * \param state The CompileState to log possible errors.
   */
  virtual void allocateMemory(const PreprocessingImmutableArguments& immutable,
                              const CompileErrorAnnotator& annotator,
                              MemoryAllocator& allocator,
                              SectionTracker& tracker);

  /**
   * \brief Enhances the symbol table by the labels of the operation.
   * \param table The SymbolTable to insert into.
   * \param allocator The MemoryAllocator to get the memory positions from.
   * \param state The CompileState to log possible errors.
   */
  virtual void
  enhanceSymbolTable(const EnhanceSymbolTableImmutableArguments& immutable,
                     const CompileErrorAnnotator& annotator,
                     SymbolGraph& graph);

 private:
  ArgumentCompileFunction _argumentCompile;
  std::size_t _absolutePosition;
  RelativeMemoryPosition _relativePosition;
  std::size_t _size;
  std::size_t _cellSize;
  std::vector<std::string> _values;
};

#endif /* ERAGPSIM_PARSER_MEMORY_RESERVATION_DIRECTIVE_HPP */
