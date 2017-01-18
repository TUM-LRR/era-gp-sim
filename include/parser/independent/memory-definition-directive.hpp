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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_MEMORY_DEFINITION_DIRECTIVE_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_MEMORY_DEFINITION_DIRECTIVE_HPP

#include <cstddef>
#include <functional>

#include "arch/common/architecture.hpp"
#include "core/conversions.hpp"
#include "core/memory-access.hpp"
#include "core/memory-value.hpp"
#include "parser/common/compile-error-list.hpp"
#include "parser/independent/allocate-memory-immutable-arguments.hpp"
#include "parser/independent/enhance-symbol-table-immutable-arguments.hpp"
#include "parser/independent/execute-immutable-arguments.hpp"
#include "parser/independent/expression-compiler-clike.hpp"
#include "parser/independent/intermediate-directive.hpp"
#include "parser/independent/memory-allocator.hpp"
#include "parser/independent/relative-memory-position.hpp"
#include "parser/independent/section-tracker.hpp"
#include "parser/independent/string-parser.hpp"
#include "parser/independent/symbol-graph.hpp"
#include "parser/independent/symbol-replacer.hpp"

/**
 * A directive to reserve memory and writes specified data to it.
 *
 * \tparam T The number type used for parsing.
 */
template <typename T>
class MemoryDefinitionDirective : public IntermediateDirective {
 public:
  using size_t = std::size_t;

  /**
   * The internally used function to store values in memory.
   */
  using MemoryStorageFunction = std::function<void(T, size_t)>;

  /**
   * The function to compile values and put them into memory.
   */
  using ProcessValuesFunction =
      std::function<size_t(const PositionedStringVector&,
                           const SymbolReplacer&,
                           size_t,
                           CompileErrorList&,
                           const MemoryStorageFunction&)>;

  /**
 * Instantiates a new MemoryDefinitionDirective with the given arguments.
 *
 * \param positionInterval The line interval the operation occupies.
 * \param labels The vector of labels assigned to the operation.
 * \param name The name of the operation.
 * \param cellSize The size of the unit how much each argument should occupy at
 * space.
 * \param values The argument vector.
 * \param processValues A function to parse the arguments.
 */
  MemoryDefinitionDirective(const CodePositionInterval& positionInterval,
                            const PositionedStringVector& labels,
                            const PositionedString& name,
                            const PositionedStringVector& values,
                            const ProcessValuesFunction& processValues,
                            size_t cellSize = sizeof(T))
  : IntermediateDirective(positionInterval, labels, name)
  , _cellSize(cellSize)
  , _values(values)
  , _processValues(processValues) {
  }

  /**
   * Reserves memory for this operation (but does not write it).
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
                              SectionTracker& tracker) {
    if (tracker.section() == "text") {
      errors.pushWarning(
          name().positionInterval(),
          "Careful, you are trying to define static memory in the text "
          "section where the program instructions are stored. "
          "This might cause unexpected behavior. Use a '.section "
          "data' directive in front of this to resolve the "
          "issue.");
    }
    if (_values.empty()) {
      errors.pushWarning(name().positionInterval(), "Empty data definition.");
    }

    // So, we simply calculate and sum up our arguments.
    // Let's hope, the compiler optimizes this...
    auto temporary = CompileErrorList();
    size_t sizeInBytes = _processValues(_values,
                                        immutable.preliminaryReplacer(),
                                        _cellSize,
                                        temporary,
                                        [](T value, size_t position) {});

    // Next, we got to allocate our memory.
    _relativePosition =
        allocator[tracker.section()].allocateRelative(sizeInBytes);

    // The bit size is store for further usage.
    _size = sizeInBytes * immutable.architecture().getByteSize();
  }

  /**
   * Reserves entries for this operation in the symbol table, also for
   * the labels for this operation.
   *
   * \param immutable Some constant arguments which might be helpful.
   * \param errors The compile error list to note down any errors.
   * \param graph The symbol graph for taking care of symbols (to check their
   * dependencies).
   */
  virtual void
  enhanceSymbolTable(const EnhanceSymbolTableImmutableArguments& immutable,
                     CompileErrorList& errors,
                     SymbolGraph& graph) {
    _absolutePosition =
        immutable.allocator().absolutePosition(_relativePosition);
    for (const auto& label : labels()) {
      graph.addNode(
          Symbol(label, PositionedString(std::to_string(_absolutePosition))));
    }
  }

  /**
   * Executes the operation (e.g. it is inserted into the commandOutput
   * list).
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
                       MemoryAccess& memoryAccess) {
    if (_size > 0) {
      // We first of all create our memory value locally.
      MemoryValue data(_size);

      auto byteSize = immutable.architecture().getByteSize();
      size_t bitLength = byteSize * _cellSize;

      auto signedRepresentation = conversions::detail::mapRepresentation(
          immutable.architecture().getSignedRepresentation());
      auto conversion =
          conversions::detail::switchConversion(signedRepresentation);

      // Then we write to it.
      _processValues(_values,
                     SymbolReplacer(),
                     _cellSize,
                     errors,
                     [&](T value, std::size_t position) {
                       auto memoryValue =
                           conversions::convert(value, conversion, bitLength);

                       // Once converted, we take down the value.
                       data.write(memoryValue, position * byteSize);
                     });

      // Then, let's do a (probably also here) expensive memory call.
      memoryAccess.putMemoryValueAt(_absolutePosition, data);
    } else {
      errors.pushError(name().positionInterval(),
                       "Nothing to reserve with memory definition.");
    }
  }

  /**
  * \return The absolute memory position where the memory will be defined.
  */
  MemoryAddress absolutePosition() const noexcept {
    return _absolutePosition;
  }

  /**
   * \return The raw string values which shall be written to memory.
   */
  const PositionedStringVector& values() const noexcept {
    return _values;
  }

  /**
   * Finalizes a memory definition directive.
   */
  virtual ~MemoryDefinitionDirective() = default;

 private:
  /**
 * The absolute memory position where the memory will be defined.
 */
  MemoryAddress _absolutePosition;

  /**
* The section-relative memory position where the memory will be defined.
*/
  RelativeMemoryPosition _relativePosition;

  /**
   * The requested amount which shall be reserved by this instruction in bits.
   */
  size_t _size;

  /**
   * The cell size for this memory definition in bytes.
   */
  size_t _cellSize;

  /**
  * The raw string values which shall be written to memory.
  */
  PositionedStringVector _values;

  /**
   * A function to process the values for this instruction.
   */
  ProcessValuesFunction _processValues;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_MEMORY_DEFINITION_DIRECTIVE_HPP */
